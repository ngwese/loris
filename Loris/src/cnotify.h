#ifndef __Loris_c_notify__
#define __Loris_c_notify__

/*
 *	cnotify.h
 *
 *	c-callable notification
 *
 */
 
#ifdef __cplusplus
	extern "C" {
#endif

void notify( const char * cstr );
void fatalError( const char * cstr );

#ifdef __cplusplus
	}	// end of extern "C"
#endif

#endif	/* ndef __Loris_c_notify__ */