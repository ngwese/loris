#
#   This is the Loris C++ Class Library, implementing analysis, 
#   manipulation, and synthesis of digitized sounds using the Reassigned 
#   Bandwidth-Enhanced Additive Sound Model.
#   
#   Loris is Copyright (c) 1999-2000 by Kelly Fitz and Lippold Haken
#  
#   This program is free software; you can redistribute it and/or modify
#   it under the terms of the GNU General Public License as published by
#   the Free Software Foundation; either version 2 of the License, or
#   (at your option) any later version.
#  
#   This program is distributed in the hope that it will be useful,
#   but WITHOUT ANY WARRANTY, without even the implied warranty of
#   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
#   GNU General Public License for more details.
#  
#   You should have received a copy of the GNU General Public License
#   along with this program; if not, write to the Free Software
#   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
#  
#  
#   Makefile for building the loris Python extension module.
#  
#   Kelly Fitz, Jan 2000
#   loris@cerlsoundgroup.org
#  
#   http://www.cerlsoundgroup.org/Loris/
#  
# 

#	use Loris rules file
TOP = ../..
include $(TOP)/Loris/classes/Rules.loris

SWIG = /usr/local/bin/swig
SWIG_INTERFACE = loris.i
SUB_INTERFACES = lorisAiffFile.i lorisAnalyzer.i lorisBpEnvelope.i \
				 lorisExportSpc.i lorisPartialList.i lorisSampleVector.i

PY_WRAPPER = loris_python_wrap.C
PY_MODULE = loriscmodule.so

PERL_WRAPPER = loris_perl_wrap.C

PY_VERSION = 2.1
PYTHON_PREFIX = /usr/local
PYTHON_INCLUDE = ${PYTHON_PREFIX}/include/python${PY_VERSION}
PYTHON_LIB = ${PYTHON_PREFIX}/lib/python${PY_VERSION}/config
INC_PY = -DHAVE_CONFIG_H -I${PYTHON_INCLUDE} -I${PYTHON_LIB}

PERL_CORE = /System/Library/Perl/darwin/CORE/

CC = c++
CFLAGS = -O2 ${INC_PY} -I${PI} -I${CLASSES} -Wno-multichar -xc++

python: $(PY_MODULE)

$(PY_MODULE): lorislib pywrap
	${CC} -shared $(PY_WRAPPER:c++=o) $(OUTPUT)/libloris.a ${LIB_LORIS} -o $(OUTPUT)/loriscmodule.so
	-mv loris.py $(OUTPUT)/loris.py

lorislib: 
	cd $(PI); $(MAKE) lib
	
pywrap: $(PY_WRAPPER:c++=o)

$(PY_WRAPPER:c++=o): $(PY_WRAPPER)
	$(CC) $(CFLAGS) -c $(PY_WRAPPER)

$(PY_WRAPPER): $(SWIG_INTERFACE) $(SUB_INTERFACES)
	$(SWIG) -python -shadow -c++ -o $(PY_WRAPPER) $(SWIG_INTERFACE)

perlwrap:
	$(SWIG) -perl5 -c++ -o $(PERL_WRAPPER) $(SWIG_INTERFACE)
	$(CC) $(CFLAGS) -I$(PERL_CORE) -c $(PERL_WRAPPER)
	
#
#   dependencies:
#   need the sed thing because g++ doesn't recognize c++ suffix,
#   so it does the wrong thing with the file names (.C.o)
#
depend:
	${CC} -MM $(CFLAGS) *.C | sed 's/c++\.o/\o/g' > dependfile

include dependfile

clean:
	-rm ./*.o $(OUTPUT)/loriscmodule.so

veryclean: clean
	cd $(CLASSES); $(MAKE) clean

#
#	suffixes:
#
.SUFFIXES: .c .C .o

.C.o:
	$(CC) -c ${CFLAGS} $<

.c.o:
	$(cc) -c ${cFLAGS} $<
