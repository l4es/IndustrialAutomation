;  Created 1/5/05 by Comperio
;(based, in part, on the script located at http://nsis.sourceforge.net/archive/nsisweb.php?page=643
;
;Usage:
;${VersionCheck} "Version1" "Version2" "outputVar"
;Version1 = 1st version number
;Version2 = 2nd version number
;outputVar = Variable used to store the ouput
;
;Return values:
;0 = both versions are equal
;1 = Version 1 is NEWER than version 2
;2 = Version1 is OLDER than version 2
;Rules for Version Numbers:
;Version numbers must always be a string of numbers only.  (A dot
;in the name is optional).
;
;[Variables]
var P1		; file pointer, used to "remember" the position in the Version1 string
var P2		; file pointer, used to "remember" the position in the Version2 string
var V1		;version number from Version1
var V2		;version number from Version2
Var Reslt	; holds the return flag


;[Macros]
!macro VersionCheck Ver1 Ver2 OutVar
;To make this work, one character must be added to the version string:
Push "x${Ver2}"
Push "x${Ver1}"
Call VersionCheckF
Pop ${OutVar}

!macroend

;[Defines]
!define VersionCheck "!insertmacro VersionCheck"

;[Functions]
Function VersionCheckF
Exch $1 ; $1 contains Version 1
Exch
Exch $2 ; $2 contains Version 2
Exch
Push $R0
;Set the file pointers:
IntOp $P1 $P1 + 1
IntOp $P2 $P2 + 1
;  ******************* Get 1st version number for Ver1 **********************
V11:
;I use $1 and $2 to help keep identify "Ver1" vs. "Ver2"
StrCpy $R0 $1 1 $P1 ;$R0 contains the character at position $P1
IntOp $P1 $P1 + 1 ;increments the file pointer for the next read
StrCmp $R0 "" V11end 0	;check for empty string
strCmp $R0 "." v11end 0
strCpy $V1 "$V1$R0"
Goto V11
V11End:
StrCmp $V1 "" 0 +2
StrCpy $V1 "0"
;  ******************* Get 1st version number for Ver2 **********************
V12:
StrCpy $R0 $2 1 $P2 ;$R0 contains the character at position $P1
IntOp $P2 $P2 + 1 ;increments the file pointer for the next read
StrCmp $R0 "" V12end 0	;check for empty string
strCmp $R0 "." v12end 0
strCpy $V2 "$V2$R0"
Goto V12
V12End:
StrCmp $V2 "" 0 +2
StrCpy $V2 "0"
;At this point, we can compare the results.  If the numbers are not
;equal, then we can exit
IntCmp $V1 $V2 cont1 older1 newer1
older1: ; Version 1 is older (less than) than version 2
StrCpy $Reslt 2
Goto ExitFunction
newer1:	; Version 1 is newer (greater than) Version 2
StrCpy $Reslt 1
Goto ExitFunction
Cont1: ;Versions are the same.  Continue searching for differences
;Reset $V1 and $V2
StrCpy $V1 ""
StrCpy $V2 ""

;  ******************* Get 2nd version number for Ver1 **********************
V21:
StrCpy $R0 $1 1 $P1 ;$R0 contains the character at position $P1
IntOp $P1 $P1 + 1 ;increments the file pointer for the next read
StrCmp $R0 "" V21end 0	;check for empty string
strCmp $R0 "." v21end 0
strCpy $V1 "$V1$R0"
Goto V21
V21End:
StrCmp $V1 "" 0 +2
StrCpy $V1 "0"
;  ******************* Get 2nd version number for Ver2 **********************
V22:
StrCpy $R0 $2 1 $P2 ;$R0 contains the character at position $P1
IntOp $P2 $P2 + 1 ;increments the file pointer for the next read
StrCmp $R0 "" V22end 0	;check for empty string
strCmp $R0 "." V22end 0
strCpy $V2 "$V2$R0"
Goto V22
V22End:
StrCmp $V2 "" 0 +2
StrCpy $V2 "0"
;At this point, we can compare the results.  If the numbers are not
;equal, then we can exit
IntCmp $V1 $V2 cont2 older2 newer2
older2: ; Version 1 is older (less than) than version 2
StrCpy $Reslt 2
Goto ExitFunction
newer2:	; Version 1 is newer (greater than) Version 2
StrCpy $Reslt 1
Goto ExitFunction
Cont2: ;Versions are the same.  Continue searching for differences
;Reset $V1 and $V2
StrCpy $V1 ""
StrCpy $V2 ""
;  ******************* Get 3rd version number for Ver1 **********************
V31:
StrCpy $R0 $1 1 $P1 ;$R0 contains the character at position $P1
IntOp $P1 $P1 + 1 ;increments the file pointer for the next read
StrCmp $R0 "" V31end 0	;check for empty string
strCmp $R0 "." v31end 0
strCpy $V1 "$V1$R0"
Goto V31
V31End:
StrCmp $V1 "" 0 +2
StrCpy $V1 "0"
;  ******************* Get 3rd version number for Ver2 **********************
V32:
StrCpy $R0 $2 1 $P2 ;$R0 contains the character at position $P1
IntOp $P2 $P2 + 1 ;increments the file pointer for the next read
StrCmp $R0 "" V32end 0	;check for empty string
strCmp $R0 "." V32end 0
strCpy $V2 "$V2$R0"
Goto V32
V32End:
StrCmp $V2 "" 0 +2
StrCpy $V2 "0"
;At this point, we can compare the results.  If the numbers are not
;equal, then we can exit
IntCmp $V1 $V2 cont3 older3 newer3
older3: ; Version 1 is older (less than) than version 2
StrCpy $Reslt 2
Goto ExitFunction
newer3:	; Version 1 is newer (greater than) Version 2
StrCpy $Reslt 1
Goto ExitFunction
Cont3: ;Versions are the same.  Continue searching for differences
;Reset $V1 and $V2
StrCpy $V1 ""
StrCpy $V2 ""
;  ******************* Get 4th version number for Ver1 **********************
V41:
StrCpy $R0 $1 1 $P1 ;$R0 contains the character at position $P1
IntOp $P1 $P1 + 1 ;increments the file pointer for the next read
StrCmp $R0 "" V41end 0	;check for empty string
strCmp $R0 "." v41end 0
strCpy $V1 "$V1$R0"
Goto V41
V41End:
StrCmp $V1 "" 0 +2
StrCpy $V1 "0"
;  ******************* Get 4th version number for Ver2 **********************
V42:
StrCpy $R0 $2 1 $P2 ;$R0 contains the character at position $P1
IntOp $P2 $P2 + 1 ;increments the file pointer for the next read
StrCmp $R0 "" V42end 0	;check for empty string
strCmp $R0 "." V42end 0
strCpy $V2 "$V2$R0"
Goto V42
V42End:
StrCmp $V2 "" 0 +2
StrCpy $V2 "0"
;At this point, we can compare the results.  If the numbers are not
;equal, then we can exit
IntCmp $V1 $V2 cont4 older4 newer4
older4: ; Version 1 is older (less than) than version 2
StrCpy $Reslt 2
Goto ExitFunction
newer4:	; Version 1 is newer (greater than) Version 2
StrCpy $Reslt 1
Goto ExitFunction
Cont4:
;Versions are the same.  We've reached the end of the version
;strings, so set the function to 0 (equal) and exit
StrCpy $Reslt 0
ExitFunction:
Pop $R0
Pop $1
Pop $2
Push $Reslt
FunctionEnd
