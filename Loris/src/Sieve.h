#ifndef INCLUDE_SIEVE_H
#define INCLUDE_SIEVE_H
/*
 * This works with the Loris class library.
 *
 * Sieve.h
 *
 * Definition of class Sieve.
 *
 * Lippold Haken, 20 Jan 2001
 *
 */

#include <list>

#if !defined( NO_LORIS_NAMESPACE )
//	begin namespace
namespace Loris {
#endif

class Partial;

// ---------------------------------------------------------------------------
//	class Sieve
//
//	Sift labeled Partials: 
//  If any two partials with same label overlap in time,
//  keep only the longer of the two partials.
//  Set the label of the shorter duration partial to zero.
//
//
class Sieve
{
//	-- instance variables --
	double _minGapTime;

//	-- public interface --
public:
//	construction:	
	Sieve( double minGapTime );
	~Sieve( void );
	
//	sift:
	void sift( std::list<Partial> & l );

//	-- unimplemented --
private:
	Sieve( const Sieve & other );
	Sieve & operator= ( const Sieve & other );
	
};	//	end of class Sieve

#if !defined( NO_LORIS_NAMESPACE )
}	//	end of namespace Loris
#endif

#endif /* ndef INCLUDE_SIEVE_H */
