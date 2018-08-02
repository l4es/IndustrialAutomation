#################################################
#   Objects declarations for a Makefile
#################################################

OBJS0 = advise_d.$(O) advise_q.$(O) anenum.$(O) browse.$(O) cacheupd.$(O) \
 callback.$(O) clidown.$(O) connpnt.$(O) dopack.$(O) enumgi.$(O) filter.$(O) \
 geterr.$(O) groupman.$(O) growlist.$(O) iniclr.$(O) tagtype.$(O) \
 ioasync.$(O) ioread.$(O) iowrite.$(O) ioreal.$(O) hr_clock.$(O) \
 itemman.$(O) itemprop.$(O) lopcser.$(O) lservice.$(O) objtrack.$(O) \
 propdesc.$(O) propenum.$(O) proplist.$(O) proprel.$(O) realtag.$(O) \
 reg_com.$(O) reqqueue.$(O) schedcl.$(O) servunk.$(O) strutil.$(O) \
 thrcontr.$(O) upd_item.$(O) varpack.$(O) wcmb.$(O) mallocx.$(O) \
 losynch.$(O) charray.$(O)
OBJS=$(OBJS0) dllmain.$(O) 

#################################################
#   Common targets and rules 
#################################################

$(DLL):

all: $(ALLTARG) 

clean-all: clean
	-del $(ALLTARG) $(TEST)

clean:
	-del $(OBJS) sample.$(O)
	-del lightopc.exp lightopc.map
	-del lopcsamp.map 
	-del lopcsamd.map lopcsamd.lib lopcsamd.exp 

.cpp.$(O):
	$(CC) -c $(CFLAGS) $(INCLUDES) $<

.c.$(O):
	$(CC) -c $(CFLAGS) $(INCLUDES) $<


include depends.mk

##################################################
