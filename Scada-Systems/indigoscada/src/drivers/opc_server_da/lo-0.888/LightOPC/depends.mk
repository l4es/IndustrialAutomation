#####################################################
# Dependencies
#####################################################

advise_d.$(O): advise_d.cpp privopc.h options.h losynch.h lightopc.h \
 loserv.h reqqueue.h util.h growlist.h objtrack.h connpnt.h
advise_q.$(O): advise_q.cpp privopc.h options.h losynch.h lightopc.h \
 loserv.h reqqueue.h util.h growlist.h objtrack.h connpnt.h
anenum.$(O): anenum.cpp privopc.h options.h losynch.h lightopc.h loserv.h \
 reqqueue.h util.h growlist.h objtrack.h connpnt.h enum.h enum.cpp
browse.$(O): browse.cpp privopc.h options.h losynch.h lightopc.h loserv.h \
 reqqueue.h util.h growlist.h objtrack.h connpnt.h enum.h
cacheupd.$(O): cacheupd.c loserv.h lightopc.h losynch.h options.h util.h
callback.$(O): callback.cpp privopc.h options.h losynch.h lightopc.h \
 loserv.h reqqueue.h util.h growlist.h objtrack.h connpnt.h
clidown.$(O): clidown.cpp privopc.h options.h losynch.h lightopc.h loserv.h \
 reqqueue.h util.h growlist.h objtrack.h connpnt.h
connpnt.$(O): connpnt.cpp privopc.h options.h losynch.h lightopc.h loserv.h \
 reqqueue.h util.h growlist.h objtrack.h connpnt.h
dllmain.$(O): dllmain.c util.h options.h
dopack.$(O): dopack.cpp privopc.h options.h losynch.h lightopc.h loserv.h \
 reqqueue.h util.h growlist.h objtrack.h connpnt.h
enumgi.$(O): enumgi.cpp privopc.h options.h losynch.h lightopc.h loserv.h \
 reqqueue.h util.h growlist.h objtrack.h connpnt.h enum.h
filter.$(O): filter.cpp
geterr.$(O): geterr.c loserv.h lightopc.h losynch.h options.h util.h
groupman.$(O): groupman.cpp privopc.h options.h losynch.h lightopc.h \
 loserv.h reqqueue.h util.h growlist.h objtrack.h connpnt.h nan.h
growlist.$(O): growlist.c growlist.h util.h options.h
hr_clock.$(O): hr_clock.c loserv.h lightopc.h losynch.h options.h util.h
iniclr.$(O): iniclr.c loserv.h lightopc.h losynch.h options.h reqqueue.h \
 util.h
ioasync.$(O): ioasync.cpp privopc.h options.h losynch.h lightopc.h loserv.h \
 reqqueue.h util.h growlist.h objtrack.h connpnt.h
ioread.$(O): ioread.cpp privopc.h options.h losynch.h lightopc.h loserv.h \
 reqqueue.h util.h growlist.h objtrack.h connpnt.h
ioreal.$(O): ioreal.cpp privopc.h options.h losynch.h lightopc.h loserv.h \
 reqqueue.h util.h growlist.h objtrack.h connpnt.h
iowrite.$(O): iowrite.cpp privopc.h options.h losynch.h lightopc.h loserv.h \
 reqqueue.h util.h growlist.h objtrack.h connpnt.h
itemman.$(O): itemman.cpp privopc.h options.h losynch.h lightopc.h loserv.h \
 reqqueue.h util.h growlist.h objtrack.h connpnt.h
itemprop.$(O): itemprop.cpp privopc.h options.h losynch.h lightopc.h \
 loserv.h reqqueue.h util.h growlist.h objtrack.h connpnt.h proplist.h
lopcser.$(O): lopcser.cpp privopc.h options.h losynch.h lightopc.h loserv.h \
 reqqueue.h util.h growlist.h objtrack.h connpnt.h
lservice.$(O): lservice.cpp privopc.h options.h losynch.h lightopc.h \
 loserv.h reqqueue.h util.h growlist.h objtrack.h connpnt.h proplist.h
mallocx.$(O): mallocx.cpp util.h options.h
objtrack.$(O): objtrack.cpp privopc.h options.h losynch.h lightopc.h \
 loserv.h reqqueue.h util.h growlist.h objtrack.h connpnt.h
propdesc.$(O): propdesc.c util.h options.h
propenum.$(O): propenum.cpp privopc.h options.h losynch.h lightopc.h \
 loserv.h reqqueue.h util.h growlist.h objtrack.h connpnt.h proplist.h
proplist.$(O): proplist.c loserv.h lightopc.h losynch.h options.h \
 proplist.h util.h
proprel.$(O): proprel.c loserv.h lightopc.h losynch.h options.h util.h \
 proplist.h
realtag.$(O): realtag.c loserv.h lightopc.h losynch.h options.h util.h
reg_com.$(O): reg_com.c loserv.h lightopc.h losynch.h options.h util.h
reqqueue.$(O): reqqueue.c loserv.h lightopc.h losynch.h options.h \
 reqqueue.h
schedcl.$(O): schedcl.cpp privopc.h options.h losynch.h lightopc.h loserv.h \
 reqqueue.h util.h growlist.h objtrack.h connpnt.h
servunk.$(O): servunk.cpp privopc.h options.h losynch.h lightopc.h loserv.h \
 reqqueue.h util.h growlist.h objtrack.h connpnt.h
strutil.$(O): strutil.c util.h options.h
tagtype.$(O): tagtype.c loserv.h lightopc.h losynch.h options.h util.h
thrcontr.$(O): thrcontr.c loserv.h lightopc.h losynch.h options.h
upd_item.$(O): upd_item.cpp privopc.h options.h losynch.h lightopc.h \
 loserv.h reqqueue.h util.h growlist.h objtrack.h connpnt.h nan.h
varpack.$(O): varpack.c util.h options.h
wcmb.$(O): wcmb.c util.h options.h


losynch.$(O): losynch.c $(ULOGINCL)/rwlock.c $(ULOGINCL)/rwlock.h \
 $(ULOGINCL)/condsb.c $(ULOGINCL)/condsb.h

charray.$(O): charray.c lightopc.h

sample.$(O): sample.cpp $(ULOGINCL)/unilog.h lightopc.h

#############################################################
