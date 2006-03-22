#ifndef LORIS_INTERFACE_H
#define LORIS_INTERFACE_H

/*
 * This is Fossa, a grapical control application for analysis, synthesis, 
 * and manipulations of digitized sounds using Loris (Fitz and Haken). 
 *
 * Fossa is Copyright (c) 2001 - 2002 by Susanne Lefvert
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
 * lorisInterface.h
 *
 * Definition of the LorisInterface class.
 * 
 *
 * Susanne Lefvert, 1 March 2002
 *
 *
 */

#include <Analyzer.h>
#include <AiffFile.h>
#include <SdifFile.h>
#include <LinearEnvelope.h> 
#include <Channelizer.h>
#include <Distiller.h>
#include <Morpher.h>
#include <Partial.h>
#include <Synthesizer.h>
#include <Notifier.h>
#include <Exception.h>
#include <FrequencyReference.h>

using namespace Loris;
using std::list;

// ---------------------------------------------------------------------------
// class LorisInterface
// The class handles operations requiring communication with the loris c++ 
// library.  <Partial.h> is included in the SoundList and Partial classes, 
// in order to be able to encapsulate Loris::Sound into the datastructure, 
// else LorisInterface is the only class which includes Loris headers.
class LorisInterface{
  public:
    LorisInterface();
    std::list<Partial>* importAiff(
	const char*	path,
	double		resolution,
	double		width
    );

    list<Partial>* importSdif(const char* path);

    void channelize(
	int			refLabel, 
	double			minFreq, 
	double			maxFreq, 
	std::list<Partial>&	sound
    );

    std::list<Partial>* morph(
	LinearEnvelope&		famp,
	LinearEnvelope&		ffreq,
	LinearEnvelope&		fbw,
	std::list<Partial>	Sound1,
	std::list<Partial>	sound2
    );

    void distill(std::list<Partial>& sound); 

    void exportAiff(
	double			sampleRate, 
	int			bitsPerSample, 
	const			char* name, 
	std::list<Partial>	sound, 
	double maxtime
    );

    void exportSdif(
	const char*		name,
	std::list<Partial>	sound
    );
};

#endif // LORIS_INTERFACE_H
