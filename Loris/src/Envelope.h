#ifndef INCLUDE_ENVELOPE_H
#define INCLUDE_ENVELOPE_H
// ===========================================================================
//	Envelope.h
//	
//	Envelope is an abstract base class representing a generic real (double) 
//	function of one real (double) argument. 
//
//	-kel 21 July 2000
//
// ===========================================================================

#if !defined( NO_LORIS_NAMESPACE )
//	begin namespace
namespace Loris {
#endif

// ---------------------------------------------------------------------------
//	class Envelope
//
//	Abstract base class, specifying interface ( valueAt() ).
//	Derived classes must implement valueAt().
//
class Envelope
{
//	-- public interface --
public:
	virtual double valueAt( double x ) const = 0;	
	virtual ~Envelope( void );
	
//	-- protected interface --
protected:
	//	protect construction:
	Envelope( void );
	Envelope( const Envelope & );
	
//	-- unimplemented --
private:
	Envelope & operator= ( const Envelope & );

};	//	end of abstract class Envelope

#if !defined( NO_LORIS_NAMESPACE )
}	//	end of namespace Loris
#endif

#endif	// ndef INCLUDE_PARTIALLIST_H
