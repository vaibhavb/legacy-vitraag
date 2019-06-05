#$Id: Makefile,v 1.1 2003/04/30 11:37:20 vaibhav Exp $

MAN_NAME = chaiman
MAIN_FILE = main

dvi:
	latex $(MAIN_FILE)
	bibtex $(MAIN_FILE)  
	makeindex $(MAIN_FILE).idx
	latex $(MAIN_FILE)

ps:
	\dvips -f < $(MAIN_FILE).dvi > $(MAN_NAME).ps

clean:
	rm -rf *.lo[gft] *.idx *.aux *.toc

showps:
	ghostview $(MAN_NAME).ps & 
