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
#include "Partial.h"
#include "Handle.h"	//	Batov's template Handle class	

#if !defined( NO_LORIS_NAMESPACE )
//	begin namespace
namespace Loris {
#endif

class Envelope;

// ---------------------------------------------------------------------------
//	class Morpher
//
//	Hey, how about a comment?
//
class Morpher
{
//	-- instance variables --
	//	reference-counted smart pointers from Batov:
	//	(these Envelopes will never be modified by the Morpher
	//	class, but Morpher should be able to grant non-const
	//	access to them, so they are not const Handles)
	Handle< Envelope > _freqFunction;	//	frequency morphing function
	Handle< Envelope > _ampFunction;	//	amplitude morphing function
	Handle< Envelope > _bwFunction;		//	bandwidth morphing function
	
	PartialList _partials;	//	collect Partials here
			
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
	PartialList & partials( void ) { return _partials; }
	const PartialList & partials( void ) const { return _partials; }
	
//	-- helpers --
protected:	
//	single Partial morph:
//	(core morphing operation, called by morph() and crossfade())
	void morphPartial( const Partial & p1, const Partial & p2, int assignLabel );
	
//	crossfade Partials with no correspondences:
//	(crossfaded Partials are unlabeled, or assigned the 
//	default label, 0)
	void crossfade( PartialList::const_iterator begin0, 
					PartialList::const_iterator end0,
					PartialList::const_iterator begin1, 
					PartialList::const_iterator end1 );

//	-- unimplemented until useful --
private:
	Morpher( const Morpher & other );
	Morpher & operator= ( const Morpher & other );

};	//	end of class Morpher

#if !defined( NO_LORIS_NAMESPACE )
}	//	end of namespace Loris
#endif

#endif	// ndef INCLUDE_MORPHER_H
