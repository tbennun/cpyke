// Defines all supported native cpyke types
#ifndef __CPYKE_TYPES_H
#define __CPYKE_TYPES_H

#include <vector>

namespace cpy
{

    enum cpyke_type
    {
        CPYKE_VOID = 0,
        CPYKE_BOOL,
        CPYKE_CHAR,
        CPYKE_SHORT,
        CPYKE_INT,
        CPYKE_LONG,
        CPYKE_LONGLONG,
        CPYKE_FLOAT,
        CPYKE_DOUBLE,
        CPYKE_POINTER,
        // Mask that can be used to determine basic type
        CPYKE_TYPEMASK = 0x10 - 1,
        // Other types that can be used as a bitmap
        CPYKE_UNSIGNED = 0x100,
        CPYKE_NDARRAY = 0x1000
    };

    template <typename T>
    struct TypeToCpyke
    {
        enum
        {
            cpyketype = CPYKE_VOID
        };
    };

#define CPYKE_DEFTYPE(type, cpytype) \
    template <>                      \
    struct TypeToCpyke<type>         \
    {                                \
        enum                         \
        {                            \
            cpyketype = (cpytype)    \
        };                           \
    }

    // POD types
    CPYKE_DEFTYPE(bool, CPYKE_BOOL);
    CPYKE_DEFTYPE(char, CPYKE_CHAR);
    CPYKE_DEFTYPE(unsigned char, CPYKE_CHAR | CPYKE_UNSIGNED);
    CPYKE_DEFTYPE(short, CPYKE_SHORT);
    CPYKE_DEFTYPE(unsigned short, CPYKE_SHORT | CPYKE_UNSIGNED);
    CPYKE_DEFTYPE(int, CPYKE_INT);
    CPYKE_DEFTYPE(unsigned int, CPYKE_INT | CPYKE_UNSIGNED);
    CPYKE_DEFTYPE(long, CPYKE_LONG);
    CPYKE_DEFTYPE(unsigned long, CPYKE_LONG | CPYKE_UNSIGNED);
    CPYKE_DEFTYPE(long long, CPYKE_LONGLONG);
    CPYKE_DEFTYPE(unsigned long long, CPYKE_LONGLONG | CPYKE_UNSIGNED);
    CPYKE_DEFTYPE(float, CPYKE_FLOAT);
    CPYKE_DEFTYPE(double, CPYKE_DOUBLE);

    // Pointer types
    template <typename T>
    struct TypeToCpyke<T *>
    {
        enum
        {
            cpyketype = CPYKE_POINTER | TypeToCpyke<T>::cpyketype
        };
    };

    // Array types
    template <typename T>
    struct TypeToCpyke<std::vector<T>>
    {
        enum
        {
            cpyketype = CPYKE_NDARRAY | TypeToCpyke<T>::cpyketype
        };
    };

    // Native ND-array type
    template <typename T>
    struct ndarray
    {
        void *data;
        std::vector<size_t> shape;
        std::vector<size_t> strides;

        // From std::vector
        ndarray(std::vector<T> &v) : data(v.data()), shape{v.size()}, strides{sizeof(T)} {}

        // From std::vector with shape
        template <typename... Dim>
        ndarray(std::vector<T> &v, Dim... shp) : ndarray(v.data(), shp...) {}

        // From pointer and shape
        template <typename... Dim>
        ndarray(T *ptr, Dim... shp) : data(ptr), shape{static_cast<size_t>(shp)...}
        {
            int ndim = sizeof...(shp);
            // Assume default strides (no padding)
            strides.resize(ndim);
            size_t curstride = sizeof(T);
            for (int i = ndim - 1; i >= 0; --i)
            {
                strides[i] = curstride;
                curstride *= shape[i];
            }
        }

        // From pointer, shape, and strides
        ndarray(T *ptr, const std::vector<size_t> &shp,
                const std::vector<size_t> &stride) : data(ptr), shape(shp), strides(stride) {}
    };

    template <typename T>
    struct TypeToCpyke<ndarray<T>>
    {
        enum
        {
            cpyketype = CPYKE_NDARRAY | TypeToCpyke<T>::cpyketype
        };
    };

    // Type and pointer pair structure (for passing along to cpyke)
    struct TypePair
    {
        cpyke_type type;
        void *arg;

        template <typename T>
        TypePair(T &a) : type(static_cast<cpyke_type>(TypeToCpyke<T>::cpyketype)), arg(&a) {}
    };

} // namespace cpy

#endif // __CPYKE_TYPES_H
