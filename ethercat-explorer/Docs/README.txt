-------------------------------------------------------------------------------
                    EtherCAT Explorer base on SOEM lib
-------------------------------------------------------------------------------

1.  INTRO

	1.1 ABOUT
		EtherCAT is a graphical windows program for exploring and navigating
		EtherCAT slaves devices. 
		The project was created in order to teach/learn the protocol.		
		It's a fast developped code without any ambition of speed, stability,
		beauty of code.
		It just wrap 'Simple Open EtherCAT Master' native C code for .NET 
		and then use it. SOEM is by Arthur Ketels, all the complex work is  
		done in his lib, not in my graphical interface.
		This document is subject to change.

		Winpcap must be installed on the Pc : do it with Wireshark.
		.NET Framework 4.0 also, and Visual C++ 10 redistribuable (both
		should be OK on a not so old PC).

	1.2 CREDITS
		The project was created by F. Chaxel, in 2016. 
		
		Base on simple Open EtherCAT Master, by Arthur Ketels :
		http://openethercatsociety.github.io/
		EtherCAT specifications are available on https://www.ethercat.org
		for EtherCAT members.

		Graphics are the usual FamFamFam: http://www.famfamfam.com/
		It is, of course, inspired by the best Bacnet Explorer, Yabe :
		http://sourceforge.net/projects/yetanotherbacnetexplorer/
		also by Profinet Explorer
		http://sourceforge.net/projects/profinetexplorer/
		and my own EtherNet/IP Explorer
		http://sourceforge.net/projects/enipexplorer/

2.  USAGE

	2.1 Exploration
		- Start EtherCAT
		- Important : Remove the master from your EtherCAT network 
		- Select "Open Interface" under "Functions".
	2.2 Read Slaves Input data
		Inputs are displayed in the corresponding grid.
	2.3 Write Slaves Input data
		Outputs are displayed in the corresponding grid.
		Values modifications are sent to the slave (hit enter key).
	2.4 State Managment
		EtherCATExplorer try to put all slaves in Operational Mode
		on startup.
		State Managment menu could be use to change it individualy.
	2.5 Read & Write PDO
		Slave must support CoE -CAN over EtherCAT- mailbox protocol.
		Index is in hexadecimal, subIndex in decimal (could be -1 
		for all values).
		For writing you must respect the same format as read
		(2 digit hex values and 1 space).
		If a database of devices description is filled with the
		manufacturer XML files, the object dictionary help to
		get the Index (just click on it).
	2.4 Devices Database
		A database file could be associate. After this, the XML 
		device files could be upload inside. The database 
		become the default one and is open without any user action.
		No maintenance operation can be done. So make copy of
		the file (or keep the XML files).
	2.5 Slave EEPROM
		Read the EEPROM from the current selected device, or from a
		file.
		Read size is limited to 32Ko. Don't know if more is required.
		Can show it in Hex or Ascii : hit the Button on top of the Textbox.
		I've no way to test write, so it's not implemented. Moreover
		it's an option for hw/fw developpers and sure each one
		has a tool for that. Beckhoff TwinCat 3.0 can do it safely, even 
		with the 7 days "infinitely renewable" trial licence.

3.  SETTINGS OPTIONS
	3.1 RefreshStatePeriod
		- In seconds : read back slaves status & update the associated icons.
		  Set to -1 for nothing.
	3.1 DelayUpMs
		- In milliseconds, max time allows to "discover" the slaves.
	3.2 InputRefreshPeriod
		- In milliseconds (more than 100 ms)
	3.3 ReadOnly & ReadWrite colors are used in the Acyclic dialogBox 
	    to shows objects attributs status.

4.  TESTS
	The EtherCAT has been tested with :
		- Wago 750-354 I/O (vendor ID 0x00000021)
		- Infranor XtrapulsPac servo-controller (vendor ID 0x00000082)

5.  SUPPORT
	There's no support for the project at this time, and certainly never. 
	If you write to me, I'm unlikely to answer. 

6.  REPORT ERRORS
	Yes, there be errors alright. There always are. Many won't be interesting
	though. Eg. if you find a computer that behaves differently from others, 
	I'm unlikely to care. This is not a commercial project and I'm not trying 
	to push it to the greater good of the GPL world. (This may change though.)
	If you find a device that doesn't work with it, it might be interesting.
	But in order for me to fix it, I need either access to the physical device
	or printouts from programs like Wireshark, that displays the error.
	Write to me using the Sourceforge link.

7.  CONTRIBUTE
	Really? You think it's missing something? It's not really meant as a huge 
	project, but if you really must, try contacting me using the Sourceforge
	link.
	
8.  MISC
	Project web page is located at: 
	http://sourceforge.net/projects/ethercat-explorer/

	
The EtherCAT Technology, the trade name and logo "EtherCAT" are the intellectual 
property of, and protected by Beckhoff Automation GmbH. You can use 
EtherCATExplorer for the sole purpose of creating, using and/or selling or 
otherwise distributing an EtherCAT network master provided that an EtherCAT 
Master License is obtained from Beckhoff Automation GmbH.
In case you did not receive a copy of the EtherCAT Master License write
to Beckhoff Automation GmbH, Eiserstrasse 5, D-33415 Verl, Germany 
(www.beckhoff.com).
This tool is realized in the field of the IUT Nancy/Brabois ETG membership.