#ifndef BOOTSTRAPPING_H
#define BOOTSTRAPPING_H

#include <map>
#include <memory>
#include "Instruments.h" // Para que conozca la clase Instrument

class Bootstrapping {
public:
    Bootstrapping() {};
    
    // CORREGIDO: Añadidos los tipos al map
    typedef std::map<double, double> DiscountCurveType; 
    typedef std::map<double, std::shared_ptr<Instrument>> InstrumentsMapType; 
    
    DiscountCurveType operator() (const InstrumentsMapType& instruments) const;
    
    virtual ~Bootstrapping() = default;
};

#endif // BOOTSTRAPPING_H