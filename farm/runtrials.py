#! python
"""
runtrials.py

Python script to run Loris trials in parallel on one machine
from the farm directory.
"""

print __doc__

import os,time,sys

ttup = time.localtime(time.time())
date = '%i.%i.%i'%(ttup[2], ttup[1],ttup[0])

# farm is a dictionary of directory,scriptnames pairs
farm = { 
	'bell': ('nisobell.py',), 
	'cats': ('angrycat.py', 'meows.py'), 
	'cellos': ('cellos.py',),  
	'choir': ('webernchoir.py',), 
	'elephants': ('elephants.py', ),
	'flutes': ('flute.py', 'flutter.py', ),
	'french': ('french.py',),
	'gong': ('gong.py',),
	'moses': ('moses.py',),
	'sax': ('saxriff.py', )
}
	
if len(sys.argv) > 1:
	dirs = sys.argv[1:]
else:
	dirs = farm.keys()

inbkgrnd = '&'

farmdir = os.getcwd()  #'/net/magoo/users/kfitz/farm'
def changedir(dir):
	return 'cd %s/%s'%(farmdir,dir)
	
if 'PYTHONPATH' in os.environ.keys():
	runpython = 'env PYTHONPATH=%s:%s python'%(farmdir,os.environ['PYTHONPATH'])
else:
	runpython = 'env PYTHONPATH=%s python'%farmdir



for dir in dirs:
	if dir[-1] == '/':
		dir = dir[:-1]
	try:
		scripts = farm[dir]
	except:
		print "no directory", dir
		continue
		
	for script in scripts:
		cmd = '%s; %s %s %s'%(changedir(dir), runpython, script, inbkgrnd)
		print cmd
		os.system(cmd)
