; GetIEVersion
;
; Based on Yazno's function, http://yazno.tripod.com/powerpimpit/
; Returns on top of stack
; 1-6 (Installed IE Version)
; or
; '' (IE is not installed)
;
; Usage:
;   Call GetIEVersion
;   Pop $R0
;   ; at this point $R0 is "5" or whatnot

Function GetIEVersion
  Push $R0
  ClearErrors
  ReadRegStr $R0 HKLM "Software\Microsoft\Internet Explorer" "Version"
  IfErrors lbl_123 lbl_456

  lbl_456: ; ie 4+
    Strcpy $R0 $R0 1
  Goto lbl_done

  lbl_123: ; older ie version
    ClearErrors
    ReadRegStr $R0 HKLM "Software\Microsoft\Internet Explorer" "IVer"
    IfErrors lbl_error

      StrCpy $R0 $R0 3
        StrCmp $R0 '100' lbl_ie1
        StrCmp $R0 '101' lbl_ie2
        StrCmp $R0 '102' lbl_ie2
        StrCpy $R0 '3' ; default to ie3 if not 100, 101, or 102.
        Goto lbl_done
          lbl_ie1:
            StrCpy $R0 '1'
          Goto lbl_done
          lbl_ie2:
            StrCpy $R0 '2'
          Goto lbl_done
       lbl_error:
         StrCpy $R0 ''
   lbl_done:
   Exch $R0
FunctionEnd
