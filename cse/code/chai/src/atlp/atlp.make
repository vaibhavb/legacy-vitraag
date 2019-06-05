CSRC += atlpMain.c atlpUtil.c 
HEADERS += atlp.h atlpInt.h 
LEXSRC += atlp.l 
YACCSRC += atlp.y 
GENERATEDCSRC += atlpRead.c atlpLex.c 

$(objectdir)/atlpLex.c: atlp.l 
	$(LEX) -PAtlpYy -o$(objectdir)/atlpLex.c $< 
	-@chmod 0664 $(objectdir)/atlpLex.c 

$(objectdir)/atlpLex.o: $(objectdir)/atlpLex.c $(objectdir)/atlpRead.c 

$(objectdir)/atlpRead.c: atlp.y atlp.h atlpInt.h 
	$(YACC) -pAtlpYy -d -t -o $(objectdir)/atlpRead.c $< 
	-@chmod 0664 $(objectdir)/atlpRead.c 
	mv $(objectdir)/atlpRead.h $(local_srcdir)/src/atlp/atlpTokens.h 

$(objectdir)/atlpRead.o: $(objectdir)/atlpRead.c 

DEPENDENCYFILES = $(CSRC) 
