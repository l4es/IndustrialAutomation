//-< REPLICATPOR.H >----------------------------------------------------*--------*
// GigaBASE                  Version 1.0         (c) 1999  GARRET    *     ?  *
// (Post Relational Database Management System)                      *   /\|  *
//                                                                   *  /  \  *
//                          Created:     13-Mar-2002  K.A. Knizhnik  * / [] \ *
//                          Last update: 13-Mar-2002   K.A. Knizhnik  * GARRET *
//-------------------------------------------------------------------*--------*
// Replicated database 
//-------------------------------------------------------------------*--------*

#ifndef __REPLICATOR_H__
#define __REPLICATOR_H__

#include "database.h"
#include "sockio.h"

BEGIN_GIGABASE_NAMESPACE

/**
 *  
 * This abstract class should be implemented by application to receive 
 * notification about replication process and to control replication process.
 */
class dbReplicationManager { 
   public:
    /**
     * Connnection with specified host is broken
     * If this method returns TRUE then replication is continued for the rest of slaves,
     * otherwise replication is interrupted.
     */
    virtual bool connectionBroken(char* hostName) = 0;
    
    /**
     * This method is called only for replication slave when the whole transaction 
     * is completely transfered from the master.
     */
    virtual void transactionCommitted() = 0;

    /**
     * This method is called only for replication slave when replication is terminated by master
     */ 
    virtual void replicationEnd() = 0;

    /**
     * If this method returns true, then GigaBASE will preserve consistency of
     * slave replica of the database. So in case of master or slave crash, it will be
     * possible to recover and continue work with slave database. 
     * If this method returns false, the slave performance is greatly increased
     * (because of avoiding flushing file buffers to the disk), but in case of 
     * fault, slave database may be stayed in inconsistent state.
     */
    virtual bool preserveSlaveConsistency() = 0;
};

/**
 * Replicated version of GigaBASE
 */
class GIGABASE_DLL_ENTRY dbReplicatedDatabase : public dbDatabase { 
  public:    
    /**
     * Stop replication to slave.
     * Method is anvioked at moster node.
     */
    void stopMasterReplication();
    
    /**
     * Constructor of the replicated database
     * @param mng replication manager
     * @param poolSize number of pages in page pool, if <code>0</code> GigaBASE will choose pool size itself depending 
     * on size of memory in the system
     * DISKLESS_CONFIGURATION option, then in this parameter <B>MAXIMAL</B> size of the 
     * database should be specified (in this mode database can not be reallocated)
     * @param dbExtensionQuantum quentum for extending memory allocation bitmap 
     * @param dbInitIndexSize initial index size (objects)
     * @param nThreads concurrency level for sequential search and sort operations 
     * @see setConcurrency(unsigned nThreads)
     */     
    dbReplicatedDatabase(dbReplicationManager* mng = NULL, 
                  size_t poolSize = 0, // autodetect size of available memory
                  size_t dbExtensionQuantum = dbDefaultExtensionQuantum,
                  size_t dbInitIndexSize = dbDefaultInitIndexSize,
                  int nThreads = 1
                  // Do not specify the last parameter - it is only for checking
                  // that application and GigaBASE library were built with the
                  // same compiler options (-DNO_PTHREADS is critical)
                  // Mismached parameters should cause linker error
#ifdef NO_PTHREADS
                  , bool usePthreads = false
#endif
                  )
        : dbDatabase(dbAllAccess, poolSize, dbExtensionQuantum, dbInitIndexSize, nThreads), 
          replicationManager(mng)
        {}
    
    /**
     * Open database
     * @param masterHostAddress address of the replication master 
     * @param nReplicas number of slaves for master or 0 for slave
     * @param databaseName path to the database file
     * @param transactionCommitDelay delayed commit timeout, by default disabled
     * @param openAttr oppen attributes for database file
     * @return <code>true</code> if database was successfully opened     
     */
    bool open(char const* masterHostAddress,
              int nReplicas, // number of slaves for master or 0 for slave
              char_t const* databaseName, 
              time_t transactionCommitDelay = 0, 
              int openAttr = dbFile::no_buffering);
    /**
     * Close database
     */
    void close();
  protected:  
    /**
     * Method executed at slave to perform sycnhronization with master node
     */
    void slaveReplication();
    
    /**
     * Replicate page to all online slave nodes
     */
    void replicatePage(offs_t pageOffs, void* pageData);
    
    int                       nReplicas;
    socket_t**                replicationSlaves;
    socket_t*                 replicationMaster;
    dbReplicationManager*     replicationManager;
    dbThread                  replicationThread;
 
    static void thread_proc slaveReplicationProc(void* arg) { 
        ((dbReplicatedDatabase*)arg)->slaveReplication();
    }
};

END_GIGABASE_NAMESPACE

#endif

