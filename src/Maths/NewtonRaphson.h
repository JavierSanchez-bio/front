#ifndef NEWTON_RAPHSON_H
#define NEWTON_RAPHSON_H
#include <functional>
namespace Maths {
    double newtonRaphson(std::function<double(double)> f, 
                         std::function<double(double)> df, 
                         double x0);
}
#endif