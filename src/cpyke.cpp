#include <cstdio>
#include <cstdarg>
#include <iostream>

#include <pybind11/embed.h>
#include <pybind11/numpy.h>
namespace py = pybind11;

#define CPYKE_INTERNAL
#include <cpyke.h>

using namespace cpy;

py::scoped_interpreter guard{};

size_t size_from_pytype(cpyke_type type)
{
    switch (type & CPYKE_TYPEMASK)
    {
    case CPYKE_BOOL:
        return sizeof(bool);
    case CPYKE_CHAR:
        return sizeof(char);
    case CPYKE_SHORT:
        return sizeof(short);
    case CPYKE_INT:
        return sizeof(int);
    case CPYKE_LONG:
        return sizeof(long);
    case CPYKE_LONGLONG:
        return sizeof(long long);
    case CPYKE_FLOAT:
        return sizeof(float);
    case CPYKE_DOUBLE:
        return sizeof(double);
    default:
        return sizeof(void *);
    }
}

char format_from_pytype(cpyke_type type)
{
    // Returns type to struct format string
    char result;
    switch (type & CPYKE_TYPEMASK)
    {
    case CPYKE_BOOL:
        result = '?';
        break;
    case CPYKE_CHAR:
        result = 'b';
        break;
    case CPYKE_SHORT:
        result = 'h';
        break;
    case CPYKE_INT:
        result = 'i';
        break;
    case CPYKE_LONG:
        result = 'l';
        break;
    case CPYKE_LONGLONG:
        result = 'q';
        break;
    case CPYKE_FLOAT:
        result = 'f';
        break;
    case CPYKE_DOUBLE:
        result = 'd';
        break;
    default:
        result = 'P'; // Assume void *
    }

    if (result >= 'a' && result <= 'z' && (type & CPYKE_UNSIGNED))
        result -= 32;

    return result;
}

py::object pytype_from_pair(TypePair &pair)
{
    if (pair.type & CPYKE_NDARRAY)
    {
        // TODO: Move out
        py::buffer_info *buff = new py::buffer_info();
        py::buffer_info &buffer = *buff;

        ndarray<void> *arr = (ndarray<void> *)pair.arg;
        buffer.ptr = arr->data;
        buffer.itemsize = size_from_pytype(pair.type);
        buffer.ndim = arr->shape.size();
        buffer.shape.resize(buffer.ndim);
        buffer.strides.resize(buffer.ndim);
        buffer.readonly = false;
        for (ssize_t i = 0; i < buffer.ndim; ++i)
        {
            buffer.shape[i] = arr->shape[i];
            buffer.strides[i] = arr->strides[i];
        }
        buffer.format = std::string(1, format_from_pytype(pair.type));

        return py::memoryview(buffer);
    }
    else if (pair.type & CPYKE_POINTER) // Assume void* for now
        return py::cast(pair.arg);

    // POD types
    auto type = pair.type & CPYKE_TYPEMASK;
    switch (type)
    {
    case CPYKE_BOOL:
    {
        bool *val = (bool *)pair.arg;
        return py::bool_(*val);
    }
    case CPYKE_CHAR:
    {
        char *val = (char *)pair.arg;
        char vstr[2] = {*val, '\0'};
        return py::str(vstr);
    }
    case CPYKE_SHORT:
    {
        short *val = (short *)pair.arg;
        return py::int_(*val);
    }
    case CPYKE_INT:
    {
        int *val = (int *)pair.arg;
        return py::int_(*val);
    }
    case CPYKE_LONGLONG:
    {
        long long *val = (long long *)pair.arg;
        return py::int_(*val);
    }
    case CPYKE_FLOAT:
    {
        float *val = (float *)pair.arg;
        return py::float_(*val);
    }
    case CPYKE_DOUBLE:
    {
        double *val = (double *)pair.arg;
        return py::float_(*val);
    }
    default:
    {
        throw "Unsupported type";
    }
    }
}

result pyobj_to_result(py::object &obj)
{
    // Check POD types
    if (py::isinstance<py::bool_>(obj))
        return bool(py::bool_(obj));
    else if (py::isinstance<py::int_>(obj))
        return (long long)(py::int_(obj));
    else if (py::isinstance<py::float_>(obj))
        return double(py::float_(obj));
    else if (py::isinstance<py::array>(obj))
    {
        py::array arr(obj);
        void *ptr = (void *)arr.data();
        std::vector<size_t> shape(arr.ndim()), strides(arr.ndim());
        for (ssize_t i = 0; i < arr.ndim(); ++i)
        {
            shape[i] = arr.shape(i);
            strides[i] = arr.strides(i);
        }
        cpy::ndarray<void> *ndresult = new cpy::ndarray<void>(ptr, shape, strides);
        return result(ndresult, CPYKE_NDARRAY);
    }
    else
    {
        printf("Unrecognized result type from Python: %s\n",
               std::string(py::str(obj.get_type())).c_str());
        return nullptr;
    }
}

CPYKE_EXPORTED result _cpyke(const char *script, TypePair *data, int nargs)
{
    auto locals = py::dict();
    locals["statements"] = script;

    // Create list of arguments
    py::list args;
    for (int i = 0; i < nargs; ++i)
    {
        auto pytype = pytype_from_pair(data[i]);
        args.append(pytype);
    }
    locals["args"] = args;

    try
    {
        py::exec(R"(
                 import cpyke
                 result = cpyke.invoke(statements, args, globals())
                 )",
                 py::globals(), locals);
    }
    catch (const std::exception &ex)
    {
        printf("Caught Python exception: %s\n", ex.what());
        return nullptr;
    }

    py::object result = locals["result"];
    if (result.is_none())
        return nullptr;

    return pyobj_to_result(result);
}

CPYKE_EXPORTED bool cpyke_pip_install(const char *package)
{
    try
    {
        std::stringstream ss;
        ss << "import cpyke; cpyke.pip_install('" << package << "')" << std::endl;
        py::exec(ss.str());
    }
    catch (const std::exception &ex)
    {
        printf("Caught Python exception: %s\n", ex.what());
        return false;
    }
    return true;
}
