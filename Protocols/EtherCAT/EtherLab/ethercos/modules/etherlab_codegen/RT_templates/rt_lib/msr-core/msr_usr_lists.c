/* Schnittstelle rt_lib für Userspace Programme */

/*
***************************************************************************************************
*
* Function: msr_init()
*
* Beschreibung: Initialisierung der msr_lib
*
*               
* Parameter: newparamflag: Callback für Parmeteränderung
*                          Rückgabe: 0: erfolgreich -1: nicht erfolgreich 
*                          Parameter für Callback void* : Startadresse, die geändert wurde
*                                                  int   : Länge in byte die geändert wurden
*                          NULL: Funktion wird nicht aufgerufen						  
*               
* Rückgabe: 0: wenn ok, sonst -1
*               
* Status: 
*
***************************************************************************************************
*/


int msr_init(int (*newparamflag)(void*,int)); 

/*
***************************************************************************************************
*
* Function: msr_cleanup()
*
* Beschreibung: Freigeben der msr_lib
*
*               
* Parameter:
*               
* Rückgabe: 0: wenn ok, sonst -1
*               
* Status: 
*
***************************************************************************************************
*/

void msr_cleanup(void);


/* Kommunikation wie Unix IO ----------------------------*/
int msr_open(void);  //Rückgabe ist ein Filedescriptor
int msr_close(int fd);


/* Read ist von Client aus gesehen. Daten gehen von MSR an Client */
ssize_t msr_read(int fd, char *buf, size_t count); //immer non blocking

ssize_t msr_write(int fd, const char *buf, size_t count);


/* Registrierung von Parametern und Signalen*/

/*
***************************************************************************************************
*
* Function: msr_reg_param
*
* Beschreibung: Registrierung einer Variablen als Parameter (generell sind Variablen immer Matrizen - Matlab)
*               Diese Funktion muß für jeden zu registrierenden Parameter aufgerufen werden 
*
*               
* Parameter: path: Ablagepfad
*            name: Name der Variablen
*            cTypeName: String in der der C-Variablenname steht
*            data: Adresse der Variablen
*            rnum: Anzahl Zeilen
*            cnum: Anzahl Spalten
*            dataType: Simulink/Matlab Datentyp
*            orientation: rtwCAPI_MATRIX_COL_MAJOR/rtwCAPI_MATRIX_ROW_MAJOR 
*            dataSize: Größe der Variablen in byte (eines Elementes) 
*               
* Rückgabe: 0: wenn ok, sonst -1
*               
* Status: 
*
***************************************************************************************************
*/

int msr_reg_param( const char *path, const char *name, const char *cTypeName,
		   void *data, 
		   unsigned int rnum, unsigned int cnum, 
		   unsigned int dataType, unsigned int orientation, 
		   unsigned int dataSize);


/* to define -> HA */
/*
***************************************************************************************************
*
* Function: msr_reg_signal
*
* Beschreibung: Registrierung einer Variablen als Kanal/Signal (generell sind Variablen immer Matrizen - Matlab)
*               Diese Funktion muß für jeden zu registrierenden Kanal/Signal aufgerufen werden 
*
*               
* Parameter: path: Ablagepfad
*            name: Name der Variablen
*            cTypeName: String in der der C-Variablenname steht
*            offset: Offset im Datenblock (siehe msr_update)
*            rnum: Anzahl Zeilen
*            cnum: Anzahl Spalten
*            dataType: Simulink/Matlab Datentyp
*            orientation: rtwCAPI_MATRIX_COL_MAJOR/rtwCAPI_MATRIX_ROW_MAJOR 
*            dataSize: Größe der Variablen in byte (eines Elementes) 
*               
* Rückgabe: 0: wenn ok, sonst -1
*               
* Status: 
*
***************************************************************************************************
*/

int msr_reg_signal( const char *path, const char *name, const char *cTypeName,
		    void *offset,                                              // !!!
		    unsigned int rnum, unsigned int cnum, 
		    unsigned int dataType, unsigned int orientation, 
		    unsigned int dataSize);



/* im zyklischen Teil ..... */
/*
***************************************************************************************************
*
* Function: msr_update
*
* Beschreibung: Überträgt die Kanalwerte in die msr_rt_lib-Ringpuffer
*               für jeden Abtastschritt einmal aufrufen               
*
*               
* Parameter: data: Zeiger auf Datenbereich der Kanäle
*               
* Rückgabe: 0: wenn ok, sonst -1
*               
* Status: 
*
***************************************************************************************************
*/

int msr_update(void *data); 


/*
***************************************************************************************************
*
* Function: msr_write_stdout
*
* Beschreibung: stdout vom Echtzeitprozess (Debugging Feature)
*
*               
* Parameter: siehe STDIO
*               
* Rückgabe: 0: wenn ok, sonst -1
*               
* Status: 
*
***************************************************************************************************
*/

ssize_t msr_write_stdout(const char *buf, size_t count);














