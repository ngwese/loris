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
				 lorisPartialList.i lorisSampleVector.i

PY_WRAPPER = loris_python_wrap.C
PY_MODULE = loriscmodule.so

PERL_WRAPPER = loris_perl_wrap.C
PERL_MODULE = loris.so

TCL_WRAPPER = loris_tcl_wrap.C
TCL_MODULE = loris.so

PY_VERSION = 1.5
PYTHON_PREFIX = /usr
PYTHON_INCLUDE = ${PYTHON_PREFIX}/include/python${PY_VERSION}
# PYTHON_LIB = ${PYTHON_PREFIX}/lib/python${PY_VERSION}/config

# PERL_INCLUDE = /System/Library/Perl/darwin/CORE/
PERL_INCLUDE = /usr/lib/perl5/5.005/i386-linux/CORE/

TCL_INCLUDE = /usr/include

CC = c++
CFLAGS = -O2 -I${CLASSES} -Wno-multichar -xc++

python: classes pywrap
	${CC} -shared $(PY_WRAPPER:c++=o) $(OBJ_LORIS) ${LIB_LORIS} -o $(OUTPUT)/$(PY_MODULE)
	-mv loris.py $(OUTPUT)/loris.py

perl: classes perlwrap
	${CC} -shared $(PERL_WRAPPER:c++=o) $(OBJ_LORIS) ${LIB_LORIS} -o $(OUTPUT)/$(PERL_MODULE)
	-mv loris.pm $(OUTPUT)/loris.pm

tcl: classes tclwrap
	${CC} -shared $(TCL_WRAPPER:c++=o) $(OBJ_LORIS) ${LIB_LORIS} -o $(OUTPUT)/$(TCL_MODULE)

pywrap: $(PY_WRAPPER:c++=o)

$(PY_WRAPPER:c++=o): $(PY_WRAPPER)
	$(CC) $(CFLAGS) -I$(PYTHON_INCLUDE) -c $(PY_WRAPPER)

$(PY_WRAPPER): $(SWIG_INTERFACE) $(SUB_INTERFACES)
	$(SWIG) -python -shadow -c++ -o $(PY_WRAPPER) $(SWIG_INTERFACE)

perlwrap: $(PERL_WRAPPER:c++=o)

$(PERL_WRAPPER:c++=o): $(PERL_WRAPPER)
	$(CC) $(CFLAGS) -I$(PERL_INCLUDE) -c $(PERL_WRAPPER)

$(PERL_WRAPPER): $(SWIG_INTERFACE) $(SUB_INTERFACES)
	$(SWIG) -perl5 -c++ -o $(PERL_WRAPPER) $(SWIG_INTERFACE)
	
tclwrap: $(TCL_WRAPPER:c++=o)

$(TCL_WRAPPER:c++=o): $(TCL_WRAPPER)
	$(CC) $(CFLAGS) -I$(TCL_INCLUDE) -c $(TCL_WRAPPER)

$(TCL_WRAPPER): $(SWIG_INTERFACE) $(SUB_INTERFACES)
	$(SWIG) -tcl -c++ -o $(TCL_WRAPPER) $(SWIG_INTERFACE)

classes:
	cd $(CLASSES); $(MAKE)
	
clean:
	-rm ./*.o 
	-rm $(OUTPUT)/$(PY_MODULE) 
	-rm $(OUTPUT)/$(PERL_MODULE) 
	-rm $(OUTPUT)/$(TCL_MODULE)

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
