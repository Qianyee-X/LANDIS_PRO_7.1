#ifndef STANDS_H
#define STANDS_H

#include "Stand.h"

#ifdef HARVESTDLL_EXPORTS
#define DLLCLASS   __declspec( dllexport )
#else
#define DLLCLASS   __declspec( dllimport )
#endif

class DLLCLASS STANDS {
public :
    STANDS();
    virtual ~STANDS();
    int number();
    Stand* STANDS::operator() (int n);
    void construct();
private :
    int numStands;
    Stand* stands;
};

#undef DLLCLASS

#endif

