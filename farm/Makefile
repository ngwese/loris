PYTHON = /sw/bin/python
PYTHONPATH = /usr/local/lib/python2.3/site-packages:..
# PYTHONPATH = /Loris/scripting/.libs:/Loris/scripting
PYTHONENV = env PYTHONPATH=$(PYTHONPATH)

DIRS = alienthreat bass bassoon bell bongo cats cellos choir \
	   density derbari elephants flutes french funny gong moses \
	   piano pianosax sax shaku trombone \
	   morphing

all: $(DIRS) 

.PHONY: $(DIRS) 

alienthreat: 
	cd $@; $(PYTHONENV) $(PYTHON) alienthreat.py
	
bass: 
	cd $@; $(PYTHONENV) $(PYTHON) bassD2.py
	
bassoon: 
	cd $@; $(PYTHONENV) $(PYTHON) bassoonDb2.py
	
bell: 
	cd $@; $(PYTHONENV) $(PYTHON) nisobell.py
	
bongo: 
	cd $@; $(PYTHONENV) $(PYTHON) bongo.py	
	
cats: 
	cd $@; $(PYTHONENV) $(PYTHON) meows.py	

cellos: 
	cd $@; $(PYTHONENV) $(PYTHON) cellos.py	
	
choir: 
	cd $@; $(PYTHONENV) $(PYTHON) webernchoir.py
	
density: 
	cd $@; $(PYTHONENV) $(PYTHON) density.py

derbari: 
	cd $@; $(PYTHONENV) $(PYTHON) derbari.py	
	
elephants: 
	cd $@; $(PYTHONENV) $(PYTHON) elephants.py
	
flutes: 
	cd $@; $(PYTHONENV) $(PYTHON) flute.py	
	cd $@; $(PYTHONENV) $(PYTHON) flutter.py

french: 
	cd $@; $(PYTHONENV) $(PYTHON) french.py

funny:
	cd $@; $(PYTHONENV) $(PYTHON) funny.py

gong: 
	cd $@; $(PYTHONENV) $(PYTHON) gong.py

moses: 
	cd $@; $(PYTHONENV) $(PYTHON) moses.py

pianosax: 
	cd $@; $(PYTHONENV) $(PYTHON) pianosax.py

piano: 
	cd $@; $(PYTHONENV) $(PYTHON) pianoEb3.py
	cd $@; $(PYTHONENV) $(PYTHON) pianoBb3.py
	cd $@; $(PYTHONENV) $(PYTHON) pianoC4.py

sax: 
	cd $@; $(PYTHONENV) $(PYTHON) saxriff.py

shaku: 
	cd $@; $(PYTHONENV) $(PYTHON) shaku.py

trombone: 
	cd $@; $(PYTHONENV) $(PYTHON) tbnflutter.py

morphing: elephants
	cd $@; $(PYTHONENV) $(PYTHON) morph_elephants.py
