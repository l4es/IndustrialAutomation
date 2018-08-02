VAR_GLOBAL
   pos    : INT
   delay1 : INT
   delay2 : INT
END_VAR

MAP_IS0
   doExit  : BOOL : /GOButton/out/1
   sysExit : BOOL : /system/exit/0
END_MAP

   LD FALSE
   ST sysExit
   ST doExit

   IS0C
   JMPCN L_exit

   LD 0
   ST pos
   LD 15
   ST delay1
   LD 70
   ST delay2
   LD FALSE
   ST doExit
start:
   LD delay2
   STP

   LD sysExit
   JMPC L_close

   LD doExit
   JMPCN start

   LD pos
   ADD 1
   ST pos
   EQ 1
   JMPC step1
   LD pos
   EQ 2
   JMPC step2
   LD pos
   EQ 3
   JMPC step3
   LD pos
   EQ 4
   JMPC step4
   LD pos
   EQ 5
   JMPC step5
   LD pos
   EQ 6
   JMPC step6
   LD pos
   EQ 7
   JMPC step7
   LD pos
   EQ 8
   JMPC step8
   LD pos
   EQ 9
   JMPC step9
   LD pos
   EQ 10
   JMPC step10

step1:
   LD TRUE
   STIS0 "/Line_1/in/0"
   LD delay1
   STP
   LD FALSE
   STIS0 "/Line_10/in/0"
   JMP start
step2:
   LD TRUE
   STIS0 "/Line_2/in/0"
   LD delay1
   STP
   LD FALSE
   STIS0 "/Line_1/in/0"
   JMP start
step3:
   LD TRUE
   STIS0 "/Line_3/in/0"
   LD delay1
   STP
   LD FALSE
   STIS0 "/Line_2/in/0"
   JMP start
step4:
   LD TRUE
   STIS0 "/Line_4/in/0"
   LD delay1
   STP
   LD FALSE
   STIS0 "/Line_3/in/0"
   JMP start
step5:
   LD TRUE
   STIS0 "/Line_5/in/0"
   LD delay1
   STP
   LD FALSE
   STIS0 "/Line_4/in/0"
   JMP start
step6:
   LD TRUE
   STIS0 "/Line_6/in/0"
   LD delay1
   STP
   LD FALSE
   STIS0 "/Line_5/in/0"
   JMP start
step7:
   LD TRUE
   STIS0 "/Line_7/in/0"
   LD delay1
   STP
   LD FALSE
   STIS0 "/Line_6/in/0"
   JMP start
step8:
   LD TRUE
   STIS0 "/Line_8/in/0"
   LD delay1
   STP
   LD FALSE
   STIS0 "/Line_7/in/0"
   JMP start
step9:
   LD TRUE
   STIS0 "/Line_9/in/0"
   LD delay1
   STP
   LD FALSE
   STIS0 "/Line_8/in/0"
   JMP start
step10:
   LD TRUE
   STIS0 "/Line_10/in/0"
   LD delay1
   STP
   LD FALSE
   STIS0 "/Line_9/in/0"
   LD 0
   ST pos
   JMP start

L_close:
   IS0D

L_exit:
   EXIT