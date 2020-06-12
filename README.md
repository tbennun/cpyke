# 📍 CPyKE - C/Python Kludge Eliminator 📍

Have you ever noticed how long boilerplate C++ code takes only several lines 
in Python? Want to avoid wrapper scripts for simple plotting and dataframe 
analysis?

Keep the productivity of Python and the performance of C++ with cpyke!

cpyke links as a separate dynamic library (.so/.dll), so no special flags or 
CMake modules are necessary. Compile cpyke once against Python and reuse in any
application directly.

## Executing commands

As easy as `cpyke`:

```cpp
int a = cpyke("1 + 2");
printf("a = %d\n", a); // a = 3

cpyke("import numpy as np");
cpy::ndarray<double> b = cpyke("np.random.rand(something, else_)", a, 4);
cpyke("print('shape:', b.shape)", b); // shape: (3, 4)
```

## Installing packages

With `cpyke_pip_install`, you can install packages directly from your C++ 
program, ensuring anyone who runs your application will have the right modules.

```cpp
cpyke("import seaborn as sns"); // ModuleNotFoundError
cpyke_pip_install("seaborn");
cpyke("import seaborn as sns"); // import successful!
```

## Behind the scenes
cpyke is a seamless wrapper around the amazing [pybind11](https://github.com/pybind/pybind11) library.
The cpyke C++ library calls the cpyke Python module, which analyzes the given 
code to find undefined variables (by order of appearance), mapping them to the arguments.

For example: `cpyke("print(a, b.c)", d, e);` will map `d` in C++ to `a` in Python,
and `e` in C++ to `b` in Python.

## Compiling and installing cpyke
```shell
$ git clone --recursive https://github.com/tbennun/cpyke.git
$ cd cpyke
$ mkdir build
$ cd build

$ cmake .. 
# If a specific version of Python is required, use 
# cmake -DPYBIND11_PYTHON_VERSION=3.7 ..
# or
# cmake -DPYTHON_EXECUTABLE=/path/to/python ..
# instead.

$ make
# To install: 
$ sudo make install
# If you wish to install manually, ensure cpyke.h and libcpyke.so are accessible
# (using CPATH and LD_LIBRARY_PATH) and install the cpyke Python module with:
# python -m pip install /path/to/cpyke
```

## Linking with cpyke
Nothing more than adding cpyke as a library. For example:
`g++ myfile.cpp -lcpyke -o myfile`

## Dependencies and supported compilers
All dependencies are included as git submodules. Compiling cpyke or linking 
with it requires only a C++11 compiler (GCC 4.8 or newer, clang 3.3 or newer, 
VS2015 update 3 or newer etc.).

## Contributing

cpyke is licensed under the New BSD (3-clause) license. 
Any contributions are welcome!
