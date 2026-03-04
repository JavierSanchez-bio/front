#include "Bootstrapper.h"
#include "../Maths/NewtonRaphson.h"
#include <algorithm>
#include <functional>
#include <iostream>

Bootstrapper::Bootstrapper(std::shared_ptr<ZeroCouponCurve> curve)
    : curve_(std::move(curve)) 
{
}

void Bootstrapper::add_instrument(double maturity_t, std::shared_ptr<Instrument> instrument) 
{
    instruments_.emplace_back(maturity_t, std::move(instrument));
}

void Bootstrapper::build_curve() 
{
    // 1. Es vital ordenar los instrumentos de menor a mayor vencimiento
    std::sort(instruments_.begin(), instruments_.end(),
              [](const auto& a, const auto& b) { return a.first < b.first; });

    double last_t = 0.0; // <-- NUEVO: Controlamos cuál fue el último pilar añadido

    // 2. Iteramos sobre cada instrumento para encontrar el tipo cero de su pilar
    for (const auto& pair : instruments_) {
        double T = pair.first;
        auto inst = pair.second;

        // Función objetivo f(z)
        std::function<double(double)> f = [&](double z) {
            curve_->add_pillar(T, z);
            return inst->price(); 
        };

        // Derivada numérica
        std::function<double(double)> df = [&](double z) {
            double h = 1e-5;
            double f_plus = f(z + h);
            double f_minus = f(z - h);
            return (f_plus - f_minus) / (2.0 * h);
        };

        // Estimación inicial por defecto
        double initial_guess = 0.05; 
        
        // CORRECCIÓN: Si ya hemos calibrado algún pilar anterior, usamos ese valor
        if (last_t > 0.0) {
            initial_guess = curve_->get_zc(last_t);
        }

        // 3. ¡Magia! El solver encuentra el tipo cero exacto para este vencimiento
        double z_solved = Maths::newtonRaphson(f, df, initial_guess);

        // 4. Fijamos el pilar definitivo en la curva
        curve_->add_pillar(T, z_solved);
        
        // Actualizamos nuestro control del último pilar
        last_t = T; 
        
        std::cout << "Calibrado pilar T=" << T << " -> Tipo Cero: " << (z_solved * 100.0) << "%" << std::endl;
    }
}