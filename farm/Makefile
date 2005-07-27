PYTHON = /sw/bin/python
PYTHONPATH = /usr/local/lib/python2.3/site-packages:..
# PYTHONPATH = /Loris/scripting/.libs:/Loris/scripting
PYTHONENV = env PYTHONPATH=$(PYTHONPATH)

all: alienthreat bell bongo cats cellos choir density derbari elephants \
	 flutes french funny gong moses pianosax sax shaku trombone \
	 morphing

alienthreat: Makefile
	cd $@; $(PYTHONENV) $(PYTHON) alienthreat.py
	
bell: Makefile
	cd $@; $(PYTHONENV) $(PYTHON) nisobell.py
	
bongo: Makefile
	cd $@; $(PYTHONENV) $(PYTHON) bongo.py	
	
cats: Makefile
	cd $@; $(PYTHONENV) $(PYTHON) meows.py	

cellos: Makefile
	cd $@; $(PYTHONENV) $(PYTHON) cellos.py	
	
choir: Makefile
	cd $@; $(PYTHONENV) $(PYTHON) webernchoir.py
	
density: Makefile
	cd $@; $(PYTHONENV) $(PYTHON) density.py

derbari: Makefile
	cd $@; $(PYTHONENV) $(PYTHON) derbari.py	
	
elephants: Makefile
	cd $@; $(PYTHONENV) $(PYTHON) elephants.py
	
flutes: Makefile
	cd $@; $(PYTHONENV) $(PYTHON) flute.py	
	cd $@; $(PYTHONENV) $(PYTHON) flutter.py

french: Makefile
	cd $@; $(PYTHONENV) $(PYTHON) french.py

funny: Makefile
	cd $@; $(PYTHONENV) $(PYTHON) funny.py

gong: Makefile
	cd $@; $(PYTHONENV) $(PYTHON) gong.py

moses: Makefile
	cd $@; $(PYTHONENV) $(PYTHON) moses.py

pianosax: Makefile
	cd $@; $(PYTHONENV) $(PYTHON) pianosax.py

sax: Makefile
	cd $@; $(PYTHONENV) $(PYTHON) sax.py

shaku: Makefile
	cd $@; $(PYTHONENV) $(PYTHON) shaku.py

trombone: Makefile
	cd $@; $(PYTHONENV) $(PYTHON) tbnflutter.py

morphing: elephants
	cd $@; $(PYTHONENV) $(PYTHON) morph_elephants.py
