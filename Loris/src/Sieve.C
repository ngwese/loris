/*
 * This works with the Loris class library.
 *
 * Sieve.C
 *
 * Implementation of class Sieve.
 *
 * Lippold Haken, 20 Jan 2001
 *
 */

#if HAVE_CONFIG_H
	#include <config.h>
#endif

#include<Sieve.h>
#include<Partial.h>
#include<Breakpoint.h>
#include<Exception.h>
#include<PartialUtils.h>
#include<Notifier.h>
#include <algorithm>
#include <list>

//	begin namespace
namespace Loris {

// ---------------------------------------------------------------------------
//	sieve_aux STATIC
// ---------------------------------------------------------------------------
//	iterate over all other Partials with same labeling.
//  if any other partial has time-overlap with this partial,
//  and if this partial is not the longer of the two,
//  then remove the label from this partial.
//
//  return 1 if we remove the partial, else return zero.
//	
static int sieve_aux( Partial &src, double minGapTime,
				   		 std::list< Partial >::const_iterator start,
				   		 std::list< Partial >::const_iterator end)
{
	std::list< Partial >::const_iterator it;
	for ( it = start; it != end; ++it ) 
	{
		//	skip the source Partial:
		//	(identity test: compare addresses)
		if ( &(*it) == &src )
			continue;
			
		//	skip if other partial is already sifted out.
		if ( it->label() == 0 )
			continue;

		//  skip if no overlap
		if ( src.startTime() > it->endTime() + minGapTime 
					|| src.endTime() + minGapTime < it->startTime() )
			continue;
		
		//  are we longer duration?
		if ( src.duration() > it->duration() ) 
			continue;
		
		//  we overlap wth something longer; remove us from this label.
		/*
		debugger << "Partial starting " << src.startTime() << ", " 
				 << src.frequencyAt( src.startTime() ) << " ending " 
				 << src.endTime()  << ", " << src.frequencyAt( src.endTime() ) 
				 << " zapped by Partial starting " 
				 << it->startTime() << ", " << it->frequencyAt( it->startTime() )
				 << " ending " << it->endTime() << ", " 
				 << it->frequencyAt( it->endTime() ) << endl;
		*/
		src.setLabel( 0 );
		return 1;
	}	//	end iteration over Partial range
	
	return 0;
}


// ---------------------------------------------------------------------------
//	Sieve constructor
// ---------------------------------------------------------------------------
//
Sieve::Sieve( double minGapTime )
{
	_minGapTime = minGapTime;
	Assert( _minGapTime > 0.0 );
}

// ---------------------------------------------------------------------------
//	Sieve destructor
// ---------------------------------------------------------------------------
//
Sieve::~Sieve( void )
{
}

// ---------------------------------------------------------------------------
//	sift
// ---------------------------------------------------------------------------
//	Sift labeled Partials: 
//  If any two partials with same label overlap in time,
//  keep only the longer of the two partials.
//  Set the label of the shorter duration partial to zero.
//
void 
Sieve::sift( std::list<Partial> & l )
{
	int zapped = 0;

	//	sort the std::list< Partial > by label:
	l.sort( PartialUtils::label_less() );
	
	// 	iterate over labels and sift each one:
	std::list<Partial>::iterator sieve_begin = l.begin();
	while ( sieve_begin != l.end() )
	{
		int label = sieve_begin->label();
		
		//	first the first element in l after sieve_begin
		//	having a label not equal to 'label':
		std::list<Partial>::iterator sieve_end = 
			std::find_if( sieve_begin, l.end(), 
						  std::not1( std::bind2nd( PartialUtils::label_equals(), label ) ) );

		//  sift all partials with this label
		if ( label != 0 )
		{
			std::list< Partial >::iterator it;
			for ( it = sieve_begin; it != sieve_end; ++it ) 
			{
				zapped += sieve_aux( *it, _minGapTime, sieve_begin, sieve_end );
			} 
		}
		
		//	advance Partial list iterator:
		sieve_begin = sieve_end;
	}

	std::cout  << "sifted out " << zapped << " of " << l.size() << endl;
}

}	//	end of namespace Loris

