#include "cs.h"

//  Author:  Corbin Champion
//  Last edited:  5/4/02
//  bwe2.h

typedef struct PRIVSTORE PRIVSTORE;

//structure to hold parameters for the bwe module
typedef struct {
	OPDS h;  //standard structure holding esr, ksmps etc
	float *result, *time, *ifilnam, *ampenv, *freqenv, *bwenv, *fadetime;     //holds output, followed by inputs
	PRIVSTORE *bwestore;  //a structure defined in bwe2.C
} BWE;
