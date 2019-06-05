#$Id: Makefile,v 1.1.1.1 2003/06/17 06:35:53 vaibhav Exp $
MAN_NAME = chaiman
MAIN_FILE = main
WWW_FILE_BAKUP = main.bak
WWWDIR=/projects/bubble/dvlab/p/

dvi:
	latex $(MAIN_FILE)
	#bibtex $(MAIN_FILE)  
	makeindex $(MAIN_FILE).idx
	latex $(MAIN_FILE)

ps:
	dvips -f < $(MAIN_FILE).dvi > $(MAN_NAME).ps

clean:
	rm -rf *.lo[gft] *.idx *.aux *.toc *~ *.blg *.ilg *.bbl *.ind

showps:
	ghostview $(MAN_NAME).ps & 

html:
	latex2html -ascii_mode -title "Chai Manual" -noinfo $(MAIN_FILE)

webput:
	mv $(WWWDIR)/$(MAIN_FILE) $(WWWDIR)/$(WWW_FILE_BAKUP)
	cp -r $(MAIN_FILE) $(WWWDIR)
	chmod -R a+rx $(WWWDIR)$(MAIN_FILE)
