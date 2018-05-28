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

proc check_stateport {} { global slaveselected; global selectid; global .eltop; global slavestring; global slavestate; global showstate; set slaveselected [.eltop.list get [.eltop.list curselection]]; set selectid [.eltop.list curselection]; set selstate  $slavestate([.eltop.list curselection]); if {$selstate == 1} then { .eltop.showstatus configure -state normal } else {.eltop.showstatus configure -state disable; set showstate 0;}}

#array set slavestring {
# 0 EL2004V1
# 1 EL2004V2
# 2 EL2008
# 3 EL2032V1
# 4 EL2032V2
#}
#array set slavestate {
# 0 0
# 1 0
# 2 0
# 3 1
# 4 1
#}
#set slaveselected {None selected}
#set masterid 0
#set domainid 0
#set slavealias 0
#set slaveposition {}
#set showstate 0
set okstate 0
set cancelstate 0
set eltop .eltop
toplevel .eltop
#wm resizeable .eltop 0 0
wm title .eltop "Configure Beckhoff EL2xxx"
#wm iconname .eltop "EL2xxx"
label .eltop.ellabel -text "Selected Slavetype:" -justify center
entry .eltop.showslave -textvariable slaveselected -justify center
scrollbar .eltop.h -orient horizontal -command ".eltop.list xview"
scrollbar .eltop.v -command ".eltop.list yview"
listbox .eltop.list -selectmode single -width 20 -height 10 -setgrid 1 -xscroll ".eltop.h set" -yscroll ".eltop.v set"
checkbutton .eltop.showstatus -text {Enable Statusport} -justify center -variable showstate -state disable
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
grid .eltop.showstatus -row 5 -column 3 -columnspan 2 -sticky "ew"
grid .eltop.setok -row 6 -column 3 
grid .eltop.setcancel -row 6 -column 4
grid columnconfigure .eltop 0 -weight 1
grid rowconfigure .eltop 0 -weight 1
bind .eltop.list <ButtonRelease-1> "check_stateport "
raise .eltop
#.eltop.list insert end $slavestring(0)
#.eltop.list insert end $slavestring(1)
#.eltop.list insert end $slavestring(2)
#.eltop.list insert end $slavestring(3)
#.eltop.list insert end $slavestring(4)


