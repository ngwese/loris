"""
shaku.py

Python script for analyzing and reconstructing one of a variety 
of sounds used to test the analysis/modification/synthesis routines 
in Loris.

This script pertains to the shakuhachi sample that was included in the 
ICMC 2000 bakeoff. It is a 13 second phrase that sounds like it was
recorded in a steam tunnel.

The parameters we liked once were 80 Hz resolution and 300 Hz
analysis window.

trial1:
	- sounds okay, as before, the articulation is exaggerated for some reason
	- why should this need 80 Hz resolution? The fundamental is over 400 Hz!
trial 2:


Last updated: 28 May 2003 by Kelly Fitz
"""
print __doc__

import loris, time
print "using Loris version", loris.version()


# use this trial counter to skip over
# eariler trials
trial = 2
print "running trial number", trial, time.ctime(time.time())


src = 'shaku'
f = loris.AiffFile(src+'.aiff')
samples = f.samples()
rate = f.sampleRate()

if trial == 1:
	res = 80
	mlw = 300
	a = loris.Analyzer( res, mlw )
	p = a.analyze( samples, rate )
	ofile = 'shaku.%i.%i.raw'%(res, mlw)
	# collate
	loris.distill( p )
	# export
	loris.exportAiff( ofile + '.aiff', loris.synthesize( p, rate ), rate, 1, 16 )
	loris.exportSpc( ofile + '.s.spc', p, 60, 0 ) 
	loris.exportSpc( ofile + '.e.spc', p, 60, 1 ) 

if trial == 2:
	resolutions = ( 80, 160, 240, 320 )
	pref = loris.importSpc('shaku.fund.qharm')
	ref = loris.createFreqReference( pref, 50, 1000 )
	for res in resolutions:
		for mlw in ( 300, 450, 600 ):
			a = loris.Analyzer( res, mlw )
			p = a.analyze( samples, rate )
			d1 = p.copy()
			d2 = p.copy()
			
			# export raw
			loris.distill( p )
			ofile = 'shaku.%i.%i.raw'%(res, mlw)
			loris.exportAiff( ofile + '.aiff', loris.synthesize( p, rate ), rate, 1, 16 )
			# prune before Spc export
			iter = p.begin()
			while not iter.equals( p.end() ):
				if iter.partial().label() > 511:
					next = iter.next()
					p.erase( iter )
					iter = next
				else:
					iter = iter.next()
			loris.exportSpc( ofile + '.s.spc', p, 68, 0 ) 
			loris.exportSpc( ofile + '.e.spc', p, 68, 1 ) 
		
			# distill one partial per harmonic
			loris.channelize( d1, ref, 1 )
			loris.distill( d1 )
			# export
			ofile = 'shaku.%i.%i.d1'%(res, mlw)
			loris.exportAiff( ofile + '.aiff', loris.synthesize( d1, rate ), rate, 1, 16 )
			# prune before Spc export
			iter = d1.begin()
			while not iter.equals( d1.end() ):
				if iter.partial().label() > 511:
					next = iter.next()
					d1.erase( iter )
					iter = next
				else:
					iter = iter.next()
			loris.exportSpc( ofile + '.s.spc', d1, 68, 0 ) 
			loris.exportSpc( ofile + '.e.spc', d1, 68, 1 ) 
		
			# distill two partials per harmonic
			loris.channelize( d2, ref, 2 )
			loris.distill( d2 )
			# export
			ofile = 'shaku.%i.%i.d2'%(res, mlw)
			loris.exportAiff( ofile + '.aiff', loris.synthesize( d2, rate ), rate, 1, 16 )
			# prune before Spc export
			iter = d2.begin()
			while not iter.equals( d2.end() ):
				if iter.partial().label() > 511:
					next = iter.next()
					d2.erase( iter )
					iter = next
				else:
					iter = iter.next()
			loris.exportSpc( ofile + '.s.spc', d2, 68, 0 ) 
			loris.exportSpc( ofile + '.e.spc', d2, 68, 1 ) 
		
