# Copyright (C) 1999-2001, Kelly Fitz and Lippold Haken <loris@cerlsoundgroup.org>
#  
# This file is free software; as a special exception the author gives
# unlimited permission to copy and/or distribute it, with or without 
# modifications, as long as this notice is preserved.
# 
# This program is distributed in the hope that it will be useful, but
# WITHOUT ANY WARRANTY, to the extent permitted by law; without even the
# implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

SUBDIRS = doc src scripting csound test

MAINTAINERCLEANFILES = 	Makefile.in aclocal.m4 configure config.h.in \
						stamp-h.in \
 			$(ac_aux_dir)/config.guess \
            $(ac_aux_dir)/config.sub \
            $(ac_aux_dir)/install-sh \
            $(ac_aux_dir)/ltmain.sh \
            $(ac_aux_dir)/missing \
            $(ac_aux_dir)/mkinstalldirs \
            $(ac_aux_dir)/depcomp

DISTCLEANFILES =

EXTRA_DIST = reconf \
			$(ac_aux_dir)/config.guess \
            $(ac_aux_dir)/config.sub \
            $(ac_aux_dir)/install-sh \
            $(ac_aux_dir)/ltmain.sh \
            $(ac_aux_dir)/missing \
            $(ac_aux_dir)/mkinstalldirs

DEMO_DIST_FILES =	Loris/demo/Makefile \
					Loris/demo/README \
					Loris/demo/morphdemo.py \
					Loris/demo/source \
					Loris/demo/reference

dist-demo:
	cd $(top_srcdir)/demo; tar zxf morphsources.tgz; tar zxf morphreference.tgz
	cd $(top_srcdir)/..; tar zcf Loris/$(PACKAGE)-demo-$(VERSION_STR).tgz $(DEMO_DIST_FILES)
