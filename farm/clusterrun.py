#! python
"""
clusterrun.py

Python script to run Loris trials on the Beowulf cluster.
"""

print __doc__

import os,time

ttup = time.localtime(time.time())
date = '%i.%i.%i'%(ttup[2], ttup[1],ttup[0])

# farm is a tuple of directory,scriptname pairs
farm = ( 
	( 'angyrcat', 'angrycat.py'), 
	( 'bell', 'nisobell.py'), 
	( 'cellos', 'cellos.py'), 
	( 'choir', 'webernchoir.py'), 
	( 'elephants', 'elephants.py'), 
	( 'flute', 'flute.py'), 
	( 'sax', 'saxriff.py') 
)
	
def nodename(nodenum):
	if nodenum > 9:
		s = str(nodenum)
	else:
		s = '0'+str(nodenum)
	return 'node'+s
	
shellcmd = 'rsh -n'
inbkgrnd = '&'

farmdir = '/cluster/kfitz/farm'
def changedir(dir):
	return 'cd %s/%s'%(farmdir,dir)
	
runpython = 'env PYTHONPATH=%s python'%farmdir

def dump(script):
	 return '> %s.%s.out'%(script[:-3], date)

node = 1
for (dir,script) in farm:
	cmd = '%s %s "%s; %s %s %s" %s'%(shellcmd, nodename(node), changedir(dir), runpython, script, dump(script), inbkgrnd)
	print cmd
	# os.system(cmd)
	node = node+1
	if node>16:
		node=1
