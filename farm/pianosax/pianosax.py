"""
pianosax.py

Python script for analyzing and reconstructing one of a variety 
of sounds used to test the analysis/modification/synthesis routines 
in Loris.

This script pertains to the piano/soprano sax duet that I used in 
my dissertation work. I also used this sound to test the frequency 
tracking analysis, developed after release 1.0.2, but didn't work long 
enough at it to make it sound good. We once liked to configure the
analyzer with resolution and window width at 300Hz, but then needed
to lower the frequency floor to about 90 Hz. For testing the frequency
tracking analysis, I used resolution 150 Hz (or maybe 90?), and 
window width 300 Hz.

A fundamental frequency envelope traced in Kyma is in 
pianosax.fund.qharm.

trial 1:
	- must have been using 90 Hz resolution (more importantly, 90 Hz
	frequency floor) to get the thuds in the piano notes.
	
trial 2:
	- setting frequency floor to 90 Hz is adequate, don't need resolution
	that low
	- tracking analyses are crunchy, try turning off BW association

trial 3:
	- sifting but leaving the zeros in still sounds crunchy, removing
	the zeros sounds anemic and bubbly
	- two partials per harmonic restores some of the sound, but also leaves
	in some crunch or whoosh, can't figure out where it comes from.
	- the crunch isn't actually noise, apparently, because it is still there
	even if the noise energy in scaled to zero!

Last updated: 4 June 2003 by Kelly Fitz
"""

print __doc__

import loris, time
print "using Loris version", loris.version()

# use this trial counter to skip over
# eariler trials
trial = 4

print "running trial number", trial, time.ctime(time.time())

source = 'pianosax.aiff'
file = loris.AiffFile( source )
samples = file.samples()
rate = file.sampleRate()
pref = loris.importSpc('pianosax.fund.qharm')
ref = loris.createFreqReference( pref, 50, 1000 )

if trial == 1:
	resolutions = ( 90, 150 )
	widths = ( 300, )
	for r in resolutions:
		for w in widths:
			a = loris.Analyzer( r, w )
			# turn off BW association for now
			# a.setBwRegionWidth( 0 )
			p = a.analyze( samples, rate )
			# export raw
			ofile = 'pianosax.%i.%i.raw'%(r, w)
			loris.distill( p )
			# export
			loris.exportAiff( ofile + '.aiff', loris.synthesize( p, rate ), rate, 1, 24 )
			# prune before Spc export
			iter = p.begin()
			while not iter.equals( p.end() ):
				next = iter.next()
				if iter.partial().label() > 511:					
					p.erase( iter )
				iter = next
			loris.exportSpc( ofile + '.s.spc', p, 60, 0 ) 
			loris.exportSpc( ofile + '.e.spc', p, 60, 1 ) 

if trial == 2:
	resolutions = ( 90, 150 )
	width = 300
	for res in resolutions:
		# do normal analysis
		a = loris.Analyzer( res, width )
		a.setFreqFloor( 90 )
		p = a.analyze( samples, rate )
		# export
		ofile = 'pianosax.%i.%i.raw'%(res, width)
		loris.distill( p )
		# export
		loris.exportAiff( ofile + '.aiff', loris.synthesize( p, rate ), rate, 1, 24 )
		# prune before Spc export
		iter = p.begin()
		while not iter.equals( p.end() ):
			next = iter.next()
			if iter.partial().label() > 511:					
				p.erase( iter )
			iter = next
		loris.exportSpc( ofile + '.s.spc', p, 60, 0 ) 
		loris.exportSpc( ofile + '.e.spc', p, 60, 1 ) 
		
		# do tracking analysis
		a = loris.Analyzer( res, width )
		a.setFreqFloor( 90 )
		p = a.analyze( samples, rate, ref )
		# export
		ofile = 'pianosax.%i.%i.trk'%(res, width)
		loris.channelize(p, ref, 1)
		loris.sift( p )
		loris.distill( p )
		# export
		loris.exportAiff( ofile + '.aiff', loris.synthesize( p, rate ), rate, 1, 24 )
		# prune before Spc export
		iter = p.begin()
		while not iter.equals( p.end() ):
			next = iter.next()
			if iter.partial().label() > 511:					
				p.erase( iter )
			iter = next
		loris.exportSpc( ofile + '.s.spc', p, 60, 0 ) 
		loris.exportSpc( ofile + '.e.spc', p, 60, 1 ) 
		
if trial == 3:
	res = 150
	width = 300
	# do tracking analysis
	a = loris.Analyzer( res, width )
	a.setFreqFloor( 90 )
	# turn off BW association for now
	a.setBwRegionWidth( 0 )
	p = a.analyze( samples, rate, ref )
	# export
	ofile = 'pianosax.%i.%i.trk'%(res, width)
	loris.channelize(p, ref, 2)
	loris.sift( p )
	zeros = loris.extractLabeled( p, 0 )
	loris.distill( p )
	# scale noise to zero?
	loris.scaleNoiseRatio( p, loris.BreakpointEnvelope(0) )
	# export
	loris.exportAiff( ofile + '.aiff', loris.synthesize( p, rate ), rate, 1, 24 )
	# prune before Spc export
	iter = p.begin()
	while not iter.equals( p.end() ):
		next = iter.next()
		if iter.partial().label() > 511:					
			p.erase( iter )
		iter = next
	loris.exportSpc( ofile + '.s.spc', p, 60, 0 ) 
	loris.exportSpc( ofile + '.e.spc', p, 60, 1 ) 
	
if trial == 4:
	res = 150
	width = 300
	# do tracking analysis with high amp floor
	a = loris.Analyzer( res, width )
	a.setFreqFloor( 90 )
	a.setAmpFloor( -40 )
	# turn off BW association for now
	a.setBwRegionWidth( 0 )
	p = a.analyze( samples, rate, ref )
	
	# use this analysis to create another reference env
	loris.channelize(p, ref, 1)
	loris.distill( p )
	newref = loris.createFreqReference( p, 100, 500 )
	
	# let's look at these partials
	loris.exportSpc( 'whoppee.s.spc', p, 60, 0 ) 
	
	# re-analyze with the new reference and
	# a normal amp floor:
	a.setAmpFloor( -90 )
	p = a.analyze( samples, rate, newref )
	
	# export
	ofile = 'pianosax.%i.%i.trk'%(res, width)
	loris.channelize(p, newref, 2)
	loris.sift( p )
	# zeros = loris.extractLabeled( p, 0 )
	loris.distill( p )
	# scale noise to zero?
	loris.scaleNoiseRatio( p, loris.BreakpointEnvelope(0) )
	# export
	loris.exportAiff( ofile + '.aiff', loris.synthesize( p, rate ), rate, 1, 24 )
	# prune before Spc export
	iter = p.begin()
	while not iter.equals( p.end() ):
		next = iter.next()
		if iter.partial().label() > 511:					
			p.erase( iter )
		iter = next
	loris.exportSpc( ofile + '.s.spc', p, 60, 0 ) 
	loris.exportSpc( ofile + '.e.spc', p, 60, 1 ) 
	

