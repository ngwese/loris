# Copyright (C) 1999-2004, Kelly Fitz and Lippold Haken <loris@cerlsoundgroup.org>
#  
# This file is free software; as a special exception the author gives
# unlimited permission to copy and/or distribute it, with or without 
# modifications, as long as this notice is preserved.
# 
# This program is distributed in the hope that it will be useful, but
# WITHOUT ANY WARRANTY, to the extent permitted by law; without even the
# implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

EXTRA_DIST = Doxyfile.in opcodes.html
			 
noinst_DATA = 

# add something to build docs using doxygen, if it is present
if HAVE_DOXYGEN

noinst_DATA += html 


CPP_HTML_DIR = html
HTML = ${CPP_HTML_DIR}/index.html

${HTML}: Doxyfile
	${DOXYGEN} 

html: ${HTML}

PDF = Loris${LORIS_MAJOR_VERSION}.${LORIS_MINOR_VERSION}.pdf

${PDF}: ${HTML}
	cd latex; make 
	cp latex/refman.pdf $@

pdf: ${PDF}

endif

MAINTAINERCLEANFILES = 	Makefile.in Doxyfile

