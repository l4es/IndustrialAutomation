//------------------------------------------------------------------------
//  Readme.txt
//------------------------------------------------------------------------
This project stationery is designed to get you up and running
quickly with CodeWarrior on your MPC5668G target.

The wizard has created the following CodeWarrior target(s):


The targets are set up for the selected CPU and options,
but can be easily modified.

          *************
          * IMPORTANT *
          *************

If your application will run from flash memory, you should
initialize some additional registers to improve system
performance. For MPC551x devices, initialize the PFCRP0 and 
PFCRP1 registers; for other devices, initialize the BIUCR.

Please review the MPC5668G reference manual to determine 
appropriate values.

Sample code for the following language(s) is at your disposal:


Other options may be available:

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
- Header Files: all application and runtime header files
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
