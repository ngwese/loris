/*
 * This is the Loris C++ Class Library, implementing analysis, 
 * manipulation, and synthesis of digitized sounds using the Reassigned 
 * Bandwidth-Enhanced Additive Sound Model.
 *
 * Loris is Copyright (c) 1999-2000 by Kelly Fitz and Lippold Haken
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY, without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 *
 *	lorisgens.C
 *
 *	Implementation of Csound unit generators supporting bandwidth-enhanced 
 *	synthesis using the Loris library.
 *
 *	This lorisplay module was originally written by Corbin Champion, 2002.
 *
 * Kelly Fitz, 9 May 2002
 * loris@cerlsoundgroup.org
 *
 * http://www.cerlsoundgroup.org/Loris/
 *
 */

#include "cs.h"
#include "lorisgens.h"
#include "string.h"

#include <Breakpoint.h>
#include <Envelope.h>
#include <Exception.h>
#include <Morpher.h>
#include <Oscillator.h>
#include <Partial.h>
#include <SdifFile.h>

#include <algorithm>
#include <exception>
#include <iostream>
#include <map>
#include <string>
#include <memory>
#include <set>
#include <utility>
#include <vector>

using namespace Loris;
using namespace std;

typedef std::vector< Breakpoint > BREAKPOINTS;
typedef std::vector< Partial > PARTIALS;
typedef std::vector< Oscillator > OSCILS;

#pragma mark -- static helpers --

// ---------------------------------------------------------------------------
//	import_partials
// ---------------------------------------------------------------------------
//
static void import_partials( const std::string & sdiffilname, PARTIALS & part )
{
	try 
	{
		//	clear the dstination:
		part.clear();
		
		//	import:
		std::cerr << "** importing SDIF file " << sdiffilname << std::endl;		  
		SdifFile f( sdiffilname );

		//	copy the Partials into the vector:
		part.reserve( f.partials().size() );
		part.insert( part.begin(), f.partials().begin(), f.partials().end() );
	}
	catch(Exception ex)
	{
		std::cerr << "\nERROR importing SDIF file: " << ex.what() << std::endl;		  
	}
	catch(std::exception ex)
	{
		std::cerr << "\nERROR importing SDIF file: " << ex.what() << std::endl;		  
	}

}

// ---------------------------------------------------------------------------
//	apply_fadetime
// ---------------------------------------------------------------------------
//	Fade Partials in and out, if fadetime > 0.
//
static void apply_fadetime( PARTIALS & part, double fadetime )
{
	//	nothing to do if fadetime is zero:
	if (fadetime <= 0.)
		return;
		
	//	iterator over all Partials, adding Breakpoints at both ends:
	PARTIALS::iterator iter;
	for ( iter = part.begin(); iter != part.end(); ++iter )  
	{
		Partial & partial = *iter;
		
		double btime = partial.startTime();	// get start time of partial
	    double etime = partial.endTime();	// get end time of partial

		//	if a fadetime has been specified, introduce zero-amplitude
		//	Breakpoints to fade in and out over fadetime seconds:
		if( fadetime != 0 )
		{
			if ( partial.amplitudeAt(btime) > 0. )
			{
				//	only fade in if starting amplitude is non-zero:
				if( btime > 0. ) 
				{
					//	if the Partial begins after time 0, insert a Breakpoint
					//	of zero amplitude at a time fadetime before the beginning, 
					//	of the Partial, or at zero, whichever is later:
					double t = std::max(btime - fadetime, 0.);
					partial.insert( t, Breakpoint( partial.frequencyAt(t), partial.amplitudeAt(t), 
												   partial.bandwidthAt(t), partial.phaseAt(t)));
				}
				else 
				{
					//	if the Partial begins at time zero, insert the zero-amplitude
					//	Breakpoint at time zero, and make sure that the next Breakpoint
					//	in the Partial is no more than fadetime away from the beginning
					//	of the Partial:
					
					//	find the first Breakpoint later than time 0:
					Partial::iterator pit = partial.begin();
					while (pit.time() < 0.)
						++pit;
					if ( pit.time() > fadetime )
					{
						//	if first Breakpoint afer 0 is later than fadetime, 
						//	insert a Breakpoint at fadetime:
						double t = fadetime;
						partial.insert( t, Breakpoint( partial.frequencyAt(t), partial.amplitudeAt(t), 
													   partial.bandwidthAt(t), partial.phaseAt(t)));
					}
					
					//	insert the zero-amplitude Breakpoint at 0:
					partial.insert( 0, Breakpoint( partial.frequencyAt(0), 0, 
												   partial.bandwidthAt(0), partial.phaseAt(0)));

				}
			}
			
			//	add fadeout Breakpoint at end:
			double t = etime + fadetime;
			partial.insert( t, Breakpoint( partial.frequencyAt(t), 0, 
										   partial.bandwidthAt(t), partial.phaseAt(t)));
		}
	}
	
}

// ---------------------------------------------------------------------------
//	radianFreq
// ---------------------------------------------------------------------------
//	Compute radian frequency (used by Loris::Oscillator) from frequency in Hz.
//	
static inline double radianFreq( double hz )
{
	return hz * tpidsr;
}

// ---------------------------------------------------------------------------
//	accum_samples
// ---------------------------------------------------------------------------
//	helper
//
static void accum_samples( Oscillator & oscil, Breakpoint & bp, double * bufbegin, int nsamps )
{
	if( bp.amplitude() > 0 || oscil.amplitude() > 0 ) 
	{
		double radfreq = radianFreq( bp.frequency() );
		double amp = bp.amplitude();
		double bw = bp.bandwidth();
		
		//	initialize the oscillator if it is changing from zero
		//	to non-zero amplitude in this  control block:
		if ( oscil.amplitude() == 0. )
		{
			//	don't initialize with bogus values, Oscillator
			//	only guards against out-of-range target values
			//	in generateSamples(), parameter mutators are 
			//	dangerous:
			
			if ( radfreq > PI )	//	don't alias
				amp = 0.;

			if ( bw > 1. )		//	clamp bandwidth
				bw = 1.;
			else if ( bw < 0. )
				bw = 0.;
		
			/*
			std::cerr << "initializing oscillator " << std::endl;
			
			std::cerr << "parameters: " << bp.frequency() << "  ";
			std::cerr << amp << "  " << bw << std::endl;
			*/
						
			//	initialize frequency, amplitude, and bandwidth to 
			//	their target values:
			oscil.setRadianFreq( radfreq );
			oscil.setAmplitude( amp );
			oscil.setBandwidth( bw );
			
			//	roll back the phase:
			oscil.setPhase( bp.phase() - ( radfreq * nsamps ) );
		}	
		
		//	accumulate samples into buffer:
		oscil.generateSamples( bufbegin, bufbegin + nsamps, radfreq, amp, bw );
	}
}

// ---------------------------------------------------------------------------
//	clear_buffer
// ---------------------------------------------------------------------------
//	helper
//
static inline void clear_buffer( double * buf, int nsamps )
{
	std::fill( buf, buf + nsamps, 0. );
}

// ---------------------------------------------------------------------------
//	convert_samples
// ---------------------------------------------------------------------------
//	helper
//
static inline void convert_samples( const double * src, float * tgt, int nn )
{
	do 
	{
		// 	scale Loris sample amplitudes (+/- 1.0) to 
		//	csound sample amplitudes (+/- 32k):
		*tgt++ = (*src++) * 32767.;  
	} while(--nn);
}

#pragma mark -- LorisPartials --

// ---------------------------------------------------------------------------
//	LorisPartials definition
// ---------------------------------------------------------------------------
//	LorisPartials keeps track of a collection of imported Partials and the
//	fadetime, if any, that is applied to them, and the name of the file from
//	which they were imported. LorisPartials instances can be compared using
//	equivalence so that they can be stored in an associative container. 
//	LorisPartials are stored in a std::set accessed by the static member
//	GetPartials(), so that Partials from a particular file and using a 
//	particular fade time can be imported just once and reused.
//
//	The PARTIALS member is mutable, since it is not involve in the equivalence
//	test. Only const access to Partials is provided to clients, but the 
//	GetPartials() member needs to be able to import the Partials into a 
//	LorisPartials that is already part of a std::set (and is thus immutable).
//	(The alternative is to copy, which is wasteful.)
//
class LorisPartials
{
	mutable PARTIALS _partials;
	double _fadetime;
	std::string _fname;
	
public:
	//	construction:
	LorisPartials( void ) {}
	LorisPartials( const string & path, double fadetime ) :
		_fadetime( fadetime ), _fname( path ) {}
	~LorisPartials( void ) {}
	
	//	access:
	const Partial & operator [] ( long idx ) const { return _partials[idx]; }
	
	long size( void ) const { return _partials.size(); }
	
	//	comparison:
	friend bool operator < ( const LorisPartials & lhs, const LorisPartials & rhs )
	{
		return (lhs._fadetime < rhs._fadetime) || (lhs._fname < rhs._fname);
	}

	// 	static member for managing a permanent collection:
	static const LorisPartials & GetPartials( const string & sdiffilname, double fadetime );
};

// ---------------------------------------------------------------------------
//	GetPartials
// ---------------------------------------------------------------------------
//	Return a reference to a collection of Partials from the specified file
//	with the specified fadetime applied. Import if necessary, reuse previously
//	imported Partials if possible. Store imported Partials in a permanent
//	set of imported Partials.
//
const LorisPartials & 
LorisPartials::GetPartials( const string & sdiffilname, double fadetime )
{
	static std::set< LorisPartials > AllPartials;
	
	LorisPartials empty( sdiffilname, fadetime );
	std::set< LorisPartials >::iterator it = AllPartials.find( empty );
		
	if ( it == AllPartials.end() )
	{
		it = AllPartials.insert( empty ).first;
		
		try 
		{
			//	import Partials and apply fadetime:
			import_partials( sdiffilname, it->_partials );
			apply_fadetime( it->_partials, fadetime );
		}
		catch( Exception & ex )
	    {
	        std::string s("Loris exception in LorisPartials::GetPartials(): " );
	        s.append( ex.what() );
	        std::cerr << s << std::endl;
	    }
	    catch( std::exception & ex )
	    {
	        std::string s("std C++ exception in LorisPartials::GetPartials(): " );
	        s.append( ex.what() );
	        std::cerr << s << std::endl;
	    }
	}
	else 
	{
		std::cerr << "** reusing SDIF file " << sdiffilname << std::endl;		  
	}
	
	return *it;
}		


#pragma mark -- LorisReader --

// ---------------------------------------------------------------------------
//	LorisReader definition
// ---------------------------------------------------------------------------
//	LorisReader samples a LorisPartials instance at a given time, updated by
//	calls to updateEnvelopePoints(). 
//
//	A static map of LorisReaders is maintained that allows LorisReaders to be
//	found by index and Csound owner-instrument. A LorisReader can be added to 
//	this map, by is parent Lorisread_priv (below), and subsequently found by
//	other generators having the same owner instrument. This is how lorisplay
//	and lorismorph access the data read by a LorisReader.
//
class LorisReader
{
	const LorisPartials & _partials;
	BREAKPOINTS _breakpoints;
	double _time;
	
public:	
	//	construction:
	LorisReader( const string & fname, double fadetime );
	~LorisReader( void ) {}
	
	//	envelope parameter computation:
	//	(returns number of active Partials)
	long updateEnvelopePoints( double time, double fscale, double ascale, double bwscale );
	
	//	access:
	const LorisPartials & partials( void ) const { return _partials; }
	const BREAKPOINTS & envelopePoints( void ) const { return _breakpoints; }
	double time( void ) const { return _time; }
	
	//	access a particular LorisReader by owner instrument and index:
	static void AssignOwnerAndIndex( INSDS * owner, int index, LorisReader & reader );
	static LorisReader * GetByOwnerAndIndex( INSDS * owner, int index );
	
private:
	typedef std::pair< INSDS *, int > OwnerAndIndex;
	static std::map<OwnerAndIndex, LorisReader *> & OwnerAndIndexMap( void );
}; 

// ---------------------------------------------------------------------------
//	LorisReader construction
// ---------------------------------------------------------------------------
//
LorisReader::LorisReader( const string & fname, double fadetime ) :
	_partials( LorisPartials::GetPartials( fname, fadetime ) ),
	_time( 0. )
{
	_breakpoints.resize( _partials.size() );
}

// ---------------------------------------------------------------------------
//	LorisReader updateEnvelopePoints
// ---------------------------------------------------------------------------
//
long 
LorisReader::updateEnvelopePoints( double time, double fscale, double ascale, double bwscale )
{
	// update time;
	_time = time;
	
	long countActive = 0;
	
	for (long i = 0; i < _partials.size(); ++i )
	{
		const Partial & p = _partials[i];
		Breakpoint & bp = _breakpoints[i];
		
		//	update envelope paramters for this Partial:
	 	bp.setFrequency( fscale * p.frequencyAt( time ) );
		bp.setAmplitude( ascale * p.amplitudeAt( time ) );
		bp.setBandwidth( bwscale * p.bandwidthAt( time ) );
		bp.setPhase( p.phaseAt( time ) );
	
		//	update counter:
		if ( bp.amplitude() > 0 )
			++countActive;
	}
	
	return countActive;
}

// ---------------------------------------------------------------------------
//	LorisReader AssignOwnerAndIndex
// ---------------------------------------------------------------------------
//
void 
LorisReader::AssignOwnerAndIndex( INSDS * owner, int index, LorisReader & reader )
{
	OwnerAndIndexMap()[ std::make_pair( owner, index ) ] = &reader;
}

// ---------------------------------------------------------------------------
//	LorisReader GetByOwnerAndIndex
// ---------------------------------------------------------------------------
//	May return NULL.
//
LorisReader * 
LorisReader::GetByOwnerAndIndex( INSDS * owner, int index )
{
	return OwnerAndIndexMap()[ std::make_pair( owner, index ) ];
}

// ---------------------------------------------------------------------------
//	LorisReader OwnerAndIndexMap
// ---------------------------------------------------------------------------
//	Protect this map inside a function, because Csound has a C main() function,
//	and global C++ objects cannot be guaranteed to be instantiated properly.
//
std::map< LorisReader::OwnerAndIndex, LorisReader * > & 
LorisReader::OwnerAndIndexMap( void )
{
	static std::map< OwnerAndIndex, LorisReader * > readers;
	return readers;
}

#pragma mark -- Lorisread_priv --

// ---------------------------------------------------------------------------
//	Lorisread_priv definition
// ---------------------------------------------------------------------------
// 	Define a structure holding private internal data.
//
struct Lorisread_priv
{
	std::auto_ptr< LorisReader > reader;

	Lorisread_priv( LORISREAD * params );
	~Lorisread_priv( void ) {}
}; 

// ---------------------------------------------------------------------------
//	Lorisread_priv contructor
// ---------------------------------------------------------------------------
//
Lorisread_priv::Lorisread_priv( LORISREAD * params )
{
	std::string sdiffilname;

	//	determine the name of the SDIF file to use:
	//	this code adapted from ugens8.c pvset()
	if ( *params->ifilnam == sstrcod )
	{
		//	use strg name, if given:
		sdiffilname = unquote(params->STRARG);
	}
    /* unclear what this does, not described in pvoc docs
    else if ((long)*p->ifilnam <= strsmax && strsets != NULL && strsets[(long)*p->ifilnam])
      strcpy(sdiffilname, strsets[(long)*p->ifilnam]);
     */
    else 
    {
    	//	else use loris.filnum
    	char tmp[32];
    	sprintf(tmp,"loris.%d", (int)*params->ifilnam);
		sdiffilname = tmp;
	}
	
	//	load the reader:
	reader.reset( new LorisReader( sdiffilname, *(params->fadetime) ) );
	LorisReader::AssignOwnerAndIndex( params->h.insdshead, *(params->readerIdx), *reader );
}

#pragma mark -- lorisread generator functions --

static void lorisread_cleanup(void * p);

// ---------------------------------------------------------------------------
//	lorisread_setup
// ---------------------------------------------------------------------------
//	Runs at initialization time for lorisplay.
//
extern "C"
void lorisread_setup( LORISREAD * p )
{
	p->priv = new Lorisread_priv( p );
	p->h.dopadr = lorisread_cleanup;  // set lorisplay_cleanup as cleanup routine
}

// ---------------------------------------------------------------------------
//	lorisread
// ---------------------------------------------------------------------------
//	Control-rate generator function.
//
extern "C"
void lorisread( LORISREAD * p )
{
	*(p->result) = p->priv->reader->updateEnvelopePoints( *p->time, 
														  *p->freqenv, 
														  *p->ampenv,
														  *p->bwenv );
}

// ---------------------------------------------------------------------------
//	lorisread_cleanup
// ---------------------------------------------------------------------------
//	Cleans up after lorisread.
//
static
void lorisread_cleanup(void * p)
{
	LORISREAD * tp = (LORISREAD *)p;
	delete tp->priv;
}

#pragma mark -- Lorisplay_priv --

// ---------------------------------------------------------------------------
//	Lorisplay_priv definition
// ---------------------------------------------------------------------------
// 	Define a structure holding private internal data.
//
struct Lorisplay_priv
{
	LorisReader * reader;
	OSCILS oscils;
	
	std::vector< double > dblbuffer;
	
	Lorisplay_priv( LORISPLAY * params );
	~Lorisplay_priv( void ) {}
}; 

// ---------------------------------------------------------------------------
//	Lorisplay_priv contructor
// ---------------------------------------------------------------------------
//
Lorisplay_priv::Lorisplay_priv( LORISPLAY * params ) :
	reader( LorisReader::GetByOwnerAndIndex( params->h.insdshead, *(params->readerIdx) ) ),
	dblbuffer( ksmps, 0. )
{
	oscils.resize( reader->envelopePoints().size() );
}

#pragma mark -- lorisplay generator functions --

static void lorisplay_cleanup(void * p);

// ---------------------------------------------------------------------------
//	lorisplay_setup
// ---------------------------------------------------------------------------
//	Runs at initialization time for lorisplay.
//
extern "C"
void lorisplay_setup( LORISPLAY * p )
{
	p->priv = new Lorisplay_priv( p );
	p->h.dopadr = lorisplay_cleanup;  // set lorisplay_cleanup as cleanup routine
}

// ---------------------------------------------------------------------------
//	lorisplay
// ---------------------------------------------------------------------------
//	Audio-rate generator function.
//
extern "C"
void lorisplay( LORISPLAY * p )
{
	Lorisplay_priv & player = *(p->priv);
	const BREAKPOINTS & envPts = player.reader->envelopePoints();
	OSCILS & oscils = player.oscils;
	
	//	clear the buffer first!
	double * bufbegin =  &(player.dblbuffer[0]);
	clear_buffer( bufbegin, ksmps );
	
	//	now accumulate samples into the buffer:
	long numOscils = player.oscils.size();
	for( long i = 0; i < numOscils; ++i )  
	{
		const Breakpoint & bp = envPts[i];
		Breakpoint modifiedBp(  (*p->freqenv) * bp.frequency(),
								(*p->ampenv) * bp.amplitude(),
								(*p->bwenv) * bp.bandwidth(),
								bp.phase() );
		accum_samples( oscils[i], modifiedBp, bufbegin, ksmps );
	} 
	
	//	transfer samples into the result buffer:
	convert_samples( bufbegin, p->result, ksmps );
}

// ---------------------------------------------------------------------------
//	lorisplay_cleanup
// ---------------------------------------------------------------------------
//	Cleans up after lorisplay.
//
static
void lorisplay_cleanup(void * p)
{
	LORISPLAY * tp = (LORISPLAY *)p;
	delete tp->priv;
}

#pragma mark -- Lorismorph_priv --

// ---------------------------------------------------------------------------
//	Lorismorph_priv definition
// ---------------------------------------------------------------------------
// 	Define a structure holding private internal data for lorismorph
//
struct Lorismorph_priv
{
	Morpher morpher;
	LorisReader * src_reader;
	LorisReader * tgt_reader;
	OSCILS oscils;
		
	typedef std::map< long, std::pair< long, long > > LabelMap;
	LabelMap labelMap;
	std::vector< long > src_unlabeled, tgt_unlabeled;
	
	std::vector< double > dblbuffer;
	
	//double time;
	
	Lorismorph_priv( LORISMORPH * params );
	~Lorismorph_priv( void ) {}
	
	//
	//	Define Envelope classes that can access the 
	//	morphing functions defined in the orchestra 
	//	at the control rate:
	struct GetFreqFunc : public Envelope
	{
		LORISMORPH * params;
		GetFreqFunc( LORISMORPH * p ) : params(p) {}
		GetFreqFunc( const GetFreqFunc & other ) : params( other.params ) {}
		~GetFreqFunc( void ) {}
		
		//	Envelope interface:
		GetFreqFunc * clone( void ) const { return new GetFreqFunc( *this ); }
		
		double valueAt( double /* time */ ) const
		{
			//	ignore time, can only access the current value:
			double val = *params->freqenv;
			if ( val > 1 )
				val = 1;
			else if ( val < 0 )
				val = 0; 
			return val;
		}
	};
	
	struct GetAmpFunc : public Envelope
	{
		LORISMORPH * params;
		GetAmpFunc( LORISMORPH * p ) : params(p) {}
		GetAmpFunc( const GetAmpFunc & other ) : params( other.params ) {}
		~GetAmpFunc( void ) {}
		
		//	Envelope interface:
		GetAmpFunc * clone( void ) const { return new GetAmpFunc( *this ); }
		
		double valueAt( double /* time */ ) const
		{
			//	ignore time, can only access the current value:
			double val = *params->ampenv;
			if ( val > 1 )
				val = 1;
			else if ( val < 0 )
				val = 0; 
			return val;
		}
	};

	struct GetBwFunc : public Envelope
	{
		LORISMORPH * params;
		GetBwFunc( LORISMORPH * p ) : params(p) {}
		GetBwFunc( const GetBwFunc & other ) : params( other.params ) {}
		~GetBwFunc( void ) {}
		
		//	Envelope interface:
		GetBwFunc * clone( void ) const { return new GetBwFunc( *this ); }
		
		double valueAt( double /* time */ ) const
		{
			//	ignore time, can only access the current value:
			double val = *params->bwenv;
			if ( val > 1 )
				val = 1;
			else if ( val < 0 )
				val = 0; 
			return val;
		}
	};

}; 

// ---------------------------------------------------------------------------
//	Lorismorph_priv contructor
// ---------------------------------------------------------------------------
//	This is a very ugly piece of code to set up an index map that makes it
//	fast to associate the Breakpoints in the source and target readers with
//	the correct Oscillator. There should be a nicer way to do this, but
//	we cannot count on anything like unique labeling (though the results 
//	will be unpredictable if the labeling is not unique), so it seems
//	like an index map is the most efficient way.
//
Lorismorph_priv::Lorismorph_priv( LORISMORPH * params ) :
	morpher( GetFreqFunc( params ), GetAmpFunc( params ), GetBwFunc( params ) ),
	src_reader( LorisReader::GetByOwnerAndIndex( params->h.insdshead, *(params->srcidx) ) ),
	tgt_reader( LorisReader::GetByOwnerAndIndex( params->h.insdshead, *(params->tgtidx) ) ),
	dblbuffer( ksmps, 0. )
{
	//	build Partial pointer maps:
	const LorisPartials & srcPartials = src_reader->partials();
	const LorisPartials &tgtPartials = tgt_reader->partials();
	
	//	allocate some memory -- this could be more memory-efficient:
	src_unlabeled.reserve( srcPartials.size() );
	tgt_unlabeled.reserve( tgtPartials.size() );
	
	//	map source Partial indices:
	//	(make a note of the largest label we see)
	long maxsrclabel = 0;
	for ( long i = 0; i < srcPartials.size(); ++i )
	{
		const Partial & part = srcPartials[i];
		if ( part.label() != 0 )
			labelMap[ part.label() ] = std::make_pair(i, long(-1));
		else
			src_unlabeled.push_back( i );
			
		if ( part.label() > maxsrclabel )
			maxsrclabel = part.label();
	}
	// std::cerr << "** Largest source label is " << maxsrclabel << std::endl;
	
	//	map target Partial indices:
	//	(make a note of the largest label we see)
	long maxtgtlabel = 0;
	for ( long i = 0; i < tgtPartials.size(); ++i )
	{
		const Partial & part = tgtPartials[i];
		if ( part.label() != 0 )
		{
			if ( labelMap.count( part.label() ) > 0 )
				labelMap[part.label()].second = i;
			else
				labelMap[part.label()] = std::make_pair(long(-1), i);
		}
		else
			tgt_unlabeled.push_back( i );
			
		if ( part.label() > maxtgtlabel )
			maxtgtlabel = part.label();
	}
	// std::cerr << "** Largest target label is " << maxtgtlabel << std::endl;
	
	std::cerr << "** Morph will use " << labelMap.size() << " labeled Partials, ";
	std::cerr << src_unlabeled.size() << " unlabeled source Partials, and ";
	std::cerr << tgt_unlabeled.size() << " unlabeled target Partials." << std::endl;
	
	//	allocate Oscillators:
	oscils.resize( labelMap.size() + src_unlabeled.size() + tgt_unlabeled.size() );
}


#pragma mark -- lorismorph generator functions --

static void lorismorph_cleanup(void * p);

// ---------------------------------------------------------------------------
//	lorismorph_setup
// ---------------------------------------------------------------------------
//	Runs at initialization time for lorismorph.
//
extern "C"
void lorismorph_setup( LORISMORPH * p )
{
	p->priv = new Lorismorph_priv( p );
	p->h.dopadr = lorismorph_cleanup;  // set lorismorph_cleanup as cleanup routine
}

// ---------------------------------------------------------------------------
//	lorismorph
// ---------------------------------------------------------------------------
//	Audio-rate generator function.
//
extern "C"
void lorismorph( LORISMORPH * p )
{
	Lorismorph_priv & imp = *(p->priv);
	const BREAKPOINTS & srcEnvPts = imp.src_reader->envelopePoints();
	const BREAKPOINTS & tgtEnvPts = imp.tgt_reader->envelopePoints();
	OSCILS & oscils = imp.oscils;
	
	//	clear the buffer first!
	double * bufbegin =  &(imp.dblbuffer[0]);
	clear_buffer( bufbegin, ksmps );
	
	//	now accumulate samples into the buffer:
	
	//	first render all the labeled (morphed) Partials:
	// std::cerr << "** Morphing Partials labeled " << imp.labelMap.begin()->first;
	// std::cerr << " to " << (--imp.labelMap.end())->first << std::endl;
	
	Breakpoint bp;
	Partial dummy;
	long oscidx = 0;
	Lorismorph_priv::LabelMap::iterator it;
	for( it = imp.labelMap.begin(); it != imp.labelMap.end(); ++it, ++oscidx )
	{
		long label = it->first;
		std::pair<long, long> & indices = it->second;
		
		long isrc = indices.first;
		long itgt = indices.second;
		
		//	this should not happen:
		if ( itgt < 0 && isrc < 0 )
		{
			std::cerr << "HEY!!!! The labelMap had a pair of bogus indices in it at pos " << oscidx << std::endl;
			continue;
		}
		
		//	note: the time argument for all these morphParameters calls
		//	is irrelevant, since it is only used to index the morphing
		//	functions, which, as defined above, do not use the time
		//	argument, they can only return their current value.
		if ( itgt < 0 )
		{
			//	morph from the source to a dummy:
			// std::cerr << "** Morphing source to dummy " << oscidx << std::endl;
			imp.morpher.morphParameters( srcEnvPts[isrc], dummy, 0, bp );
		}
		else if ( isrc < 0 )
		{
			//	morph from a dummy to the target:
			// std::cerr << "** Morphing dummy to target " << oscidx << std::endl;
			imp.morpher.morphParameters( dummy, tgtEnvPts[itgt], 0, bp );
		}
		else 
		{
			//	morph from the source to the target:
			// std::cerr << "** Morphing source to target " << oscidx << std::endl;
			imp.morpher.morphParameters( srcEnvPts[isrc], tgtEnvPts[itgt], 0, bp );
		}	
		
		accum_samples( oscils[oscidx], bp, bufbegin,ksmps );
	} 
	
	//	render unlabeled source Partials:
	// std::cerr << "** Crossfading " << imp.src_unlabeled.size();
	// std::cerr << " unlabeled source Partials" << std::endl;
	for( long i = 0; i < imp.src_unlabeled.size(); ++i, ++oscidx )  
	{
		//	morph from the source to a dummy:
		imp.morpher.morphParameters( srcEnvPts[ imp.src_unlabeled[i] ], dummy, 0, bp );
		accum_samples( oscils[oscidx], bp, bufbegin, ksmps );
	}
	
	
	//	render unlabeled target Partials:
	// std::cerr << "** Crossfading " << imp.tgt_unlabeled.size();
	// std::cerr << " unlabeled target Partials" << std::endl;
	for( long i = 0; i < imp.tgt_unlabeled.size(); ++i, ++oscidx )  
	{
		//	morph from a dummy to the target:
		imp.morpher.morphParameters( dummy, tgtEnvPts[ imp.tgt_unlabeled[i] ], 0, bp );
		accum_samples( oscils[oscidx], bp, bufbegin, ksmps );
	}	
	
	//	transfer samples into the result buffer:
	convert_samples( bufbegin, p->result, ksmps );

	//	update time:
	//imp.time += ksmps / esr;
	 
}

// ---------------------------------------------------------------------------
//	lorismorph_cleanup
// ---------------------------------------------------------------------------
//	Cleans up after lorismorph.
//
static
void lorismorph_cleanup(void * p)
{
	LORISMORPH * tp = (LORISMORPH *)p;
	delete tp->priv;
}
