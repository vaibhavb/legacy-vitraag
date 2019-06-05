#$Id: Makefile,v 1.2 2003/10/28 00:25:18 vaibhav Exp $
DOC_NAME = theisis
MAIN_FILE = main
WWWDIR=/projects/bubble/dvlab/p/

dvi:
	latex $(MAIN_FILE)
	bibtex $(MAIN_FILE)  
	makeindex $(MAIN_FILE).idx
	latex $(MAIN_FILE)
	latex $(MAIN_FILE)

ps:
	dvips -f < $(MAIN_FILE).dvi > $(DOC_NAME).ps

clean:
	rm -rf *.lo[gft] *.idx *.aux *.toc *~ *.blg *.ilg *.bbl *.ind *~

showps:
	ghostview $(DOC_NAME).ps & 

html:
	latex2html -ascii_mode -title "Theisis" -noinfo $(MAIN_FILE)

webput:
	mv $(WWWDIR)/$(MAIN_FILE) $(WWWDIR)/$(WWW_FILE_BAKUP)
	cp -r $(MAIN_FILE) $(WWWDIR)
	chmod -R a+rx $(WWWDIR)$(MAIN_FILE)
