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

#ifndef __DISPATCHER_H__
#define __DISPATCHER_H__

#define DISPATCHER_VERSION 102

//BEGIN_GIGABASE_NAMESPACE

class DispatcherServer {
  private:
	char* serverURL;
//	dbEvent TerminationEvent;
    
  public:
    void run(int argc, char* argv[]);

    DispatcherServer();
    virtual~DispatcherServer();
};

//END_GIGABASE_NAMESPACE

#endif
