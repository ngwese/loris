#!/usr/bin/python

"""
webernchoir.py

Analyze a short sample from a Webern choir piece. The polyphony 
and close harmony and the low male voices make this one hard, 
but there aren't any transients.

The parameters we settled on were resolution 15 Hz, floor 50 Hz, 
window 100 Hz, region width 5 kHz.

Last updated: 5 Jan 2009 by Kelly Fitz
"""

import loris, time, os

orate = 44100

tag = ''

stuff = {}

# ----------------------------------------------------------------------------


"""
Notes from old experiments with this sample:

notes from trial 1:
	- widest resolution, 50 Hz, is too wide
	- narrow wndows, like 50 Hz, sound funny, showery or hissy, 
		except for 15 Hz resolution, which is more forgiving of
		window width than any larger resolution
	- wide windows, like 150 Hz, don't sound right
	- narrow windows sound more reverberant, but I think its just
		smearing out temporal features like vibrato
	- 40 Hz resolution is probably too wide, but 15, 20, and 30 
		sound very similar
	- 5 kHz regions don't sound different from 2 kHz
		
notes from trial 2:
	- 90 Hz window seems about perfect, 70 and 110 don't work well
	- 20 Hz resolution seems best, 15 Hz is no improvement, 30 Hz
	sounds a little noisy

notes from trial 3 (using 1.0beta8):
	- all of these sound crunchy to me today, but otherwise pretty decent
	
	- today, 27 May 2003, using 1.0.3 (not yet relased) they all sound 
	like crap, try turning off BW assoc.

notes from trial 4:
	- turning off BW association gets rid of most of the crunch, but they still
	don't sound very good. 
	- nothing is really any better than 24.90, all are prety similar
	- maybe try changing the noise floor?
	
notes from trial 5: 
	- raising noise floor is bad, makes then all sound much worse, badly
	lowpass filtered

"""

import loris, time, os

orate = 44100

tag = ''

stuff = {}

# ----------------------------------------------------------------------------

def doChoir( exportDir = '' ):
	name = 'webernchoir'
	f = loris.AiffFile( name + '.aiff' )
	
	print 'analyzing %s (%s)'%(name, time.ctime(time.time()))
	anal = loris.Analyzer( 24, 90 )
	anal.setBwRegionWidth( 0 )
	p = anal.analyze( f.samples(), f.sampleRate() )
	
	# collate, haven't found a way to distill this one
	# print 'collating %s (%s)'%(name, time.ctime(time.time()))
	# loris.collate( p )
	
	
	if exportDir:
		print 'synthesizing %i raw partials (%s)'%(p.size(), time.ctime(time.time()))
		out_sfile = loris.AiffFile( p, orate )
		
		opath = os.path.join( exportDir, name + tag + '.raw.aiff' ) 
		print 'writing %s (%s)'%(opath, time.ctime(time.time()))
		out_sfile.setMidiNoteNumber( 43 )
		out_sfile.setMarkers( f.markers() )
		out_sfile.write( opath )
		
		opath = os.path.join( exportDir, name + tag + '.sdif' )
		print 'writing %s (%s)'%(opath, time.ctime(time.time()))
		out_pfile = loris.SdifFile( p )
		out_pfile.setMarkers( f.markers() )
		out_pfile.write( opath )

	stuff[ name ] = ( p, anal )
	
	print 'Done. (%s)'%(time.ctime(time.time()))

# source = 'webernchoir.aiff'
# file = loris.AiffFile( source )
# samples = file.samples()
# rate = file.sampleRate()



# ----------------------------------------------------------------------------

if __name__ == '__main__':
print __doc__

	print 'Using Loris version %s'%( loris.version() )

	import sys
	odir = os.curdir
	if len( sys.argv ) > 1:
		tag = '.' + sys.argv[1]

	doChoir( odir )