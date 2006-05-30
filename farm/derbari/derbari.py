"""
derbari.py

Python script for analyzing and reconstructing one of a variety 
of sounds used to test the analysis/modification/synthesis routines 
in Loris.

This script pertains to the raga singing sample that was included in the 
ICMC 2000 bakeoff and called then "shafqat-derbari."

More recently, I have used this sound in my demo at ICMC 2002, and the 
parameters I liked then were 180 Hz resolution and 350 Hz window. I tried
sifting and distilling, and I think sifting (removing zeros) worked better.

notes from trial 1:
	- none of these sounds particularly realistic, same-difference tests fail
	- turning off BW association makes sift and distill sound identical
	- sounds like a lot of low-frequency oomph is lost from the onset
	
notes from trial 2: 
	- got back some of that oomph, sounds much better, but still not like the
	original


There is something going on at high-ish frequencies that might best be captured
as noise, but bandwidth enhancement and distilling doesn't get it. Sounds a
little better if the zero-labeled partials are retained.

Last updated: 30 May 2006 by Kelly Fitz
"""
print __doc__

import loris, time
print "using Loris version", loris.version()

src = 'derbari'
f = loris.AiffFile(src+'.aiff')
samples = f.samples()
rate = f.sampleRate()

print "analyzing", time.ctime(time.time())
a = loris.Analyzer( 140, 280 )
# turn off BW association for now
a.setBwRegionWidth( 0 )
a.setFreqFloor( 100 )
a.setFreqDrift( 20 )

p = a.analyze( samples, rate )
ref = loris.createFreqReference( p, 140, 330 )

# distill
print "distilling", time.ctime(time.time())
pdist = loris.PartialList( p )
loris.channelize( pdist, ref, 1 )
loris.distill( pdist )
# loris.removeLabeled( pdist, 0 )
ofile = src+'.distill'
fout = loris.AiffFile( pdist, rate )
fout.write( ofile + '.aiff' )
loris.exportSdif( ofile + '.sdif', pdist )

# sift
print "sifting", time.ctime(time.time())
psift = loris.PartialList( p )
loris.channelize( psift, ref, 1 )
loris.sift( psift )
# loris.removeLabeled( psift, 0 )
loris.distill( psift )
ofile = src+'.sift'
fout = loris.AiffFile( psift, rate )
fout.write( ofile + '.aiff' )
loris.exportSdif( ofile + '.sdif', psift )

# collate
print "collating", time.ctime(time.time())
praw = p
loris.collate( praw )
ofile = src+'.raw'
fout = loris.AiffFile( praw, rate )
fout.write( ofile + '.aiff' )
loris.exportSdif( ofile + '.sdif', praw )

