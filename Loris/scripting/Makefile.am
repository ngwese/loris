INCLUDES =  $(PYINCLUDE) $(TCLINCLUDE) $(PERLINCLUDE) -I$(top_srcdir)/src
EXTRA_DIST = loris.py loris.pm

MAINTAINERCLEANFILES = Makefile.in

PYTHON_WRAPPER = loris_python_wrap.C
TCL_WRAPPER = loris_tcl_wrap.C
PERL_WRAPPER = loris_perl_wrap.C

#----------------------------------------------------------------
# If Python is available, define a target for building and
# installing the Python module
#
if HAVE_PYTHON_H
if HAVE_SITEPY
pythonsitepkgdir = $(SITEPY)
else
pythonsitepkgdir = $(pkglibdir)/python
endif
pythonsitepkg_SCRIPTS = loris.py loris.pyo
pythonsitepkg_LTLIBRARIES = loriscmodule.la
endif

loriscmodule_la_SOURCES = $(PYTHON_WRAPPER)
loriscmodule_la_LDFLAGS = -module -avoid-version
loriscmodule_la_LIBADD = $(top_builddir)/src/libclasses.la -lstdc++ 


# This should be an install hook, but on my machine, the
# install hook does not execute _after_ everything is 
# installed, as it is documented to do, it executes before
# loris.py and loriscmodule.a are installed, and thus fails.
loris.pyo: loris.py
	env PYTHONPATH=$(srcdir):.libs $(PYTHON) -O -c "import loris"


#----------------------------------------------------------------
# If Tcl is available, define a target for building and
# installing the PythonTcl module
#
if HAVE_TCL_H
tclsitepkgdir = $(pkglibdir)/tcl
tclsitepkg_LTLIBRARIES = tcLoris.la
endif

tcLoris_la_SOURCES = $(TCL_WRAPPER)
tcLoris_la_LDFLAGS = -module -avoid-version
tcLoris_la_LIBADD = $(top_builddir)/src/libclasses.la -lstdc++

#----------------------------------------------------------------
# Don't install the Perl module, it is untested.
#
noinst_LTLIBRARIES = loris_perl.la
noinst_SCRIPTS = loris.pm

loris_perl_la_SOURCES = $(PERL_WRAPPER)
loris_perl_la_LDFLAGS = -module -avoid-version
loris_perl_la_LIBADD = $(top_builddir)/src/libclasses.la -lstdc++


#----------------------------------------------------------------
# If swig is available, define a target for building the
# wrappers from swig interface files.
#
if HAVE_SWIG
SWIG_INTERFACE = loris.i

wrappers: 
	$(SWIG) -python -shadow -c++ -o $(PYTHON_WRAPPER) $(SWIG_INTERFACE)
	$(SWIG) -tcl -c++ -o $(TCL_WRAPPER) $(SWIG_INTERFACE)
	$(SWIG) -perl5 -c++ -o $(PERL_WRAPPER) $(SWIG_INTERFACE)
	mv loris_perl.pm loris.pm
endif
