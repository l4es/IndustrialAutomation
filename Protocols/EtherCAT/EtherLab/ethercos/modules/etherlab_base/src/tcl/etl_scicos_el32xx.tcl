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

proc check_stateport {} { global slaveselected; global selectid; global .eltop; global slavestring; global slavestate; global showstate; global wire_1; global wire_2; global wire_3; global wire_4;  set slaveselected [.eltop.list get [.eltop.list curselection]]; set selectid [.eltop.list curselection]; set selstate  $slavestate([.eltop.list curselection]); if {$selstate == 1} then { .eltop.showstatus configure -state normal } else {.eltop.showstatus configure -state disable; set showstate 0;}; switch -exact $slaveselected { EL3201 { set wire_2 0; set wire_3 0; set wire_4 0; } EL3202 { if {$wire_1 > 2} then {set wire_1 0;}; set wire_3 0; set wire_4 0; } EL3204 { if {$wire_1 > 1} then {set wire_1 0;}; if {$wire_2 > 1} then {set wire_2 0;};   }} }

#array set slavestring {
# 0 EL3201
# 1 EL3202
# 2 EL3204
#}
#array set slavestate {
# 0 1
# 1 1
# 2 1 
#}
#set slaveselected {None selected}
#set masterid 0
#set domainid 0
#set slavealias 0
#set slaveposition 0
#set showstate 0
#set enFilter 0

#set wire_1 0
#set wire_2 0
#set wire_3 0
#set wire_4 0

#set rtdtyp_0 0
#set rtdtyp_1 0
#set rtdtyp_3 0
#set rtdtyp_4 0


set okstate 0
set cancelstate 0
set eltop .eltop
toplevel .eltop
wm title .eltop "Configure Beckhoff EL32xx"
label .eltop.ellabel -text "Selected Slavetype:" -justify center
entry .eltop.showslave -textvariable slaveselected -justify center
scrollbar .eltop.h -orient horizontal -command ".eltop.list xview"
scrollbar .eltop.v -command ".eltop.list yview"
listbox .eltop.list -selectmode single -width 20 -height 10 -setgrid 1 -xscroll ".eltop.h set" -yscroll ".eltop.v set"
checkbutton .eltop.showstatus -text {Enable Statusport} -justify center -variable showstate 
checkbutton .eltop.enablefilter -text {Enable Filter} -justify center -variable enFilter 
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

label .eltop.labelch1 -text "Channel 1" -justify left
frame .eltop.frame1 -width 0.5i -height 0.5i -relief ridge -bd 2
label .eltop.frame1.labelwire_1 -text "Connectiontyp:" -justify left
radiobutton .eltop.frame1.wire_0 -text "not connected" -variable wire_1 -value 0 -justify left 
radiobutton .eltop.frame1.wire_1 -text "2-Wire" -variable wire_1 -value 1 -justify left 
radiobutton .eltop.frame1.wire_2 -text "3-Wire" -variable wire_1 -value 2 -justify left 
radiobutton .eltop.frame1.wire_3 -text "4-Wire" -variable wire_1 -value 3 -justify left 

label .eltop.labelch2 -text "Channel 2" -justify left
frame .eltop.frame2 -width 0.5i -height 0.5i -relief ridge -bd 2
label .eltop.frame2.labelwire_2 -text "Connectiontyp:" -justify left
radiobutton .eltop.frame2.wire_0 -text "not connected" -variable wire_2 -value 0 -justify left 
radiobutton .eltop.frame2.wire_1 -text "2-Wire" -variable wire_2 -value 1 -justify left 
radiobutton .eltop.frame2.wire_2 -text "3-Wire" -variable wire_2 -value 2 -justify left 
label .eltop.frame2.labeldummy_2 -text " " -justify left

label .eltop.labelch3 -text "Channel 3" -justify left
frame .eltop.frame3 -width 0.5i -height 0.5i -relief ridge -bd 2
label .eltop.frame3.labelwire_3 -text "Connectiontyp:" -justify left
radiobutton .eltop.frame3.wire_0 -text "not connected" -variable wire_3 -value 0 -justify left 
radiobutton .eltop.frame3.wire_1 -text "2-Wire" -variable wire_3 -value 1 -justify left 
label .eltop.frame3.labeldummy_31 -text " " -justify left
label .eltop.frame3.labeldummy_32 -text " " -justify left

label .eltop.labelch4 -text "Channel 4" -justify left
frame .eltop.frame4 -width 0.5i -height 0.5i -relief ridge -bd 2
label .eltop.frame4.labelwire_4 -text "Connectiontyp:" -justify left
radiobutton .eltop.frame4.wire_0 -text "not connected" -variable wire_4 -value 0 -justify left 
radiobutton .eltop.frame4.wire_1 -text "2-Wire" -variable wire_4 -value 1 -justify left 
label .eltop.frame4.labeldummy_41 -text " " -justify left
label .eltop.frame4.labeldummy_42 -text " " -justify left
foreach i {0 1 3 4} { frame .eltop.framertd$i -width 0.5i -height 0.5i -relief ridge -bd 2; label .eltop.framertd$i.label -text "RTD-Typ:" -justify left; radiobutton .eltop.framertd$i.rtd1 -text "Pt100" -variable rtdtyp_$i -value 0 -justify left; radiobutton .eltop.framertd$i.rtd2 -text "Ni100" -variable rtdtyp_$i -value 1 -justify left; radiobutton .eltop.framertd$i.rtd3 -text "Pt1000" -variable rtdtyp_$i -value 2 -justify left; radiobutton .eltop.framertd$i.rtd4 -text "Pt500" -variable rtdtyp_$i -value 3 -justify left; radiobutton .eltop.framertd$i.rtd5 -text "Pt200" -variable rtdtyp_$i -value 4 -justify left; radiobutton .eltop.framertd$i.rtd6 -text "Ni1000(-60-160°C)" -variable rtdtyp_$i -value 5 -justify left; radiobutton .eltop.framertd$i.rtd7 -text "Ni1000(-30-160°C)" -variable rtdtyp_$i -value 6 -justify left; radiobutton .eltop.framertd$i.rtd8 -text "Ni120" -variable rtdtyp_$i -value 7 -justify left; radiobutton .eltop.framertd$i.rtd9 -text "Resolution 1/16 Ohm" -variable rtdtyp_$i -value 8 -justify left; radiobutton .eltop.framertd$i.rtd10 -text "Resolution 1/64 Ohm" -variable rtdtyp_$i -value 9 -justify left;}


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
grid .eltop.enablefilter -row 6 -column 4 -columnspan 2 -sticky "w"
grid .eltop.setok -row 7 -column 3 
grid .eltop.setcancel -row 7 -column 4
grid columnconfigure .eltop 0 -weight 1
grid rowconfigure .eltop 0 -weight 1
grid .eltop.labelch1 -row 10 -column 0 -sticky "ew"
grid .eltop.frame1 -row 11 -column 0 -sticky "ew"
grid .eltop.frame1.labelwire_1 -row 0 -column 0 -sticky "ew"
grid .eltop.frame1.wire_0 -row 1 -column 0 -sticky "w"
grid .eltop.frame1.wire_1 -row 2 -column 0 -sticky "w"
grid .eltop.frame1.wire_2 -row 3 -column 0 -sticky "w"
grid .eltop.frame1.wire_3 -row 4 -column 0 -sticky "w"
grid .eltop.labelch2 -row 10 -column 1 -sticky "ew"
grid .eltop.frame2 -row 11 -column 1 -sticky "ew"
grid .eltop.frame2.labelwire_2 -row 0 -column 0 -sticky "ew"
grid .eltop.frame2.wire_0 -row 1 -column 0 -sticky "w"
grid .eltop.frame2.wire_1 -row 2 -column 0 -sticky "w"

grid .eltop.frame3.labelwire_3 -row 0 -column 0 -sticky "ew"
grid .eltop.frame3.wire_0 -row 1 -column 0 -sticky "w"
grid .eltop.frame3.wire_1 -row 2 -column 0 -sticky "w"
grid .eltop.frame2.wire_2 -row 3 -column 0 -sticky "w"
grid .eltop.frame2.labeldummy_2 -row 4 -column 0 -sticky "ew"
grid .eltop.labelch3 -row 10 -column 3 -sticky "ew"
grid .eltop.frame3 -row 11 -column 3 -sticky "ew"
grid .eltop.frame3.labeldummy_31 -row 3 -column 0 -sticky "ew"
grid .eltop.frame3.labeldummy_32 -row 4 -column 0 -sticky "ew"
grid .eltop.labelch4 -row 10 -column 4 -sticky "ew"
grid .eltop.frame4 -row 11 -column 4 -sticky "ew"
grid .eltop.frame4.labelwire_4 -row 0 -column 0 -sticky "ew"
grid .eltop.frame4.wire_0 -row 1 -column 0 -sticky "w"
grid .eltop.frame4.wire_1 -row 2 -column 0 -sticky "w"
grid .eltop.frame4.labeldummy_41 -row 3 -column 0 -sticky "ew"
grid .eltop.frame4.labeldummy_42 -row 4 -column 0 -sticky "ew"
foreach i {0 1 3 4} { grid .eltop.framertd$i -row 12 -column $i -sticky "ew"; grid .eltop.framertd$i.label -row 0 -column 0 -sticky "ew"; foreach j {1 2 3 4 5 6 7 8 9 10} { grid .eltop.framertd$i.rtd$j -row $j -column 0 -sticky "w"; } }
 
bind .eltop.list <ButtonRelease-1> "check_stateport "
raise .eltop

#.eltop.list insert end $slavestring(0)
#.eltop.list insert end $slavestring(1)
#.eltop.list insert end $slavestring(2)
