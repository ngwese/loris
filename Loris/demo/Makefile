# Copyright (C) 1999-2002, Kelly Fitz and Lippold Haken <loris@cerlsoundgroup.org>
#  
# This file is free software; as a special exception the author gives
# unlimited permission to copy and/or distribute it, with or without 
# modifications, as long as this notice is preserved.
# 
# This program is distributed in the hope that it will be useful, but
# WITHOUT ANY WARRANTY, to the extent permitted by law; without even the
# implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

demo_sources = source/flute.source.aiff source/clarinet.source.aiff source/cello.source.aiff
demo_source_tarball = morphsources.tgz
demo_refs_tarball = morphreference.tgz
demoscript = morphdemo.py

demo_output = cello.dilated.aiff \
	cello.dilated.sdif \
	cello.premorph.spc \
	cellute.aiff \
	cellute2.aiff \
	cellute2.spc \
	clar.dilated.aiff \
	clariflute.aiff \
	clariflute.sdif \
	clarinet.dilated.sdif \
	flute.dilated.aiff \
	flute.dilated.sdif \
	flute.premorph.spc \
	flutello.aiff \
	flutello2.aiff \
	flutinet.aiff

# targets:

all: demo refs

demo:
	-tar zxf $(demo_source_tarball)
	python $(demoscript)
	
refs: 
	tar zxf $(demo_refs_tarball)

clean:
	rm -f $(demo_output)
	
distclean: clean
	-rm -fr source 
	-rm -fr reference