#ifndef PARTIALS_H
#define PARTIALS_H

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
 * partials.h
 *
 * Definition of the Partials class. 
 * 
 *
 * Susanne Lefvert, 1 March 2002
 *
 *
 */

#include "lorisInterface.h"
#include "partialsPixmap.h"

#include <qlist.h>

#include <stdio.h>
#include <math.h>

#include <Partial.h>

class PartialsPixmap;
class AmplitudePixmap;
class FrequencyPixmap;
class NoisePixmap;
class EmptyPixmap;

using namespace Loris;

// ---------------------------------------------------------------------------
// class Partials
//
// A Partials has a name, and a list of actual Loris partials representing sound.
// It contains state information and has plots over its frequency, amplitude, and 
// noise envelopes over time. Partials communicates with lorisInterface in order
// to preform modifications of the list of Loris::Partial.

class Partials{
 
 public:
  enum State{amplitude, frequency, noise};
  Partials(list<Partial> p, QString  n, LorisInterface* interface);
  ~Partials();
  QString getName() const;
  list<Partial>* getPartials() const;
  double getMaxAmplitude() const;
  double getMaxFrequency() const;
  double getMaxNoise() const;
  double getDuration() const;
  State getState() const;
  int getNumberOfPartials() const;
  QPixmap getAmplitudePixmap() const;
  QPixmap getFrequencyPixmap() const;
  QPixmap getNoisePixmap() const;
  bool isDistilled() const;
  bool isChannelized() const;
  
  void rename(QString newName);
  void play();
  void setState(State newState);
  void shiftFrequency(double val);
  void shiftNoise(double val);
  void shiftAmplitude(double val);
  void channelize(int refLabel, double minFreq, double maxFreq);
  void distill();
  void exportToAiff(double sampleRate, int sampleBits, const char* name);
  void exportToSdif(const char* name);
 
 private:
 
  int nrOfPartials;
  QString name;
  list<Partial>* partialList;
  double maxAmplitude;
  double maxFrequency;
  double maxNoise;
  double maxTime;
  bool channelized;
  bool distilled;
  LorisInterface* interface;
  QPixmap* amplitudePixmap;
  QPixmap* frequencyPixmap;
  QPixmap* noisePixmap;
  State state;
  
  void setValues();
  void updatePixmap();
};

#endif // PARTIALS_H
































