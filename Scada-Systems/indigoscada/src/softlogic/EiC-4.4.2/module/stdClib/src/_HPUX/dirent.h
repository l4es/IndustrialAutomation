#ifndef _SYS_DIRENTH
#define _SYS_DIRENTH

 struct dirent {
      ino_t d_ino;
      short d_reclen;
      short d_namlen;
      char  d_name[255  + 1];
   };

#endif /* _SYS_DIRENTH */






