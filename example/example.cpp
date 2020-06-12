#include <cassert>
#include <cstdio>
#include <cpyke.h>

void my_function(cpy::ndarray<double> &a, int N, int M)
{
    cpyke("import numpy as np");

    int result = cpyke("1 + 2");
    assert(result == 3);

    // Randomize some array
    cpy::ndarray<double> b = cpyke("arr = np.random.rand(n, m); arr", N, M);

    // Print information from Python and C++
    cpyke(R"(
print("Array format:", b.shape, b.strides, b.ndim, b.itemsize, b.format)
print("Some values:", b[0, 0], b[10, 11]);
          )",
          b);
    printf("Same values from C++: %f %f\n", b.data[0], b.data[10 * M + 11]);

    if (cpyke("np.allclose(a, b)", a, b))
        assert(false && "Equal, what are the odds?");
    else
        printf("The arrays are not equal, hooray!\n");
}

int main(int argc, char **argv)
{
    // std::vectors are also supported
    double *a = new double[20 * 30];
    for (int i = 0; i < 20 * 30; ++i)
        a[i] = (double)i;

    cpy::ndarray<double> arr(a, 20, 30);
    my_function(arr, 20, 30);

    delete[] a;
    return 0;
}
