  $(OBJDIR)/frontc.cmo:   $(OBJDIR)/cabs.cmo   $(OBJDIR)/cabs2cil.cmi \
      $(OBJDIR)/clexer.cmi   $(OBJDIR)/cparser.cmi   $(OBJDIR)/cprint.cmo \
     $(OBJDIR)/errormsg.cmi   $(OBJDIR)/patch.cmi  $(OBJDIR)/pretty.cmi  $(OBJDIR)/stats.cmi \
     $(OBJDIR)/trace.cmi   $(OBJDIR)/frontc.cmi 
  $(OBJDIR)/frontc.cmx:   $(OBJDIR)/cabs.cmx   $(OBJDIR)/cabs2cil.cmx \
      $(OBJDIR)/clexer.cmi   $(OBJDIR)/cparser.cmx   $(OBJDIR)/cprint.cmx \
     $(OBJDIR)/errormsg.cmx   $(OBJDIR)/patch.cmx  $(OBJDIR)/pretty.cmx  $(OBJDIR)/stats.cmx \
     $(OBJDIR)/trace.cmx   $(OBJDIR)/frontc.cmi 
