#/******************************************************************************
# *
# *  Copyright (C) 2008-2009  Andreas Stewering-Bone, Ingenieurgemeinschaft IgH
# *
# *  This file is part of the IgH EtherLAB Scicos Toolbox.
# *  
# *  The IgH EtherLAB Scicos Toolbox is free software; you can
# *  redistribute it and/or modify it under the terms of the GNU Lesser General
# *  Public License as published by the Free Software Foundation; version 2.1
# *  of the License.
# *
# *  The IgH EtherLAB Scicos Toolbox is distributed in the hope that
# *  it will be useful, but WITHOUT ANY WARRANTY; without even the implied
# *  warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# *  GNU Lesser General Public License for more details.
# *
# *  You should have received a copy of the GNU Lesser General Public License
# *  along with the IgH EtherLAB Scicos Toolbox. If not, see
# *  <http://www.gnu.org/licenses/>.
# *  
# *  ---
# *  
# *  The license mentioned above concerns the source code only. Using the
# *  EtherCAT technology and brand is only permitted in compliance with the
# *  industrial property and similar rights of Beckhoff Automation GmbH.
# *
# *****************************************************************************/
#package require Tk

proc check_stateport {} { global slaveselected; global selectid; global .eltop; global slavestring; global slavestate; global showstate;  set slaveselected [.eltop.list get [.eltop.list curselection]]; set selectid [.eltop.list curselection]; set selstate  $slavestate([.eltop.list curselection]); if {$selstate == 1} then { .eltop.showstatus configure -state normal } else {.eltop.showstatus configure -state disable; set showstate 0;}; }




#array set slavestring { 0 EL5001V1 1 EL5001V2}
#array set slavestate { 0 1 1 1}
#set slaveselected EL5001V1
#set selectid 0
#set masterid 0
#set domainid 0
#set slavealias 0
#set slaveposition 0
#set showstate 0
#set frameerror 0
#set inhibit 0
#set ssicode 0
#set baud 0
#set frame 0
#set framesize 25
#set datalength 24
#set inhibittime 0



set okstate 0
set cancelstate 0
set eltop .eltop
toplevel .eltop
wm title .eltop "Configure Beckhoff EL5001"
label .eltop.ellabel -text "Selected Slavetype:" -justify center
entry .eltop.showslave -textvariable slaveselected -justify center
scrollbar .eltop.h -orient horizontal -command ".eltop.list xview"
scrollbar .eltop.v -command ".eltop.list yview"
listbox .eltop.list -selectmode single -width 20 -height 10 -setgrid 1 -xscroll ".eltop.h set" -yscroll ".eltop.v set"
checkbutton .eltop.showstatus -text {Enable Statusport} -justify center -variable showstate 

label .eltop.masteridlabel -text "Master ID:" -justify right
entry .eltop.masteridentry -textvariable masterid -justify left
label .eltop.domainidlabel -text "Domain ID:" -justify right
entry .eltop.domainidentry -textvariable domainid -justify left
label .eltop.slavealiaslabel -text "Slave Alias:" -justify right
entry .eltop.slavealiasentry -textvariable slavealias -justify left
label .eltop.slavepositionlabel -text "Slave Position:" -justify right
entry .eltop.slavepositionentry -textvariable slaveposition -justify left
button .eltop.setok -text "Ok" -justify center -command {global okstate; set okstate 1}
button .eltop.setcancel -text "Cancel" -justify center -command {global okstate; global cancelstate; set okstate 0; set cancelstate 1}

frame .eltop.frameconfig -width 0.5i -height 0.5i -relief ridge -bd 2

label .eltop.frameconfig.dlabeldummy -text "              " -justify left

#Show Frame Error
label .eltop.frameconfig.labelframeerror -text "Disable Frame Error" -justify left
checkbutton .eltop.frameconfig.checkframeerror  -justify center -variable frameerror
label .eltop.frameconfig.labelinhibit -text " Enable Inhibit Mode" -justify left
checkbutton .eltop.frameconfig.checkinhibit  -justify center -variable inhibit

#Select SSI-Code
frame .eltop.frameconfig.framessicode -width 0.5i -height 0.5i -relief ridge -bd 2
label .eltop.frameconfig.framessicode.label -text "Select SSI Code" -justify left
radiobutton .eltop.frameconfig.framessicode.rbb -text "Binary" -variable ssicode -value 0 -justify left 
radiobutton .eltop.frameconfig.framessicode.rbg -text "Grey" -variable ssicode -value 1 -justify left 

#Select Baud-Rate
frame .eltop.frameconfig.framebaud -width 0.5i -height 0.5i -relief ridge -bd 2
label .eltop.frameconfig.framebaud.label -text "Select Baud-Rate" -justify left
radiobutton .eltop.frameconfig.framebaud.baud1 -text "1250k" -variable baud -value 0 -justify left 
radiobutton .eltop.frameconfig.framebaud.baud2 -text "1000k" -variable baud -value 1 -justify left 
radiobutton .eltop.frameconfig.framebaud.baud3 -text "500k" -variable baud -value 2 -justify left 
radiobutton .eltop.frameconfig.framebaud.baud4 -text "250k" -variable baud -value 3 -justify left 
radiobutton .eltop.frameconfig.framebaud.baud5 -text "125k" -variable baud -value 4 -justify left 


#Select Frame Type
frame .eltop.frameconfig.frametype -width 0.5i -height 0.5i -relief ridge -bd 2
label .eltop.frameconfig.frametype.label -text "Select Frame-Type" -justify left
radiobutton .eltop.frameconfig.frametype.rbmult -text "Multiturn" -variable frame -value 0 -justify left 
radiobutton .eltop.frameconfig.frametype.rbsing -text "Singleturn" -variable frame -value 1 -justify left 
radiobutton .eltop.frameconfig.frametype.rbvar -text "Variable" -variable frame -value 2 -justify left 


#Set Frame Size for variable Type
label .eltop.frameconfig.labelframesize -text "Frame Size" -justify left
entry .eltop.frameconfig.entryframesize -textvariable framesize -justify left


#Set Data Length
label .eltop.frameconfig.labeldatalength -text "Data Length" -justify left
entry .eltop.frameconfig.entrydatalength -textvariable datalength -justify left


#Set Inhibit Time
label .eltop.frameconfig.labelinhibittime -text "Inhibit Time" -justify left
entry .eltop.frameconfig.entryinhibittime -textvariable inhibittime -justify left




grid .eltop.ellabel -row 0 -column 0 -sticky "ew"
grid .eltop.showslave -row 0 -column 1 -sticky "ew"
grid .eltop.list -row 1 -column 0 -rowspan 8 -columnspan 2 -sticky "nsew"
grid .eltop.v -row 1 -column 2 -rowspan 8 -sticky "ns"
grid .eltop.h -row 9 -column 0 -columnspan 2 -sticky "ew"
grid .eltop.masteridlabel -row 1 -column 3 
grid .eltop.masteridentry -row 1 -column 4 -sticky "ew" 
grid .eltop.domainidlabel -row 2 -column 3 
grid .eltop.domainidentry -row 2 -column 4 -sticky "ew" 
grid .eltop.slavealiaslabel -row 3 -column 3 
grid .eltop.slavealiasentry -row 3 -column 4 -sticky "ew" 
grid .eltop.slavepositionlabel -row 4 -column 3 
grid .eltop.slavepositionentry -row 4 -column 4 -sticky "ew" 
grid .eltop.showstatus -row 5 -column 4 -columnspan 2 -sticky "w"
grid .eltop.setok -row 7 -column 3 
grid .eltop.setcancel -row 7 -column 4
grid columnconfigure .eltop 0 -weight 1
grid rowconfigure .eltop 0 -weight 1
grid .eltop.frameconfig -row 10 -column 0 -columnspan 5 -sticky "w"
grid .eltop.frameconfig.labelframeerror -row 0 -column 1 -sticky "w"
grid .eltop.frameconfig.checkframeerror -row 0 -column 0 -sticky "e"
grid .eltop.frameconfig.labelinhibit -row 1 -column 1 -sticky "w"
grid .eltop.frameconfig.checkinhibit -row 1 -column 0 -sticky "e"
grid .eltop.frameconfig.labelinhibittime -row 2 -column 1 -sticky "w"
grid .eltop.frameconfig.entryinhibittime -row 2 -column 0 -sticky "ew"
grid .eltop.frameconfig.labelframesize -row 3 -column 1 -sticky "w"
grid .eltop.frameconfig.entryframesize -row 3 -column 0 -sticky "ew"
grid .eltop.frameconfig.labeldatalength -row 4 -column 1 -sticky "w"
grid .eltop.frameconfig.entrydatalength -row 4 -column 0 -sticky "ew"
grid .eltop.frameconfig.dlabeldummy -row 0 -column 2 -sticky "ewns"
grid .eltop.frameconfig.framessicode -row 0 -column 3 -rowspan 2 -sticky "ew"
grid .eltop.frameconfig.framessicode.label -row 0 -column 0  -sticky "ew"
grid .eltop.frameconfig.framessicode.rbb -row 0 -column 1 -sticky "w"
grid .eltop.frameconfig.framessicode.rbg -row 1 -column 1 -sticky "w"
grid .eltop.frameconfig.framebaud -row 2 -column 3 -rowspan 5 -sticky "ew"
grid .eltop.frameconfig.framebaud.label -row 0 -column 0 -sticky "ew"
grid .eltop.frameconfig.framebaud.baud1 -row 0 -column 1 -sticky "w"
grid .eltop.frameconfig.framebaud.baud2 -row 1 -column 1 -sticky "w"
grid .eltop.frameconfig.framebaud.baud3 -row 2 -column 1 -sticky "w"
grid .eltop.frameconfig.framebaud.baud4 -row 3 -column 1 -sticky "w"
grid .eltop.frameconfig.framebaud.baud5 -row 4 -column 1 -sticky "w"
grid .eltop.frameconfig.frametype -row 8 -column 3 -rowspan 3 -sticky "ew"
grid .eltop.frameconfig.frametype.label -row 0 -column 0 -sticky "ew"
grid .eltop.frameconfig.frametype.rbmult -row 0 -column 1 -sticky "w"
grid .eltop.frameconfig.frametype.rbsing -row 1 -column 1 -sticky "w"
grid .eltop.frameconfig.frametype.rbvar -row 2 -column 1 -sticky "w"
bind .eltop.list <ButtonRelease-1> "check_stateport "
raise .eltop

#.eltop.list insert end $slavestring(0)
#.eltop.list insert end $slavestring(1)

