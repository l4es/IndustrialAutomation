CLASSIC LADDER PROJECT
Copyright (C) 2001-2015 Marc Le Douarain
marc . le - douarain /AT\ laposte \DOT/ net
http://www.sourceforge.net/projects/classicladder
http://sites.google.com/site/classicladder
February 2015


If someone want to add a new function block (ladder) to ClassicLadder,
it will find here some informations to develop it...
----------------------------------------------------------------------
(written during adding the new Registers)

I will give you the files list in which you should have to add your parts.
Of course, compile often as you can.

First the function block to add must be defined: inputs, outputs, size, ...
Well sometimes not really ok the first time, but better if it can! Generally harder to modify after
(principally if adding/movings pins on your block...)

- In classicladder.h:
  add a new define default numbers value (quantity of new blocks),
  add a new entry in plc_sizeinfo_s for size + a define shortcut for its size,
  add a new define in elements lists for the rungs,
  add new users types of variables defines "VAR_" (booleans / integers),
  and add a new typedef struct containing variables for the new function block.

- In arrays.c:
  add init default size in InitGeneralParamsMirror(),
  add declaration of an variables array,
  add alloc/free of the variables array for the new struct,
  add future InitXxxxxx() function call in ClassicLadder_InitProjectDatas().
 
- In calc.c: (we will come into it later...)
  add new function InitXxxxxx() & PrepareXxxxxx()

- In global.h:
  add "extern" of the new variables array.

(you should compile now with a make clean;make...)

- In vars_access.c:
  complete GetNbrVarsForType(), ReadVar(), WriteVar()
  
- In vars_names_list.c
  add new lines with the new vars VARS_

- In drawing.c:
  in DrawElement() add line for little display in toolbar
  in DrawComplexElement() add a new big case to display it...
  in DrawRung() add test complex block type to draw + add vars to spy for this block if needed

- edit_gtk.c
  adjust 2-D arrays at the start to have access to your new function block in the toolbar.

- edit_copy.c

- In edit.c:
  add a line in RulesForSpecialElements array with x/y sizes of the block
  add cases in LoadElementProperties(), GetElementPropertiesForStatusBar() and SaveElementProperties()
  add NewXxxxxxxUsedInRung[] array and modify SetUsedStateFunctionBlock(), CopyUsedFonctionBlockBeforeEdit() and
  GetFreeNumberFunctionBlock()

- In files.c:
  add new functions LoadXxxxxxParams() & SaveXxxxxxParams(), and use them in LoadAllLadderDatas() and in
  SaveAllLadderDatas()

- In calc.c: (as said before, not all was done, hey the logic of the block has to be coded !)
  add new function CalcTypeXXXXX() and its call in RefreshRung()

- In config_gtk.c:
  Add size config for new block in CreateGeneralParametersPage() & GetGeneralParameters()
  
- A little graphic...
  Create .ico file, adjust Makefile, and also icons_gtk.c

One function block family that can be looked at for example is the "counter" ones...
why not starting with a "grep StrCounter *.h *.c", and "grep CounterArray *.c" !? ;-)
and also later grep ELE_COUNTER *.c




