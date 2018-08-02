if HAVE_READLINE
   READLINE = -lreadline
else
   READLINE = 
endif

bin_PROGRAMS = daxc
daxc_SOURCES = daxc.c daxc.h daxc_tag.c daxc_db.c daxc_event.c
daxc_LDADD =  ../../lib/libdax.la $(READLINE)
#daxc_LDADD =  ../../lib/libdax.la

