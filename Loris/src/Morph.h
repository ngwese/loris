#ifndef __Loris_morph__
#define __Loris_morph__
// ===========================================================================
//	Morph.h
//	
//	The Morph object performs sound morphing (cite Lip's papers, and the book)
//	by interpolating Partial parmeter envelopes of corresponding Partials in
//	a pair of source sounds. The correspondences are established by labeling.
//	The Morph object collects morphed Partials in a list<Partial>, that can
//	be accessed by clients.
//
//	-kel 15 Oct 99
//
// ===========================================================================
#include "LorisLib.h"
#include "Partial.h"
#include "Map.h"
#include <list>
#include <set>
#include <memory>	//	for auto_ptr

Begin_Namespace( Loris )

// ---------------------------------------------------------------------------
//	class Morph
//
//	This class does not own PartialIterators or implement the 
//	PartialIteratorOwner interface. There's a good reason for this:
//	the morphing process needs to query the Partials directly using 
//	parameterAt() members, which are not available through the
//	PartialIterator interface. 
//
//	Iterators could be specified and used by the distillers in morph()
//	or by collectByLabel(), but neither of these is called by directly
//	by crossfadePartials(). Find a solution to this. Actually, only do 
//	this if a sensible way can be found to put it in the core morphing
//	operation: morphPartial().
//
class Morph : public PartialCollector
{
//	-- public interface --
public:
//	construction:
	Morph( void );
	Morph( const Map & f );
	Morph( const Map & ff, const Map & af, const Map & bwf );
	Morph( const Morph & other );
	
	//~Morph( void );	//	use compiler-generated destructor
	
	Morph & operator= ( const Morph & other );

//	morph two sounds (collections of Partials labeled to indicate
//	correspondences) into a single one:
#if !defined(No_template_members)
	template < class Iter0, class Iter1 >
	void morph( Iter0 begin0, Iter0 end0, Iter1 begin1, Iter1 end1 )
#else
	void morph( std::list<Partial>::const_iterator begin0, 
				std::list<Partial>::const_iterator end0,
				std::list<Partial>::const_iterator begin1, 
				std::list<Partial>::const_iterator end1 )
#endif
	{
		//	find label range:
		std::set< int > labels;
		collectLabels( begin0, end0, labels );
		collectLabels( begin1, end1, labels );
		
		//	loop over lots of labels:
		//for ( int label = _minlabel; label < _maxlabel; ++label ) {
		for ( std::set< int >::iterator it = labels.begin(); it != labels.end(); ++it ) {
			int label = *it;
			
			//	collect Partials in plist1:
			std::list<Partial> sublist1;
			collectByLabel( begin0, end0, sublist1, label );

			//	collect Partials in plist2:
			std::list<Partial> sublist2;
			collectByLabel( begin1, end1, sublist2, label );
			
			if ( label == _crossfadelabel ) {
				debugger << "crossfading Partials labeled " << label << endl;
				crossfadeLists( sublist1, sublist2 );
			}
			else {
				debugger << "morphing " << sublist1.size() << " and "
						 << sublist2.size() << " partials with label " << label << endl;
						 
				morphLists( sublist1, sublist2, label );
			}
		}
	}

#if !defined(No_template_members)
	template < class Iter0, class Iter1 >
	void operator() ( Iter0 begin0, Iter0 end0, Iter1 begin1, Iter1 end1 )
#else
	void operator() ( std::list<Partial>::const_iterator begin0, 
					  std::list<Partial>::const_iterator end0,
					  std::list<Partial>::const_iterator begin1, 
					  std::list<Partial>::const_iterator end1 )
#endif
		{ morph( begin0, end0, begin1, end1 ); }

#if !defined(No_template_members)
	template < class Collection0, class Collection1 >
	void morph( const Collection0 & col0, const Collection1 & col1 )
#else
	void morph( const std::list<Partial> & col0, 
				const std::list<Partial> & col1 )
#endif
		{ morph( col0.begin(), col0.end(), col1.begin(), col1.end() ); }

#if !defined(No_template_members)
	template < class Collection0, class Collection1 >
	void operator() ( const Collection0 & col0, const Collection1 & col1 )
#else
	void operator() ( const std::list<Partial> & col0, 
					  const std::list<Partial> & col1 )
#endif
		{ morph( col0, col1 ); }

//	crossfade Partials with no correspondences:
//
//	The Partials in the  specified range are considered to have 
//	no correspondences, so they are just faded in and out, and not 
//	actually morphed. This is the same as morphing each with an 
//	empty Partial. crossfadeFromPartials() treats the Partials
//	as part of the sound corresponding to a morph function of 0, 
//	crossfadeToPartials() treats the Partials as part of the sound 
//	corresponding to a morph function of 1.
//	These members are called by morph().
//
//	If template members aren't available, accept only list iterators.
//
#if !defined(No_template_members)
	template < class Iter >
	void crossfadeFromPartials( Iter begin, Iter end )
#else
	void crossfadeFromPartials( std::list<Partial>::const_iterator begin, 
								std::list<Partial>::const_iterator end )
#endif
	{
		//	crossfade Partials corresponding to a morph weight of 0:
		while ( begin != end )
			morphPartial( * begin++, Partial(), _crossfadelabel );	
	}
		
#if !defined(No_template_members)
	template < class Iter >
	void crossfadeToPartials( Iter begin, Iter end )
#else
	void crossfadeToPartials( std::list<Partial>::const_iterator begin, 
							  std::list<Partial>::const_iterator end )
#endif
	{		
		//	crossfade Partials corresponding to a morph weight of 1:
		while ( begin != end )
			morphPartial( Partial(), * begin++, _crossfadelabel );
	}
	
//	single Partial morph:
//	This is the core morphing operation, called by morph() and crossfade().
	void morphPartial( const Partial & p1, const Partial & p2, int assignLabel = 0 );
	
//	morphing functions access/mutation:	
	void setFrequencyFunction( const Map & f );
	void setAmplitudeFunction( const Map & f );
	void setBandwidthFunction( const Map & f );

	const Map & frequencyFunction( void ) const { return * _freqFunction; }
	const Map & amplitudeFunction( void ) const { return * _ampFunction; }
	const Map & bandwidthFunction( void ) const { return * _bwFunction; }
	
	Map & frequencyFunction( void ) { return * _freqFunction; }
	Map & amplitudeFunction( void ) { return * _ampFunction; }
	Map & bandwidthFunction( void ) { return * _bwFunction; }
	
//	-- helpers --
protected:	
	void collectByLabel( std::list<Partial>::const_iterator start, 
						 std::list<Partial>::const_iterator end, 
						 std::list<Partial> & collector, 
						 int label);

//	morph two list of Partials:
//	(distills the each list into a single partial for morphing,
//	the morphed Partial is assigned the specified label)
	void morphLists( const std::list<Partial> & fromlist, 
					 const std::list<Partial> & tolist,
					 int assignLabel = 0 );
	
//	crossfade two list of Partials:
//	(the morphed Partial is assigned the crossfade label)
	void crossfadeLists( const std::list<Partial> & fromlist, 
						 const std::list<Partial> & tolist );


//	collect Partial labels in a set:	
//	This only has to be a template because it is 
//	called by another member with the same template
//	parameter (above).
#if !defined(No_template_members)
	template < class Iter >
	void collectLabels( Iter begin, Iter end, std::set<int> & labels )
#else
	void collectLabels( std::list<Partial>::const_iterator begin, 
						std::list<Partial>::const_iterator end, 
						std::set<int> & labels )
#endif
	{
		while ( begin != end ) {
			labels.insert( begin->label() );
			++begin;
		}
	}
						
//	-- instance variables --
	//	morphing functions:
	std::auto_ptr< Map > _freqFunction;
	std::auto_ptr< Map > _ampFunction;
	std::auto_ptr< Map > _bwFunction;
	
	//	label for partials that should be crossfaded,
	//	instead of morphing:
	int _crossfadelabel;
		
};	//	end of class Morph

End_Namespace( Loris )

#endif	// ndef __Loris_morph__
