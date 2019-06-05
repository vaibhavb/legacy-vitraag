  $(OBJDIR)/cabs2cil.cmo:   $(OBJDIR)/cabs.cmo  $(OBJDIR)/cil.cmi \
      $(OBJDIR)/cprint.cmo  $(OBJDIR)/errormsg.cmi  $(OBJDIR)/pretty.cmi  $(OBJDIR)/trace.cmi \
     $(OBJDIR)/util.cmo   $(OBJDIR)/cabs2cil.cmi 
  $(OBJDIR)/cabs2cil.cmx:   $(OBJDIR)/cabs.cmx  $(OBJDIR)/cil.cmx \
      $(OBJDIR)/cprint.cmx  $(OBJDIR)/errormsg.cmx  $(OBJDIR)/pretty.cmx  $(OBJDIR)/trace.cmx \
     $(OBJDIR)/util.cmx   $(OBJDIR)/cabs2cil.cmi 
