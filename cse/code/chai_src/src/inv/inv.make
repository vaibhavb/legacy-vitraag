CSRC += invMain.c invUtil.c
HEADERS += inv.h invInt.h
LEXSRC += inv.l
YACCSRC += inv.y
GENERATEDCSRC += invRead.c invLex.c

$(objectdir)/invLex.c: inv.l
	$(LEX) -PInvYy -o$(objectdir)/invLex.c $< 
	-@chmod 0664 $(objectdir)/invLex.c


$(objectdir)/invRead.c: inv.y inv.h invInt.h
	$(YACC) -pInvYy -d -t -o $(objectdir)/invRead.c $<
	-@chmod 0664 $(objectdir)/invRead.c
	mv $(objectdir)/invRead.h $(local_srcdir)/src/inv/invTokens.h

$(objectdir)/invLex.o: $(objectdir)/invLex.c

$(objectdir)/invRead.o: $(objectdir)/invRead.c

DEPENDENCYFILES = $(CSRC)
