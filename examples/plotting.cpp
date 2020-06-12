#include <cstdio>
#include <random>
#include <vector>
#include <cpyke.h>

void randomize(std::vector<double> &vec)
{
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<double> dist(-1.0, 1.0);
    for (auto &v : vec)
        v = dist(gen);
}

int main(int argc, char **argv)
{
    // Ensure seaborn is installed
    if (!cpyke_pip_install("seaborn"))
    {
        printf("Cannot install seaborn\n");
        return 1;
    }

    // Import and set seaborn style
    cpyke(R"(
import matplotlib.pyplot as plt
import seaborn as sns
sns.set()
    )");

    // Randomize vector in the range [-1, 1]
    std::vector<double> vec(500);
    randomize(vec);

    // Plot and show violin of values
    cpyke("sns.violinplot(vec); plt.show()", cpy::ndarray<double>(vec));

    return 0;
}
