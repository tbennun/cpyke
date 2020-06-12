#include <cstdio>
#include <cpyke.h>

void my_function(cpy::ndarray<double> &a, int N, int M)
{
	cpy::ndarray<double> *b;
	cpyke("import numpy as np");

	int aaa = (int)cpyke("1 + 2");
	printf("A = 3 = %d\n", aaa);

	// Randomize some array
	b = (cpy::ndarray<double> *)cpyke("np.random.rand(n, m)", N, M);
	cpyke("print(b.shape, b.strides, b.ndim, b.itemsize, b.format)", *b);

	if (cpyke("np.allclose(a, b)", a, *b))
		printf("Equal, what are the odds?\n");
	else
		printf("Not equal, hooray\n");

	delete b;
}

int main(int argc, char **argv)
{
	double *a = new double[20 * 30];
	for (int i = 0; i < 20 * 30; ++i)
		a[i] = (double)i;

	cpy::ndarray<double> arr(a, 20, 30);
	my_function(arr, 20, 30);

	delete[] a;
	return 0;
}
