INCLUDES =  $(PYINCLUDE) $(TCLINCLUDE) $(PERLINCLUDE) -I$(top_srcdir)/src
EXTRA_DIST = loris.py loris.pm

#----------------------------------------------------------------
# If Python is available, define a target for building and
# installing the Python module
#
# cannot figure out how to have it build always, but 
# install conditionally
#
if HAVE_PYTHON_H
if HAVE_PYTHON_SITE
pythonsitepkgdir = $(PYSITE)
pythonsitepkg_SCRIPTS = loris.py loris.pyo
pythonsitepkg_LTLIBRARIES = loriscmodule.la
endif
endif

loriscmodule_la_SOURCES = loris_python_wrap.C
loriscmodule_la_LDFLAGS = -module -avoid-version
loriscmodule_la_LIBADD = $(top_builddir)/src/libclasses.la -lstdc++ 

loris.pyo: loris.py
	$(PYTHON) -O -c "import loris"	

noinst_LTLIBRARIES = tcLoris.la perLoris.la

tcLoris_la_SOURCES = loris_tcl_wrap.C
tcLoris_la_LDFLAGS = -module -avoid-version
tcLoris_la_LIBADD = $(top_builddir)/src/libclasses.la -lstdc++

perLoris_la_SOURCES = loris_perl_wrap.C
perLoris_la_LDFLAGS = -module -avoid-version
perLoris_la_LIBADD = $(top_builddir)/src/libclasses.la -lstdc++


#----------------------------------------------------------------
# If swig is available, define a target for building the
# wrappers from swig interface files.
#
if HAVE_SWIG
SWIG_INTERFACE = loris.i
PYTHON_WRAPPER = loris_python_wrap.C
TCL_WRAPPER = loris_tcl_wrap.C
PERL_WRAPPER = loris_perl_wrap.C

wrappers: 
	$(SWIG) -python -shadow -c++ -o $(PYTHON_WRAPPER) $(SWIG_INTERFACE)
	$(SWIG) -tcl -c++ -o $(TCL_WRAPPER) $(SWIG_INTERFACE)
	$(SWIG) -perl5 -c++ -o $(PERL_WRAPPER) $(SWIG_INTERFACE)
endif
