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

#include <stdio.h>
#include "shmem.h"

#ifndef ___FIFO___H
#define ___FIFO___H

#pragma warning (disable: 4291)

//Comment the following line if you need a blocking FIFO when it is full
#define USE_NOT_BLOCKING_FIFO_WHEN_FULL

class fifo_obj { 
  protected: 
    class item { 
		public:
		#ifdef USE_BASED_POINTERS
		REF(item) next;
		#else
		item* next;
		#endif
		int   length;
		char  buffer[1]; 

		void* operator new(size_t fixed_size,shared_memory& shmem, size_t varying_size) 
		{
			return shmem.allocate(fixed_size + varying_size - 4);
		}

		void __delete(void* p, shared_memory& shmem) 
		{
			shmem.deallocate(p);
		}

		void store(char* message, int message_length) 
		{
			length = message_length;
			next = NULL;
			memcpy(buffer, message, length);
		}
    };

    class header { 
		private:
		#ifdef USE_BASED_POINTERS
		REF(item) tail;
		REF(item) head;
		#else
		item* tail;
		item* head;
		#endif

		public:
		void* operator new(size_t size, shared_memory& shmem) 
		{
			return shmem.allocate(size);
		}

		void __delete(void* p, shared_memory& shmem) 
		{
			shmem.deallocate(p);
		}

		header() 
		{
			tail = NULL;
			head = NULL;
		}

		bool enqueue(shared_memory& shmem, event& not_full,char* message, int message_length)
		{
			exclusive_lock xlock(shmem);
			// create object of varying size	
			item* ip = new (shmem, message_length) item;

			if (ip == NULL) 
			{ 
				not_full.reset();
				return false;  // not enough space in storage
    		}

			ip->store(message, message_length);

			if (head == NULL) 
			{ 
				head = tail = (REF(item))ip;
			} 
			else 
			{ 
				tail = tail->next = (REF(item))ip;
			}
			return true;
		}

		int dequeue(shared_memory& shmem, char* buf, int buf_size) 
		{ 
			exclusive_lock xlock(shmem);
			item* ip = head;
			if (ip == NULL) { // queue not empty
			return -1;
			} 
			head = head->next;
			int length = ip->length < buf_size ? ip->length : buf_size;
			memcpy(buf, ip->buffer, length);
			__delete(ip, shmem);
			return length;
		}
    };

    header*       root;
    semaphore     not_empty;
    event         not_full;
    shared_memory shmem;
    char*         name;
    size_t        max_size;

    char *gl_buf;

	public:
    bool open(char const* name, size_t max_size, p_call_exit_handler f_log_arg) 
	{ 
		global_func_log = f_log_arg;

		shared_memory::status rc = shmem.open(NULL, name, max_size);

		if (rc != shared_memory::ok)
		{ 
			fprintf(stderr, "rc not ok");
			fflush(stderr);
			return false;
		}

		root = (header*)shmem.get_root_object();

		if (root == NULL) 
		{ 
			exclusive_lock xlock(shmem);
			root = new (shmem) header;
			shmem.set_root_object(root);
		}
		
		size_t len = strlen(name);
		char* global_name = new char[len+5];
		strcpy(global_name, name);
		strcpy(global_name+len, ".put");

		if (!not_full.open(global_name)) 
		{ 	
			delete[] global_name;
			return false;
		}

		strcpy(global_name+len, ".get");

		if (!not_empty.open(global_name)) 
		{ 
			delete[] global_name;
			return false;
		}

		delete[] global_name;

        gl_buf = NULL;

		return true;
    }

    void close() 
	{ 
		shmem.close();
		not_empty.close();
		not_full.close();
    }
    
    void put(char* message, int length) 
	{ 
		while (!root->enqueue(shmem, not_full, message, length)) 
		{ 
            #ifdef USE_NOT_BLOCKING_FIFO_WHEN_FULL
			//The queue is FULL! So remove from the head
			fprintf(stderr, "The queue is full\n");
			fflush(stderr);
						
			unsigned msec = 1;
			
			if(not_empty.wait(msec)) 
			{
                if(gl_buf == NULL) 
                { 
                    gl_buf = new char[length]; 
                }

				root->dequeue(shmem, gl_buf, length);
				not_full.signal();
			}
            #endif

			not_full.wait();
		}
		not_empty.signal();
    }

    int get(char* buf, int buf_size, unsigned msec) 
	{ 
		if (not_empty.wait(msec)) 
		{
			int len = root->dequeue(shmem, buf, buf_size);
			//assert(len >= 0);
			if(!(len >= 0))
				global_func_log(__LINE__,__FILE__, NULL);
			not_full.signal();
			return len;
		}
		return -1;
    }
};

#endif //___FIFO___H
