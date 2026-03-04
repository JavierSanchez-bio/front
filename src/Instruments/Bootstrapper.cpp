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

    // 2. Iteramos sobre cada instrumento para encontrar el tipo cero de su pilar
    for (const auto& pair : instruments_) {
        double T = pair.first;
        auto inst = pair.second;

        // Función objetivo f(z): Añadimos el tipo 'z' en el tiempo 'T' y vemos el NPV
        // Buscamos la raíz f(z) = 0 (que el instrumento cotice a la par)
        std::function<double(double)> f = [&](double z) {
            curve_->add_pillar(T, z);
            return inst->price(); 
        };

        // Derivada numérica para el Newton-Raphson (Diferencias finitas centrales)
        // df/dz ≈ (f(z + h) - f(z - h)) / 2h
        std::function<double(double)> df = [&](double z) {
            double h = 1e-5;
            double f_plus = f(z + h);
            double f_minus = f(z - h);
            return (f_plus - f_minus) / (2.0 * h);
        };

        // Estimación inicial (seed) del 5%
        double initial_guess = 0.05; 
        
        // Si ya hay puntos en la curva, usamos el último tipo calculado como mejor aproximación
        if (T > 0.1) {
            initial_guess = curve_->get_zc(T - 0.01);
        }

        // 3. ¡Magia! El solver encuentra el tipo cero exacto para este vencimiento
        double z_solved = Maths::newtonRaphson(f, df, initial_guess);

        // 4. Fijamos el pilar definitivo en la curva
        curve_->add_pillar(T, z_solved);
        
        // (Opcional) Imprimimos el progreso para ver cómo se construye la curva
        std::cout << "Calibrado pilar T=" << T << " -> Tipo Cero: " << (z_solved * 100.0) << "%" << std::endl;
    }
}