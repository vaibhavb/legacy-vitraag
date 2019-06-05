# Generated automatically from Makefile.in by configure.
# Makefile for extdoc
#
# Requires GNU's make
#
# Stephen Edwards <sedwards@eecs.berkeley.edu>

PRODUCT = ext
VERSION = 1.3

#### Start of system configuration section. ####

srcdir = .
VPATH = .:./doc

INSTALL = /usr/bin/install -c
INSTALL_PROGRAM = ${INSTALL}
INSTALL_DATA = ${INSTALL} -m 644

PERL = /usr/bin/perl
M4 = /usr/bin/m4

SCRIPTS = extdoc extindex
MANPAGES = extdoc extproto extindex
MANTROFF = $(addsuffix .1,$(MANPAGES))

transform = s,x,x,

prefix = /usr/local
exec_prefix = ${prefix}

# Directory in which to install scripts
bindir = $(exec_prefix)/bin

man_dir = $(prefix)/man
man1_dir = $(man_dir)/man1

#### End of system configuration section. ####

SHELL = /bin/sh

PERLFILES = extdoc.pl extindex.pl

DOCFILES = $(addsuffix .html, index extdoc extindex extproto naming packages related) \
	title_ext.gif

TEMPLATES = $(addsuffix .templ, \
	CFile CHeaderFile Enum Function Macro Struct Variable )

# Files that can be generated, but should be up-to-date for a distribution
DISTDEP = Makefile

# Files to distribute
DISTFILES = README INSTALL NEWS \
	configure configure.in \
	Makefile.in \
	extdoc.pl extindex.pl htmltoman.pl \
	ext.m4 \
	$(addprefix doc/,$(DOCFILES)) \
	$(addprefix templates/,$(TEMPLATES)) \
	install-sh mkinstalldirs

DISTDIRS = doc templates

DISTRIBUTION = $(PRODUCT)-$(VERSION)

editpl = sed -e 's,@''PERL''@,$(PERL),g'

all: $(SCRIPTS) $(MANTROFF) ext.el

.SUFFIXES:
.SUFFIXES: .pl .1 .html .m4 .el

# Fix Perl files by running "editpl"

.pl:
	rm -f $@ $@.tmp
	$(editpl) $< > $@.tmp && chmod +x $@.tmp && mv $@.tmp $@

# Convert HTML to troff (man) format by running htmltoman

.html.1:
	$(srcdir)/htmltoman $< > $@

# Convert .m4 files to .el by running them through m4

.m4.el:
	$(M4) $< > $@

dist: $(DISTDEP) $(DISTFILES)
	rm -rf $(DISTRIBUTION)
	mkdir $(DISTRIBUTION)
	for dir in $(DISTDIRS); do \
	  mkdir $(DISTRIBUTION)/$$dir; \
	done
	@echo "Copying distribution files"
	@for file in $(DISTFILES); do \
	  echo "  $$file"; \
	  ln $(srcdir)/$$file $(DISTRIBUTION)/$$file 2> /dev/null \
	    || cp -p $(srcdir)/$$file $(DISTRIBUTION)/$$file; \
	done
	tar cof - $(DISTRIBUTION) | gzip > $(DISTRIBUTION).tar.gz
	rm -rf $(DISTRIBUTION)

install: installdirs all
	$(INSTALL_PROGRAM) extdoc \
		${bindir}/`echo extdoc | sed '$(transform)'`
	$(INSTALL_PROGRAM) extindex \
		${bindir}/`echo extindex | sed '$(transform)'`
	rm -f ${bindir}/`echo extproto | sed '$(transform)'`
	ln ${bindir}/`echo extdoc | sed '$(transform)'` \
		${bindir}/`echo extproto | sed '$(transform)'`
	for manpage in $(MANPAGES); do \
		$(INSTALL_DATA) $$manpage.1 \
			${man1_dir}/`echo $$manpage | sed '$(transform)'`.1; \
	done

installdirs:
	$(srcdir)/mkinstalldirs $(bindir) $(man1_dir)

uninstall:
	rm -f ${bindir}/`echo extdoc | sed '$(transform)'` \
		${bindir}/`echo extproto | sed '$(transform)'` \
		${bindir}/`echo extindex | sed '$(transform)'`
	for manpage in $(MANPAGES); do \
		rm -f ${man1_dir}/`echo $$manpage | sed '$(transform)'`.1; \
	done

Makefile: Makefile.in config.status
	./config.status

config.status: configure
	./config.status --recheck

configure: configure.in
	cd $(srcdir) && autoconf

$(MANPAGES): htmltoman

clean mostlyclean:
	rm -f ${SCRIPTS} htmltoman *.tmp *.1 *.el

distclean: clean
	rm -f Makefile config.status config.cache config.log

maintainer-clean realclean: distclean
	@echo "This command is intended for maintainers to use;"
	@echo "rebuilding the deleted files requires makeinfo."
	rm -rf *.ps html
