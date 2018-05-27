//------------------------------------------------------------------------
//  Readme.txt
//------------------------------------------------------------------------
This project stationery is designed to get you up and running
quickly with CodeWarrior for your MPC551x target.

- RAM:
  This CodeWarrior target is setup to load and debug code from RAM (debug version).

- internal_FLASH:
  This CodeWarrior target is setup to debug code in internal Flash.
  The code must previously have been programmed into flash.

If "e200z0" was selected in addition to the e200z1, the PRC1's main
procedure will be renamed "main_p1" to allow both procedures to be
linked into the same image.

The linker configuration files for this project are defined to
allow this code to be loaded into memory without conflicting or
overlapping the memory sections used by PRC0 core's code.

Sample code for the following language(s) is at your disposal:
- C


//------------------------------------------------------------------------
//  Getting Started
//------------------------------------------------------------------------
To build  your project, use the menu Project > Make or press F7.
This will build the selected target.

//------------------------------------------------------------------------
// Project structure
//------------------------------------------------------------------------
The project generated contains various files/groups:
- readme.txt: this file
- Sources: folder with the application source code
- Startup Code: user customizable startup code 
- Lcf Files: the linker command files for the different build targets.
- Runtime: the libraries

//------------------------------------------------------------------------
//  Adding your own code
//------------------------------------------------------------------------
Once everything is working as expected, you can begin adding your own code
to the project. Keep in mind that we provide this as an example of how to
get up and running quickly with CodeWarrior. There are certainly other
ways to handle interrupts and set up your linker command file. Feel free
to modify any of the source files provided.

//------------------------------------------------------------------------
//  Additional documentation
//------------------------------------------------------------------------
Read the online documentation provided. From within the CodeWarrior IDE,
select Help > CodeWarrior Help.

//------------------------------------------------------------------------
//  Contacting Freescale
//------------------------------------------------------------------------
For bug reports, technical questions, and suggestions, please use the
forms installed in the Release_Notes folder and send them to:
cw_support@freescale.com
