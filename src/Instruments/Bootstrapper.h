#ifndef BOOTSTRAPPING_H
#define BOOTSTRAPPING_H

#include <map>
#include <memory>
#include "Instruments.h"

class Bootstrapping {
public:
    Bootstrapping() {};
    
    typedef std::map<double, double> DiscountCurveType; 
    typedef std::map<double, std::shared_ptr<Instrument>> InstrumentsMapType; 
    
    DiscountCurveType operator() (const InstrumentsMapType& instruments) const;
    
    virtual ~Bootstrapping() = default;
};

#endif