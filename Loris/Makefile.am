# Copyright (C) 1999-2001, Kelly Fitz and Lippold Haken <loris@cerlsoundgroup.org>
#  
# This file is free software; as a special exception the author gives
# unlimited permission to copy and/or distribute it, with or without 
# modifications, as long as this notice is preserved.
# 
# This program is distributed in the hope that it will be useful, but
# WITHOUT ANY WARRANTY, to the extent permitted by law; without even the
# implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

SUBDIRS = doc src mac pi scripting test

# used to have ltconfig in here too, but newer versions
# of libtool (>1.4) don't use that.
AUX_DIST = 	$(ac_aux_dir)/config.guess \
			$(ac_aux_dir)/config.sub \
			$(ac_aux_dir)/install-sh \
			$(ac_aux_dir)/ltmain.sh \
			$(ac_aux_dir)/missing \
			$(ac_aux_dir)/mkinstalldirs

AUX_DIST_EXTRA =

MAINTAINERCLEANFILES = 	Makefile.in aclocal.m4 configure config.h.in \
						stamp-h.in $(AUX_DIST)

EXTRA_DIST = reconf README-MAC


dist-hook:
	(cd $(distdir) && mkdir $(ac_aux_dir))
	for file in $(AUX_DIST) $(AUX_DIST_EXTRA); do \
		cp $$file $(distdir)/$$file; \
	done
