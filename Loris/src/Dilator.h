#ifndef __Loris_dilator__
#define __Loris_dilator__

// ===========================================================================
//	Dilator.h
//
//	Dilator is a class of objects for temporally dilating and compressing
//	Partials by specifying initial and target times of temporal features.
//
//	-kel 26 Oct 99
//
// ===========================================================================

#include "LorisLib.h"

#include <vector>

Begin_Namespace( Loris )

class Partial;

// ---------------------------------------------------------------------------
//	class Dilator
//
class Dilator
{
//	-- public interface --
public:
//	construction:
	Dilator( void ) {}
	Dilator( const std::vector< double > & init, const std::vector< double > & tgt );
	
	//	use compiler-generated:
	// Dilator( const Dilator & );
	// ~Dilator( void );
	
//	time-point access:
	const std::vector< double > & initialTimePoints( void ) const { return _initial; }
	const std::vector< double > & targetTimePoints( void ) const { return _target; }
	
	void setTimePoints( const std::vector< double > & init, 
						const std::vector< double > & tgt );
	
//	dilation:
	Partial & dilate( Partial & p ) const;
	
//	-- template member functions for timepoint specification --
//
//	Strictly speaking, we can do without these if necessary.
//
#if !defined(No_template_members)
//	template constructors from iterators:
	template < class Iter1, class Iter2 >
	Dilator( const Iter1 & ibegin, const Iter1 & iend, 
			 const Iter2 & tbegin, const Iter2 & tend )
	{
		setTimePoints( ibegin, iend, tbegin, tend );
	}
	
	template < class Iter1, class Iter2 >
	Dilator( const Iter1 & ibegin, const Iter2 & tbegin, int n )
	{
		setTimePoints( ibegin, tbegin, n );
	}
	
//	template time point specification from iterators:
//	(just makes temporary vectors)
	template < class Iter1, class Iter2 >
	void setTimePoints( const Iter1 & ibegin, const Iter1 & iend, 
						const Iter2 & tbegin, const Iter2 & tend )
	{
		setTimePoints( vector< double >( ibegin, iend ), 
					   vector< double >( tbegin, tend ) );
	}

	template < class Iter1, class Iter2 >
	void setTimePoints( const Iter1 & ibegin, const Iter2 & tbegin, int n )
	{
		vector< double > i, t;
		while ( n > 0 ) {
			i.push_back( *(ibegin++) );
			t.push_back( *(tbegin++) );
			--n;
		}
		setTimePoints( i, t );
	}
#endif	//	template members allowed

//	-- instance variables --
private:
	std::vector< double > _initial;
	std::vector< double > _target;
	
};	//	end of class Dilator


End_Namespace( Loris )

#endif	// ndef __Loris_dilator__