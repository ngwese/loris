# Copyright (C) 1999-2004, Kelly Fitz and Lippold Haken <loris@cerlsoundgroup.org>
#  
# This file is free software; as a special exception the author gives
# unlimited permission to copy and/or distribute it, with or without 
# modifications, as long as this notice is preserved.
# 
# This program is distributed in the hope that it will be useful, but
# WITHOUT ANY WARRANTY, to the extent permitted by law; without even the
# implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

EXTRA_DIST = LorisCDoc.html LorisCppDoc.html \
			 LorisPerlDoc.html LorisPythonDoc.html \
			 LorisTclDoc.html index.html \
			 Doxyfile
			 
# add something to build docs using doxygen, if it is present

CPP_HTML_DIR = cpp_html
HTML = ${CPP_HTML_DIR}/index.html

HTML: Doxyfile
	${DOXYGEN} 

html: ${HTML}

PDF = Loris${LORIS_MAJOR_VERSION}.${LORIS_MINOR_VERSION}.pdf

PDF: ${HTML}
	cd latex; make pdf
	cp latex/refman.pdf Loris${LORIS_MAJOR_VERSION}.${LORIS_MINOR_VERSION}.pdf

pdf: ${PDF}

pi:
	cp ${top_srcdir}/src/loris.h .


SCRIPTINGDIR = ${top_srcdir}/scripting

py:
	env PYTHONPATH=${SCRIPTINGDIR}:${SCRIPTINGDIR}/.libs pydoc -w loris; \
		mv loris.html loris_pydoc.html

MAINTAINERCLEANFILES = 	Makefile.in

