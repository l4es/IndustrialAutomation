#package require Tk

proc check_stateport {} { global slaveselected; global selectid; global .eltop; global slavestring; global slavestate; global showstate;  set slaveselected [.eltop.list get [.eltop.list curselection]]; set selectid [.eltop.list curselection]; set selstate  $slavestate([.eltop.list curselection]); if {$selstate == 1} then { .eltop.showstatus configure -state normal } else {.eltop.showstatus configure -state disable; set showstate 0;}; }








set okstate 0
set cancelstate 0
set eltop .eltop
toplevel .eltop
wm title .eltop "Configure Beckhoff EL5101"
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

#Registerhandling
frame .eltop.frameel5101 -width 0.5i -height 0.5i -relief ridge -bd 2

#Show Register reset
label .eltop.frameel5101.labelregisterreset -text "Enable Register Reset" -justify left
checkbutton .eltop.frameel5101.checkregisterreset  -justify center -variable enableregisterreset
#Enable Register reload
label .eltop.frameel5101.labelregisterreload -text " Enable Register Reload" -justify left
checkbutton .eltop.frameel5101.checkregisterreload  -justify center -variable enableregisterreload
#Set Reload Value
label .eltop.frameel5101.labelreloadvalue -text "Counter Reload Value" -justify left
entry .eltop.frameel5101.entryreloadvalue -textvariable counterreloadvalue -justify left
#Set Frequency Window
label .eltop.frameel5101.labelfreqwindow -text "Frequency Window" -justify left
entry .eltop.frameel5101.entryfreqwindow -textvariable freqwindow -justify left
#Enable FWD Count
label .eltop.frameel5101.labelfwdcount -text "Enable FWD Counter" -justify left
checkbutton .eltop.frameel5101.checkfwdcount -textvariable enablefwdcount -justify left

#Configure gate Setting
frame .eltop.frameel5101.framegateconfig -width 0.5i -height 0.5i -relief ridge -bd 2
label .eltop.frameel5101.framegateconfig.label -text "Select Gate Type" -justify left
radiobutton .eltop.frameel5101.framegateconfig.rbnone -text "None" -variable gatetyp -value 0 -justify left 
radiobutton .eltop.frameel5101.framegateconfig.rbpos -text "Positive" -variable gatetyp -value 1 -justify left 
radiobutton .eltop.frameel5101.framegateconfig.rbneg -text "Negative" -variable gatetyp -value 2 -justify left 


#Configure Input Ports
frame .eltop.frameel5101.frameinputconfig -width 0.5i -height 0.5i -relief ridge -bd 2
label .eltop.frameel5101.frameinputconfig.label -text "Select Input Ports" -justify left

#Controlport
label .eltop.frameel5101.frameinputconfig.labelcontrolinput -text "Enable Control Input Port" -justify left
checkbutton .eltop.frameel5101.frameinputconfig.checkcontrolinput  -justify center -variable controlinputport

#Preset
label .eltop.frameel5101.frameinputconfig.labelpresetinput -text "Enable PReset Input Port" -justify left
checkbutton .eltop.frameel5101.frameinputconfig.checkpresetinput  -justify center -variable presetinputport

#Configure Output Ports
frame .eltop.frameel5101.frameoutputconfig -width 0.5i -height 0.5i -relief ridge -bd 2
label .eltop.frameel5101.frameoutputconfig.label -text "Select Output Ports" -justify left

#Counter
label .eltop.frameel5101.frameoutputconfig.labelcounter -text "Enable Counter Output Port" -justify left
checkbutton .eltop.frameel5101.frameoutputconfig.checkcounter  -justify center -variable counteroutputport

#Latch
label .eltop.frameel5101.frameoutputconfig.labellatch -text "Enable Latch Output Port" -justify left
checkbutton .eltop.frameel5101.frameoutputconfig.checklatch  -justify center -variable latchoutputport

#Frequency
label .eltop.frameel5101.frameoutputconfig.labelfreq -text "Enable Frequency Output Port" -justify left
checkbutton .eltop.frameel5101.frameoutputconfig.checkfreq  -justify center -variable freqoutputport

#Period
label .eltop.frameel5101.frameoutputconfig.labelperiod -text "Enable Period Output Port" -justify left
checkbutton .eltop.frameel5101.frameoutputconfig.checkperiod  -justify center -variable periodoutputport

#Window
label .eltop.frameel5101.frameoutputconfig.labelwindow -text "Enable Window Output Port" -justify left
checkbutton .eltop.frameel5101.frameoutputconfig.checkperiod  -justify center -variable windowoutputport



.eltop.frameel5101


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
grid .eltop.frameel5101 -row 10 -column 0 -columnspan 5 -sticky "w"
grid .eltop.frameel5101.labelregisterreset -row 0 -column 1 -sticky "w"
grid .eltop.frameel5101.checkregisterreset -row 0 -column 0 -sticky "e"
grid .eltop.frameel5101.labelregisterreload -row 1 -column 1 -sticky "w"
grid .eltop.frameel5101.checkregisterreload -row 1 -column 0 -sticky "e"
grid .eltop.frameel5101.labelreloadvalue -row 2 -column 1 -sticky "w"
grid .eltop.frameel5101.entryreloadvalue -row 2 -column 0 -sticky "e"
grid .eltop.frameel5101.labelfreqwindow -row 3 -column 1 -sticky "w"
grid .eltop.frameel5101.entryfreqwindow -row 3 -column 0 -sticky "e"
grid .eltop.frameel5101.labelfwdcount -row 4 -column 1 -sticky "w"
grid .eltop.frameel5101.checkfwdcount -row 4 -column 0 -sticky "e"
grid .eltop.frameel5101.framegateconfig -row 0 -column 2 -sticky "ew"
grid .eltop.frameel5101.framegateconfig.label -row 0 -column 0 -sticky "ew"
grid .eltop.frameel5101.framegateconfig.rbnone -row 1 -column 0 -sticky "w"
grid .eltop.frameel5101.framegateconfig.rbpos -row 2 -column 0 -sticky "w"
grid .eltop.frameel5101.framegateconfig.rbnone -row 3 -column 0 -sticky "w"
grid .eltop.frameel5101.frameinputconfig -row 5 -column 2 -columnspan 2 -sticky "ew"
grid .eltop.frameel5101.frameinputconfig.label -row 0 -column 0 -columnspan 2 -sticky "ew"
grid .eltop.frameel5101.frameinputconfig.labelcontrolinput -row 1 -column 0 -sticky "w" 
grid .eltop.frameel5101.frameinputconfig.checkcontrolinput -row 1 -column 1 -sticky "w" 
grid .eltop.frameel5101.frameinputconfig.labelpresetinput -row 2 -column 0 -sticky "w" 
grid .eltop.frameel5101.frameinputconfig.checkpresetinput -row 2 -column 1 -sticky "w" 
grid .eltop.frameel5101.frameoutputconfig -row 10 -column 2 -columnspan 2 -sticky "ew"
grid .eltop.frameel5101.frameoutputconfig.label -row 0 -column 0 -columnsspan 2 -sticky "ew"
grid .eltop.frameel5101.frameoutputconfig.labelcounter -row 1 -column 0 -sticky "w"
grid .eltop.frameel5101.frameoutputconfig.checkcounter -row 1 -column 1 -sticky "e"
grid .eltop.frameel5101.frameoutputconfig.labellatch -row 2 -column 0 -sticky "w"
grid .eltop.frameel5101.frameoutputconfig.checklatch -row 2 -column 1 -sticky "e"
 
bind .eltop.list <ButtonRelease-1> "check_stateport "
raise .eltop

#.eltop.list insert end $slavestring(0)
#.eltop.list insert end $slavestring(1)

