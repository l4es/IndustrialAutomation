
int ModbusRequestToRespond( unsigned char * Question, int LgtQuestion, unsigned char * Response );
int GetMobdusSlaveNbrVars( unsigned char FunctCode );
void SetVarFromModbusSlave( unsigned char FunctCode, int ModbusNum, int Value );
int GetVarForModbusSlave( unsigned char FunctCode, int ModbusNum );

