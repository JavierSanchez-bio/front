#include "NewtonRaphson.h"
#include <cmath>
namespace Maths {
    double newtonRaphson(std::function<double(double)> f, 
                         std::function<double(double)> df, 
                         double x0) {
        double x = x0;
        for (int i = 0; i < 100; ++i) {
            double fx = f(x);
            double dfx = df(x);
            if (std::abs(dfx) < 1e-12) break;
            double x_next = x - fx / dfx;
            if (std::abs(x_next - x) < 1e-7) return x_next;
            x = x_next;
        }
        return x;
    }
}