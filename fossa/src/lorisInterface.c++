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
 * partialsList.c++
 *
 * The PartialsList class is the main model of the application, ie. changes in 
 * PartialsList will trigger updates in view classes. This class has a list
 * containing imported partials and partials produced from different 
 * manipulations. One partials in the list is always set to current partials, 
 * and modifications can be made only to current partials. 
 * PartialsList also keeps track of two partials which are selected when 
 * the user decides to make a morph between two sounds. The class 
 * communicates with LorisInterface for necessary operations on partials.
 * 
 *
 * Susanne Lefvert, 1 March 2002
 *
 *
 */

#if HAVE_CONFIG_H
#include <config.h>  // #define directives are placed in config.h by autoconf
#endif

#include "lorisInterface.h"

using namespace Loris;
using std::cout;
using std::list;
using std::vector;
using std::string;

// ---------------------------------------------------------------------------
//      LorisInterface constructor
// ---------------------------------------------------------------------------

LorisInterface::LorisInterface(){}

// ---------------------------------------------------------------------------
//      importAiff
// ---------------------------------------------------------------------------
// Import and analyze aiff file with given frequency resolution and window
// width.
// Consider combining importSdif and importAiff providing resolution and width 
// with default parameters.  Check parameters to decide which format to 
// import. 

list<Partial>* LorisInterface::importAiff(const char* path, double resolution, double width){ 
  try{
    list<Partial>* partials = new list<Partial>;  // deleted in partialslist
    AiffFile file(path);
    vector<double> sampleVector(file.numFrames());
    sampleVector = file.samples();

    Analyzer analyzer(width);               // according to Kelly Fitz
    analyzer.setFreqResolution(resolution); // loris version 8 has 
    analyzer.setFreqFloor(resolution);      // constructor for this.
    analyzer.setFreqDrift(resolution);

    analyzer.analyze(sampleVector, file.sampleRate());
    partials->splice(partials->end(), analyzer.partials());
    return partials;
  }

  catch(...){
    throw;
  }
}

// ---------------------------------------------------------------------------
//      importSdif
// ---------------------------------------------------------------------------
// Import sdif file 
// Concider combining importSdif and importAiff providing resolution and width 
// with default parameters.  Check parameters to decide which format to 
// import. 

list<Partial>* LorisInterface::importSdif(const char* path){ 
  
  try{
    SdifFile* file = new SdifFile(string(path));
    list<Partial>* partials = new list<Partial>;
    partials->splice(partials->end(), file->partials());
    return partials;
  }

  catch(...){
    throw;
  }
}

// ---------------------------------------------------------------------------
//     channelize
// ---------------------------------------------------------------------------
// Channelize list of Partial with given frequency label, 
// minimum frequency, and mximum frequency.

void LorisInterface::channelize(int refLabel, double minFreq, double maxFreq, list<Partial>& partials){ 
  try{
    list<Partial>::const_iterator begin = partials.begin();
    list<Partial>::const_iterator end   = partials.end();
    FrequencyReference freRef(begin, end, minFreq, maxFreq);
    BreakpointEnvelope referenceEnvelope = freRef.envelope();
    Channelizer channelizer(referenceEnvelope, refLabel);
    channelizer.channelize(partials.begin(), partials.end());
  }
  catch(...){
    throw;
  }
}

// ---------------------------------------------------------------------------
//     distill
// ---------------------------------------------------------------------------
// Distill list of Partial

void LorisInterface::distill(list<Partial>& partials){
  
  try{
  Distiller distiller;
  distiller.distill(partials);
  }

  catch(...){
    throw;
  }
}

// ---------------------------------------------------------------------------
//     morph
// ---------------------------------------------------------------------------
// Morph 2 lists of Partial with given frequency, amplitude, and noise
// envelopes.

list<Partial>* LorisInterface::morph(BreakpointEnvelope& famp, BreakpointEnvelope& ffreq, BreakpointEnvelope& fbw,  list<Partial> partials1, list<Partial> partials2){
 
  try{
    Morpher morpher(ffreq, famp, fbw);
    morpher.morph(partials1.begin(), partials1.end(), partials2.begin(), partials2.end());
    
    // is deleted in partialsList;
    list<Partial>* morphResult = new list<Partial>;
    morphResult->splice(morphResult->end(), morpher.partials());
    return morphResult;
  }
  
  catch(...){
    throw;
  }
}

// ---------------------------------------------------------------------------
//     exportAiff
// ---------------------------------------------------------------------------
// Export list of Partial to aiff file format

void LorisInterface::exportAiff(double sampleRate, int bitsPerSample, const char* name, list<Partial> partials, double maxtime){
  
  try{
    // insuring a long enough buffer for synthesized partials
	const double Padding = .01;	//	10ms is more than enough
    double time = ((maxtime + Padding) * sampleRate);
    vector<double> sampleVector = vector<double>(time);
    Synthesizer synthesizer(sampleRate, sampleVector);
    AiffFile file(partials.begin(), partials.end(), time);

   
    // Loris can only synthesise mono files (channels = 1)
    //AiffFile::Export(name, sampleRate, 1, bitsPerSample, sampleVector.begin(), sampleVector.end());
    file.write(name, bitsPerSample);
  }
  
  catch(...){
    throw;
  }
}

// ---------------------------------------------------------------------------
//     exportSdif
// ---------------------------------------------------------------------------
// Export list of Partial to sdif file format

void LorisInterface::exportSdif(const char* name, list<Partial> partials){
  try{
    SdifFile::Export(name, partials);
  }
  
  catch(...){
    throw;
  }
}
