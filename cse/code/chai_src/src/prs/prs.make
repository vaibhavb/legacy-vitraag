CSRC += prsTree.c prsIntf.c prsMain.c 
HEADERS += prs.h prsInt.h prsIntf.h
LEXSRC += prs.l 
YACCSRC += prs.y 
GENERATEDCSRC += prsRead.c prsLex.c 

SCRIPTS += prsPreproc.tcl 

$(objectdir)/prsLex.c: prs.l 
	$(LEX) -o$(objectdir)/prsLex.c $< 
	-@chmod 0664 $(objectdir)/prsLex.c 

$(objectdir)/prsRead.c: prs.y prs.h prsInt.h 
	$(YACC) -v -d -t -o $(objectdir)/prsRead.c $< 
	-@chmod -f 0664 $(objectdir)/prsRead.c 
	mv $(objectdir)/prsRead.h $(local_srcdir)/src/prs/prsTokens.h 

$(objectdir)/prsLex.o: $(objectdir)/prsLex.c 

$(objectdir)/prsRead.o: $(objectdir)/prsRead.c 

DEPENDENCYFILES = $(CSRC)
