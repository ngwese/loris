#ifndef INCLUDE_MORPHER_H
#define INCLUDE_MORPHER_H
// ===========================================================================
//	Morpher.h
//	
//	The Morpher object performs sound morphing (cite Lip's papers, and the book)
//	by interpolating Partial parmeter envelopes of corresponding Partials in
//	a pair of source sounds. The correspondences are established by labeling.
//	The Morpher object collects morphed Partials in a list<Partial>, that can
//	be accessed by clients.
//
//	-kel 15 Oct 99
//
// ===========================================================================
#include "Partial.h"	//	needed only for PartialList definition
#include "Handle.h"	 	//	Batov's template Handle class	

#if !defined( NO_LORIS_NAMESPACE )
//	begin namespace
namespace Loris {
#endif

class Envelope;
class Morpher_imp;

// ---------------------------------------------------------------------------
//	class Morpher
//
//	Fully-insulating class encapsulating manipulations involving 
//	linear interpolation of Partial parameter envelopes. The implementation
//	is entirely defined in the Morpher_imp class, in Morpher.C.
//
class Morpher
{
//	-- instance variable --
	Morpher_imp * _imp;

//	-- public interface --
public:
//	construction:
	Morpher( Handle< Envelope > f );
	Morpher( Handle< Envelope > ff, Handle< Envelope > af, Handle< Envelope > bwf );
	~Morpher( void );
	
//	morphing:
//	Morph two sounds (collections of Partials labeled to indicate
//	correspondences) into a single labeled collection of Partials.
	void morph( PartialList::const_iterator begin0, 
				PartialList::const_iterator end0,
				PartialList::const_iterator begin1, 
				PartialList::const_iterator end1 );

//	morphing functions access/mutation:	
	void setFrequencyFunction( Handle< Envelope > f );
	void setAmplitudeFunction( Handle< Envelope > f );
	void setBandwidthFunction( Handle< Envelope > f );

	Handle< Envelope > frequencyFunction( void );
	Handle< const Envelope > frequencyFunction( void ) const;

	Handle< Envelope > amplitudeFunction( void );
	Handle< const Envelope > amplitudeFunction( void ) const;

	Handle< Envelope > bandwidthFunction( void );
	Handle< const Envelope > bandwidthFunction( void ) const;
	
	
//	PartialList access:
	PartialList & partials( void ); 
	const PartialList & partials( void ) const; 

//	-- unimplemented until useful --
private:
	Morpher( const Morpher & other );
	Morpher & operator= ( const Morpher & other );

};	//	end of class Morpher

#if !defined( NO_LORIS_NAMESPACE )
}	//	end of namespace Loris
#endif

#endif	// ndef INCLUDE_MORPHER_H
