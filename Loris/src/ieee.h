#ifndef __IEEE_H__
#define __IEEE_H__
/*
	From arun Chandra's code, see ieee.c for the original source.
	
	Added namespace and typedef for ext80. -kel	
*/

#ifdef __cplusplus
namespace IEEE {
extern "C" {
#endif

typedef struct {
	char data[10];
} extended80;

extern void ConvertToIeeeExtended(double num, extended80 * x) ;
extern double ConvertFromIeeeExtended(extended80 x) ;

#ifdef __cplusplus
}	//	end extern "C"
}	//	end namespace
#endif

#endif 
