 -------------------------------------------------

	Guidelines for Makefile Creation
	and the EiC project directory


		    by

	   	Ed. Breen


 -------------------------------------------------
 ---------TABLE-of-CONTENTS---------

1. STANDARD MAKEFILE TARGETS.
        all
        install
        clean
        clobber
        showdep
        lib
2. DATA DICTIONARY
        Makefile paramaters:
           TOP
           CONFIG_DIR
           INCLUDES
           WARNINGS
           DEFINES
           LIB
           LIBS
           PLATFORM
           TOP_DIR
           INSTALL_DIR
           INCLUDE_DIR
           LIB_DIR
           STD_DEFINES

 --END-of-TABLE-of-CONTENTS---------

 ---------------------------------------------------
- STANDARD MAKEFILE TARGETS.

-- all
	Build all targets
-- install
	Install all targets and build if needed.
-- clean
	Delete all derived files except for
	targets.
-- clobber
	Delete all derived files, targets and
	installed info, such as man pages etc.
-- showdep
	Show the include dependencies of all source files
	in $(SRCS). It does not show system include dependencies.
-- lib
	build libraries; a subset of all.

 --------------------------------------------------------

- DATA DICTIONARY

-- Makefile paramaters:
	The scope of any local parameters is restricted to  the
	local or current directory.

--- TOP
	Top directory of project:

		TOP
		 |
		 +----- Level 1 dir
                 |       |
		 |       +------- Level 2 dir
		 |
		 +----- Level 1a dir
		 |
                etc

--- CONFIG_DIR
	Directory where the  config files are held.

--- INCLUDES
	Directories to search for include files.

--- WARNINGS
	Local warnings passed to the C compiler, eg

		WARNINGS = -Wall
--- DEFINES
	Local definitions passed to the C compiler.

--- LIB
	Local library to build

--- LIBS
	Libraries to link against: specific to local programs.

--- PLATFORM

	Parameter used to specify platform types: Currently the
	platforms are designated as:

          :----------------:--------------:
	  |  PLATFORM      |  DEFINE_FLAG |
          :----------------:--------------:
	  |  sparc station |   _SPARC     |
	  |  solaris       |   _SOLARIS   |
	  |  irix          |   _IRIX      |
	  |  ultrix        |   _ULTRIX    |
	  |  linux         |   _LINUX     |
	  |  Dec Alpha     |   _OSF1      |
	  |  HPUX          |   _HPUX      |
	  |  NetBSD        |   _NETBSD    |
	  :----------------:--------------:

--- TOP_DIR

	The top directory of the EiC code tree.  The figure below
        shows the relationship between TOP and TOP_DIR. It is part of
        the EiC directory project tree.

        EiC  (TOP = .)   (TOP_DIR = $(HOME)/EiC)
	 |
         +------ module (TOP = ..)    (TOP_DIR = $(HOME)/EiC)
         |        |
         |        +------- stdClib (TOP = ../..)   (TOP_DIR = $(HOME)/EiC)
        etc       |
               	 etc

--- INSTALL_DIR

	The directory to install the EiC binary (eic)

		Privately it might be  $(HOME)/bin.
		Publicly  it  might be /usr/bin

	By default EiC is configured to install privately.
	However, after playing with it you may decide
	to install it publicly.

--- INCLUDE_DIR EiC's 

	The include directory, $(TOP_DIR)/include, is where all
	modules should store their header files that are to be
	included into EiC at runtime. For example, EiC's default
	startup.h file, and the standard runtime C
	library headers such as stdio.h etc are stored here.

--- LIB_DIR
	EiC's library directory. $(TOP_DIR)/lib

--- STD_DEFINES

        Standard definitions are definitions that are passed to every
        compile command. This lets EiC programs know where every thing
        is located.


 ---------------------------------------------
