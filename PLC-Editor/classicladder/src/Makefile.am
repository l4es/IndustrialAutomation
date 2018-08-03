bin_PROGRAMS = classicladder classicladdermat

classicladder_SOURCES = \
	classicladder.c \
	calc.c \
	drawing.c \
	edit.c \
	edit_gtk.c \
	editproperties_gtk.c \
	files.c \
	vars_access.c

classicladdermat_SOURCES = \
	classicladder.c \
	calc.c \
	drawing.c \
	edit.c \
	edit_gtk.c \
	editproperties_gtk.c \
	files.c \
	vars_mat.c

#INCLUDES = $(GTK_CFLAGS)

INCLUDES = @GTK_CFLAGS@
classicladder_LDADD = @GTK_LIBS@ 
classicladdermat_LDADD = @GTK_LIBS@ \
        ../../lib/libmatplc.la -lrt
