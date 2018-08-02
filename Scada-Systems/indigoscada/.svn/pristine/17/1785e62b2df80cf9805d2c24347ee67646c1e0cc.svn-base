//-< REPLICATOR.CPP >--------------------------------------------------*--------*
// GigaBASE                  Version 1.0         (c) 1999  GARRET    *     ?  *
// (Post Relational Database Management System)                      *   /\|  *
//                                                                   *  /  \  *
//                          Created:     13-Mar-2002  K.A. Knizhnik  * / [] \ *
//                          Last update: 13-Mar-2002  K.A. Knizhnik  * GARRET *
//-------------------------------------------------------------------*--------*
// Replicated database memory management
//-------------------------------------------------------------------*--------*

#define INSIDE_GIGABASE

#include "gigabase.h"
#include "replicator.h"
#include "compiler.h"

BEGIN_GIGABASE_NAMESPACE

bool dbReplicatedDatabase::open(char const* masterHostAddress,
                                int nReplicas, // number of slaves for master or 0 for slave
                                char_t const* databaseName, 
                                time_t transactionCommitDelay, 
                                int openAttr)
{    
    replicationMaster = NULL;
    replicationSlaves = NULL;
    
    bool succeed = dbDatabase::open(databaseName, transactionCommitDelay, openAttr);
    
    if (succeed) {
        if (nReplicas == 0) { // slave
            socket_t* masterSocket = socket_t::connect(masterHostAddress);
            if (masterSocket->is_ok()) { 
                curr = header->curr ^ 1;
                replicationMaster = masterSocket;
                replicationSlaves = NULL;
                replicationThread.create((dbThread::thread_proc_t)slaveReplicationProc, this);
                return true;
            } else { 
                delete masterSocket;
                dbDatabase::close();
                return false;
            }
        } else { // master
            socket_t** slaveSockets = new socket_t*[nReplicas];
            socket_t* masterSocket = socket_t::create_global(masterHostAddress);    
            for (int i = 0; i < nReplicas; i++) {
                socket_t* slave = masterSocket->accept();
                if (slave == NULL) { 
                    while (--i >= 0) {
                        delete slaveSockets[i];
                    }
                    delete[] slaveSockets;
                    delete masterSocket;
                    dbDatabase::close();
                    return false;
                }
                slaveSockets[i] = slave;
            }
            delete masterSocket;
            replicationSlaves = slaveSockets;
            replicationMaster = NULL;
            this->nReplicas = nReplicas;
            return true;
        }
    }
    return succeed;
}

void dbReplicatedDatabase::close()
{
    dbDatabase::close();
    stopMasterReplication();
}

void dbReplicatedDatabase::replicatePage(offs_t pageOffs, void* pageData)
{
    if (replicationSlaves != NULL) { 
        for (int i = nReplicas; --i >= 0;) { 
            if (!replicationSlaves[i]->write(&pageOffs, sizeof pageOffs)
                || !replicationSlaves[i]->write(pageData, dbPageSize))
            {
                if (replicationManager != NULL) { 
                    char* peerName = replicationSlaves[i]->get_peer_name();
                    bool continueReplication = replicationManager->connectionBroken(peerName);
                    delete[] peerName;
                    if (continueReplication) { 
                        delete replicationSlaves[i];
                        for (int j = nReplicas; --j > i; replicationSlaves[j-1] = replicationSlaves[j]);
                        nReplicas -= 1;
                    } else { 
                        break;
                    }
                }
            }
        }
    }
}

#define END_REPLICATION 1

void dbReplicatedDatabase::slaveReplication()
{
    offs_t pageOffs;
    bool preserveSlaveConsistency = true;
    if (replicationManager != NULL) { 
        preserveSlaveConsistency = replicationManager->preserveSlaveConsistency();
    }
    attach();
    dbDatabaseThreadContext* ctx = threadContext.get();

    while (replicationMaster->read(&pageOffs, sizeof pageOffs)) { 
        if (pageOffs == 0) { 
            if (!replicationMaster->read(header, dbPageSize)) { 
                break;
            }
            pool.flush();
            if (preserveSlaveConsistency) { 
                file->flush();
            }
            file->write(0, header, dbPageSize);
            if (preserveSlaveConsistency) {         
                file->flush();
            }
            beginTransaction(dbExclusiveLock);
            curr = header->curr ^ 1;
            for (dbTableDescriptor* desc = tables; desc != NULL; desc = desc->nextDbTable)
            {
                dbTable* table = (dbTable*)get(desc->tableId);
                desc->firstRow = table->firstRow;
                desc->lastRow = table->lastRow;
                desc->nRows = table->nRows;
                pool.unfix(table);
            }
            endTransaction(ctx);
            if (replicationManager) { 
                replicationManager->transactionCommitted();
            }
        } else if (pageOffs == END_REPLICATION) {     
            curr = header->curr;
            delete replicationMaster;
            replicationMaster = NULL;
            if (replicationManager) { 
                replicationManager->replicationEnd();
            }
            return;
        } else { 
            byte* pg = pool.put(pageOffs);
            if (!replicationMaster->read(pg, dbPageSize)) { 
                break;
            }
            pool.unfix(pg);
        }
    }
    if (replicationManager) { 
        replicationManager->connectionBroken(replicationMaster->get_peer_name());
    }
    delete ctx;
}

void dbReplicatedDatabase::stopMasterReplication()
{
    if (replicationSlaves) { 
        offs_t cmd = END_REPLICATION;
        for (int i = nReplicas; --i >= 0;) { 
            replicationSlaves[i]->write(&cmd, sizeof cmd);
        }
        delete[] replicationSlaves;
        replicationSlaves = NULL;
    }
}

END_GIGABASE_NAMESPACE
