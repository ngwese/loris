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
#include <Exception.h>
#include <Oscillator.h>
#include <Partial.h>
#include <SdifFile.h>

#include <algorithm>
#include <exception>
#include <iostream>
#include <map>
#include <string>
#include <vector>

using namespace Loris;
using namespace std;

typedef std::vector< Breakpoint > BREAKPOINTS;
typedef std::vector< Partial > PARTIALS;
typedef std::vector< Oscillator > OSCILS;

#pragma mark -- static helpers --

// ---------------------------------------------------------------------------
//	sdif_openfile
// ---------------------------------------------------------------------------
//	Import Partials from the named SDIF file if they have not already 
//	been imported. Return a pointer to the SdifFile object containing the
//	Partials. 
//
//	This function could be used by several generators, so that they could
//	all share the pool of reassigned bandwidth-enhanced partials imported
//	from SDIF files.
//
static SdifFile * sdif_openfile( const std::string & filename )
{
	static std::map< std::string, SdifFile * > filenamemap;
	
	std::map< std::string, SdifFile * >::iterator it = filenamemap.find( filename );
		
	if ( it != filenamemap.end() )
	{
		std::cerr << "** re-using SDIF file " << filename << std::endl;		  
		return it->second;
	}
	else
	{	
		try 
		{
			std::cerr << "** importing SDIF file " << filename << std::endl;		  
			SdifFile * f = new SdifFile(filename);
			filenamemap[filename] = f;
			return f;
		}
		catch(Exception ex)
		{
			std::cerr << "\nERROR importing SDIF file: " << ex.what() << std::endl;		  
			return NULL;
		}
		catch(std::exception ex)
		{
			std::cerr << "\nERROR importing SDIF file: " << ex.what() << std::endl;		  
			return NULL;
		}
	}
}

// ---------------------------------------------------------------------------
//	import_partials
// ---------------------------------------------------------------------------
//	Import the Partials, if necessary, and make a private copy of them,
//	and allocate Oscillators for each Partial.
//
static void import_partials( const std::string & sdiffilname, PARTIALS & part )
{
	SdifFile * f = sdif_openfile( sdiffilname );
	if ( f == NULL )
		return;
		
	//	copy the Partials into the vector in bwestore:
	//	NOTE - cannot (trivially) share Partials between generators, because
	//	fadetimes might be different!
	part.reserve( f->partials().size() );
	part.insert( part.begin(), f->partials().begin(), f->partials().end() );
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

#pragma mark -- LorisReader --

// ---------------------------------------------------------------------------
//	LorisReader definition
// ---------------------------------------------------------------------------
//
class LorisReader
{
	PARTIALS _partials;
	BREAKPOINTS _breakpoints;
	double _time;
	
public:	
	//	construction:
	LorisReader( void ) : _time(0) {}	// initializes an empty reader
	~LorisReader( void ) {}
	
	//	Partial loading:
	//	(returns the number of Partials loaded);
	long loadPartials( const std::string & sdiffilname, double fadetime );
	
	//	envelope parameter computation:
	//	(returns number of active Partials)
	long updateEnvelopePoints( double time );
	
	//	access:
	const PARTIALS & partials( void ) const { return _partials; }
	const BREAKPOINTS & envelopePoints( void ) const { return _breakpoints; }
	double time( void ) const { return _time; }
	
	//	access a particular LorisReader by index:
	static LorisReader & GetByIndex( int index );
}; 

// ---------------------------------------------------------------------------
//	LorisReader loadPartials
// ---------------------------------------------------------------------------
//	Return the number of Partials loaded.
//
long
LorisReader::loadPartials( const std::string & sdiffilname, double fadetime )
{
	try 
	{
		//	initialize:
		_partials.clear();
		std::fill( _breakpoints.begin(), _breakpoints.end(), Breakpoint() );
		
		//	import Partials and apply fadetime:
		import_partials( sdiffilname, _partials );
		apply_fadetime( _partials, fadetime );
		
		//	allocate (initialize) Breakpoints:
		_breakpoints.resize( _partials.size() );
	}
	catch( Exception & ex )
    {
        std::string s("Loris exception in LorisReader::loadPartials(): " );
        s.append( ex.what() );
        std::cerr << s << std::endl;
    }
    catch( std::exception & ex )
    {
        std::string s("std C++ exception in LorisReader::loadPartials(): " );
        s.append( ex.what() );
        std::cerr << s << std::endl;
    }
	
	return _partials.size();
}

// ---------------------------------------------------------------------------
//	LorisReader updateEnvelopePoints
// ---------------------------------------------------------------------------
//
long 
LorisReader::updateEnvelopePoints( double time )
{
	// update time;
	_time = time;
	
	long countActive = 0;
	
	for (long i = 0; i < _partials.size(); ++i )
	{
		Partial & p = _partials[i];
		Breakpoint & bp = _breakpoints[i];
		
		//	update envelope paramters for this Partial:
	 	bp.setFrequency( p.frequencyAt( time ) );
		bp.setAmplitude( p.amplitudeAt( time ) );
		bp.setBandwidth( p.bandwidthAt( time ) );
		bp.setPhase( p.phaseAt( time ) );
	
		//	update counter:
		if ( bp.amplitude() > 0 )
			++countActive;
	}
	
	return countActive;
}

// ---------------------------------------------------------------------------
//	LorisReader GetByIndex
// ---------------------------------------------------------------------------
//	May return an empty reader.
//
LorisReader & 
LorisReader::GetByIndex( int index )
{
	static std::map<int, LorisReader> readers;
	return readers[index];
}

#pragma mark -- Lorisread_priv --

// ---------------------------------------------------------------------------
//	Lorisread_priv definition
// ---------------------------------------------------------------------------
// 	Define a structure holding private internal data.
//
struct Lorisread_priv
{
	LorisReader & reader;

	Lorisread_priv( LORISREAD * params );
	~Lorisread_priv( void ) {}
}; 

// ---------------------------------------------------------------------------
//	Lorisread_priv contructor
// ---------------------------------------------------------------------------
//
Lorisread_priv::Lorisread_priv( LORISREAD * params ) :
	reader( LorisReader::GetByIndex( (int)*(params->readerIdx) ) )
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
	reader.loadPartials( sdiffilname, *(params->fadetime) );
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
	*(p->result) = 
		(MYFLT) p->priv->reader.updateEnvelopePoints( *(p->time) );
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


// ---------------------------------------------------------------------------
//	Lorisplay_priv definition
// ---------------------------------------------------------------------------
// 	Define a structure holding private internal data.
//
struct Lorisplay_priv
{
	LorisReader & reader;
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
	reader( LorisReader::GetByIndex( *(params->readerIdx) ) ),
	dblbuffer( ksmps, 0. )
{
	oscils.resize( reader.envelopePoints().size() );
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
	const BREAKPOINTS & envPts = player.reader.envelopePoints();
	OSCILS & oscils = player.oscils;
	
	//	clear the buffer first!
	std::fill( player.dblbuffer.begin(), player.dblbuffer.end(), 0. );
	
	//	now accumulate samples into the buffer:
	double * bufbegin =  &(player.dblbuffer[0]);
	long numOscils = player.oscils.size();
	for( long i = 0; i < numOscils; i++)  
	{
		const Breakpoint & bp = envPts[i];

		//	do nothing if the amplitude is zero over the
		//	duration of the control block:
		if( bp.amplitude() > 0 || oscils[i].amplitude() > 0 ) 
		{
			double radfreq = radianFreq( (*p->freqenv) * bp.frequency() );
			double amp = (*p->ampenv) * bp.amplitude();
			double bw = (*p->bwenv) * bp.bandwidth();
			
			//	initialize the oscillator if it is changing from zero
			//	to non-zero amplitude in this  control block:
			if ( oscils[i].amplitude() == 0. )
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
				std::cerr << "initializing oscillator " << i << " at time ";
				std::cerr << player.reader.time() << " for Partial beginning at time ";
				std::cerr << (player.reader.partials())[i].startTime() << std::endl;
				
				std::cerr << "parameters: " << (*p->freqenv) * bp.frequency() << "  ";
				std::cerr << amp << "  " << bw << std::endl;
				*/
							
				//	initialize frequency, amplitude, and bandwidth to 
				//	their target values:
				oscils[i].setRadianFreq( radfreq );
				oscils[i].setAmplitude( amp );
				oscils[i].setBandwidth( bw );
				
				//	roll back the phase:
				oscils[i].setPhase( bp.phase() - ( radfreq * ksmps ) );
			}	
			
			//	accumulate samples into buffer:
			oscils[i].generateSamples( bufbegin, bufbegin + ksmps, radfreq, amp, bw );
		}
	} 
	
	//	transfer samples into the result buffer:
	float *answer = p->result;
	double * buf = &(player.dblbuffer[0]);
    int nn = ksmps;
	do 
	{
		// 	scale Loris sample amplitudes (+/- 1.0) to 
		//	csound sample amplitudes (+/- 32k):
		*answer++ = (*buf++) * 32767.;  
	} while(--nn);
	 
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

// ---------------------------------------------------------------------------
//	Lorismorph_priv definition
// ---------------------------------------------------------------------------
// 	Define a structure holding private internal data for lorismorph
//
struct Lorismorph_priv
{
	OSCILS oscp;
	
	PARTIALS src_part_labeled, tgt_part_labeled;
	PARTIALS src_part_unlabeled, tgt_part_unlabeled;
	
	std::vector< double > dblbuffer;
	
	Lorismorph_priv( LORISMORPH * params );
	~Lorismorph_priv( void );
}; 

// ---------------------------------------------------------------------------
//	Lorismorph_priv contructor
// ---------------------------------------------------------------------------
//
Lorismorph_priv::Lorismorph_priv( LORISMORPH * params ) :
	dblbuffer( ksmps, 0. )
{
}

/*
	Yuck. It appears that there's NO WAY to specify two string arguments at all.
	
	I think that the way to do this is going to be: implement a reader in the
	fashion of lpread, and give it a way to specify an index number, something 
	like lpslot, that can be used to refer to that data later. Then, use this
	preloaded data in lorismorph, and maybe optionally lorisplay too.
	
	The nice thing about a reader is that it could allow is to do temporal
	feature alignment in Csound too, as in the lp case, where the reader
	has a time index.
*/