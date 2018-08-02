//-< SYMTAB.H >----------------------------------------------------*--------*
// GigaBASE                  Version 1.0         (c) 1999  GARRET    *     ?  *
// (Post Relational Database Management System)                      *   /\|  *
//                                                                   *  /  \  *
//                          Created:     20-Nov-98    K.A. Knizhnik  * / [] \ *
//                          Last update: 10-Dec-98    K.A. Knizhnik  * GARRET *
//-------------------------------------------------------------------*--------*
// Symbol table interface
//-------------------------------------------------------------------*--------*

#ifndef __SYMTAB_H__
#define __SYMTAB_H__

//#include "IndentedTrace.h" //APA added

BEGIN_GIGABASE_NAMESPACE

#ifndef CLONE_IDENTIFIERS
#define GB_CLONE_ANY_IDENTIFIER false
#else
#define GB_CLONE_ANY_IDENTIFIER true
#endif

class GIGABASE_DLL_ENTRY dbSymbolTable {
    struct HashTableItem {
        HashTableItem* next;
        char_t*        str;
        unsigned       hash;
        byte           tag;
        byte           allocated;
        
//		HashTableItem(){ //APA added
//
//			IT_IT("HashTableItem::HashTableItem");
//		}

        ~HashTableItem() { 

			//IT_IT("HashTableItem::~HashTableItem");

            if (allocated) { 
                delete[] str;
            }
        }
    };
    static HashTableItem* hashTable[];

  public:
//	dbSymbolTable(){   //APA added
//
//			IT_IT("dbSymbolTable::dbSymbolTable");
//		}

    ~dbSymbolTable();
    static dbSymbolTable instance;

    static int add(char_t* &str, int tag, bool allocate = true);

    static void cleanup();
};

END_GIGABASE_NAMESPACE

#endif

