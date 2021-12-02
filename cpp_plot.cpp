#include <unistd.h>
#include <stdio.h>
#include <vector>
#include <cmath>
#include "cpp_plot.h"

int main() {
    GNUPlot plt;

    std::vector<float> x;
    std::vector<float> y;
    for (int i = 0; i < 10; i++) {
        float t = static_cast<float>(i);
        x.push_back(t);
        y.push_back(sin(M_PI * (t / 10.0)));
    }

    plt.plot(x, y, "Sin()");
    plt.scatter(y, x, "Scatter");

    for (int i = 0; i < 10; i++) {
        y[i] += 1;
    }
    plt.plot(x, y, "Offset", true, "#B0A510", 0, 5);

    plt.show();
    return 0;
}
