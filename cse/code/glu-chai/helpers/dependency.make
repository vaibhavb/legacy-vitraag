# FileName	[dependency.make]
#
# PackageName	[vis]
#
# Synopsis	[Makefile to automatically create the dependency files.]
#
# Description	[This file is called from the main Makefile.]
#
# SeeAlso	[Makefile.in]
#
# Author	[Abelardo Pardo <abel@boulder.colorado.edu>]
#
# Copyright	[This file was created at the University of Colorado at Boulder.
#  The University of Colorado at Boulder makes no warranty about the suitability
#  of this software for any purpose.  It is presented on an AS IS basis.]
#
# Revision	[$Id: dependency.make,v 1.1.1.1 2003/04/24 23:35:11 ashwini Exp $]

include $(PKGNAME).make

ifdef DEPENDENCYFILES
$(PKGNAME).d : $(DEPENDENCYFILES) $(HEADERS) $(PKGNAME).make
	@echo -n "Dependency file $(PKGNAME).d->"
	@$(CC) -MM $(CFLAGS) $(AC_FLAGS) $(INCLUDEDIRS) \
	$(filter %.c, $^) | \
	sed 's/^[a-zA-Z0-9_]*\.o[ :]*/$(objectdir)\/&/g' > $@
	@echo "Created."
else
$(PKGNAME).d :
	@echo "No dependency files given in $(PKGNAME).make"
	@echo "Define the variable DEPENDENCYFILES to create the dependencies"
endif
