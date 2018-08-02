#define A  A
#define B B
#define BB BB

#if defined A
   #error defined A
   #if defined B
       #error defined B
       #if defined BB
          #error defined BB
          #if defined BBB
             #error defined BBB
          #elif  defined BBC
             #error defined BBC
          #else
             #error Neither BBB or BBC defined
          #endif
       #elif defined BC
          #error defined BC 
       #else
          #error Neither BB or BC defined
       #endif
   #elif defined C
        #error defined C
   #elif  defined D
        #error defined D
   #endif       
   #error defined A
#elif  defined AB
   #error defined AB
#elif  defined AC
  #error defined AC
#elif  defined AD
   #error defined AD
#else
   #error Neither A, AB, AC, or AD, defined 
#endif          
#error This line must cause an error

#undef A
#undef B
#undef BB
#undef BBB
#undef BBC
#undef BC
#undef C
#undef D
#undef AB
#undef AC
#undef AD


#if defined A
   #error defined A
   #if defined B
       #error defined B
       #if defined BB
          #error defined BB
          #if defined BBB
             #error defined BBB
          #elif  defined BBC
             #error defined BBC
          #else
             #error Neither BBB or BBC defined
          #endif
       #elif defined BC
          #error defined BC 
       #else
          #error Neither BB or BC defined
       #endif
   #elif defined C
        #error defined C
   #elif  defined D
        #error defined D
   #endif       
   #error defined A
#elif  defined AB
   #error defined AB
#elif  defined AC
  #error defined AC
#elif  defined AD
   #error defined AD
#else
   #error Neither A, AB, AC, or AD, defined 
#endif          
#error This line must cause an error


