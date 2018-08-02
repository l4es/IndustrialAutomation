/*
 *                         IndigoSCADA
 *
 *   This software and documentation are Copyright 2002 to 2009 Enscada 
 *   Limited and its licensees. All rights reserved. See file:
 *
 *                     $HOME/LICENSE 
 *
 *   for full copyright notice and license terms. 
 *
 */
#ifndef __SQLSERVER_H__
#define __SQLSERVER_H__

//BEGIN_FASTDB_NAMESPACE

class RealTimeSqlServer{
  private:
	char* serverURL;
    
  public:
    void run(int argc, char* argv[]);
//	dbEvent TerminationEvent;
	
    RealTimeSqlServer();
    virtual~RealTimeSqlServer();
};

//END_FASTDB_NAMESPACE

#endif
