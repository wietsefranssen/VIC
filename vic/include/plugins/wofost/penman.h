
#ifndef PENMAN_H
#define PENMAN_H

typedef struct ETP {
    float E0;
    float ES0;
    float ET0;
} Etp;

Etp Penman;

extern float limit(float a, float b, float c);

typedef struct EVP {
    float MaxEvapWater;
    float MaxEvapSoil;
    float MaxTranspiration;
} EVP;

EVP Evtra;


#endif	// PENMAN_H

