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

demo_output = cell.dilated.aiff \
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

CONFIG_CLEAN_FILES = $(demo_sources)

# am I allowed to define this?
clean-generic:
	rm -f $(demo_output)

# targets:

all: demo refs

demo:
	tar zxf $(demo_source_tarball)
	env PYTHONPATH=$(top_srcdir)/scripting:$(top_builddir)/scripting/.libs $(PYTHON) $(demoscript)
	
refs: 
	tar zxf $(demo_refs_tarball)

