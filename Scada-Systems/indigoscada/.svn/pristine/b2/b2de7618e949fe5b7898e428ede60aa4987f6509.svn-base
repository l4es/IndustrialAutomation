//-< TSTSHMEM.CPP >--------------------------------------------------*--------*
// SHMEM                      Version 1.0        (c) 1998  GARRET    *     ?  *
// (Shared Memory Manager)                                           *   /\|  *
//                                                                   *  /  \  *
//                          Created:     28-Mar-98    K.A. Knizhnik  * / [] \ *
//                          Last update:  2-Apr-98    K.A. Knizhnik  * GARRET *
//-------------------------------------------------------------------*--------*
// Test of shared memory
//-------------------------------------------------------------------*--------*

#include "shmem.h"
#include <stdio.h>

#ifndef _WIN32
#include <sys/time.h>
#endif

const int max_size = 8*1024*1024;
const int n_inserts = 100000;
const int n_searches = 10;

shared_memory shmem;

class tree { 
  public:
#ifdef USE_BASED_POINTERS
    REF(tree) left;
    REF(tree) right;
#else
    tree* left;
    tree* right;
#endif
    int   val;

    void* operator new(size_t size) { 
	return shmem.allocate(size);
    }
    void operator delete(void* p) { 
	shmem.free(p);
    }
    tree(int key) { val = key; left = right = NULL; }
};


#ifdef USE_BASED_POINTERS

class root_object { 
  public:
    REF(tree) root;
    void insert(int key) {
	exclusive_lock x_lock(shmem);
	REF(tree) tp;
	REF(tree) *tpp = &root;
	while ((tp = *tpp) != NULL) { 
	    if (tp->val < key) tpp = &tp->right;
	    else tpp = &tp->left;
	}
	tp = (REF(tree))new tree(key);
	assert(tp != NULL);
	*tpp = tp;
    }

    tree* search(int key) { 
	shared_lock s_lock(shmem);
	tree* tp = root;
	while (tp != NULL) { 
	    if (tp->val < key) tp = tp->right; 
	    else if (tp->val > key) tp = tp->left; 
	    else return tp;
	}
	return NULL;
    }
    
    void remove(int key) { 
	exclusive_lock x_lock(shmem);
	REF(tree) tp;
	REF(tree) *tpp = &root;
	while ((tp = *tpp) != NULL) { 
	    if (tp->val < key) { 
		tpp = &tp->right;
	    } else if (tp->val > key) { 
		tpp = &tp->left; 
	    } else break;
	}
	assert(tp != NULL);
	if (tp->left == NULL) *tpp = tp->right;
	else if (tp->right == NULL) *tpp = tp->left;
	else { 
	    REF(tree) rp;
	    REF(tree) *rpp = &tp->left;
	    while ((rp = *rpp)->right != NULL) rpp = &rp->right;
	    *tpp = rp;
	    *rpp = rp->left;
	    rp->left = tp->left;
	    rp->right = tp->right;
	} 
	delete tp;
    }
    void* operator new(size_t size) { 
	return shmem.allocate(size);
    }
    void operator delete(void* p) { 
	shmem.free(p);
    }
    root_object() { root = NULL; }
};

#else

class root_object { 
  public:
    tree* root;

    void insert(int key) {
	exclusive_lock x_lock(shmem);
	tree *tp, **tpp = &root;
	while ((tp = *tpp) != NULL) { 
	    if (tp->val < key) tpp = &tp->right;
	    else tpp = &tp->left;
	}
	tp = new tree(key);
	assert(tp != NULL);
	*tpp = tp;
    }

    tree* search(int key) { 
	shared_lock s_lock(shmem);
	tree* tp = (tree*)root;
	while (tp != NULL) { 
	    if (tp->val < key) tp = tp->right; 
	    else if (tp->val > key) tp = tp->left; 
	    else return tp;
	}
	return NULL;
    }
    
    void remove(int key) { 
	exclusive_lock x_lock(shmem);
	tree *tp, **tpp = &root;
	while ((tp = *tpp) != NULL) { 
	    if (tp->val < key) { 
		tpp = &tp->right;
	    } else if (tp->val > key) { 
		tpp = &tp->left; 
	    } else break;
	}
	assert(tp != NULL);
	if (tp->left == NULL) *tpp = tp->right;
	else if (tp->right == NULL) *tpp = tp->left;
	else { 
	    tree *rp, **rpp = &tp->left;
	    while ((rp = *rpp)->right != NULL) rpp = &rp->right;
	    *tpp = rp;
	    *rpp = rp->left;
	    rp->left = tp->left;
	    rp->right = tp->right;
	} 
	delete tp;
    }
    void* operator new(size_t size) { 
	return shmem.allocate(size);
    }
    void operator delete(void* p) { 
	shmem.free(p);
    }
    root_object() { root = NULL; }
};
#endif


#ifdef _WIN32
DWORD WINAPI do_test(void* arg)
#else
unsigned do_test(void* arg)
#endif
{
    const unsigned modulus = 2147483647; 
    const unsigned factor = 397204094; 
    unsigned randseed = (unsigned)(long)arg;
    root_object* root = (root_object*)shmem.get_root_object();
    int i, j;

    for (i = 0; i < n_inserts; i++) { 
	exclusive_lock x_lock(shmem); // just test nested locks
	randseed = randseed * factor % modulus;
	root->insert(randseed);
    } 	
    shmem.flush();
    { 
	shared_lock s_lock(shmem); 
	shmem.check_heap();
    }
    for (i = 0; i < n_searches; i++) { 
	randseed = (unsigned)(long)arg;
	for (j = 0; j < n_inserts; j++) { 
	    shared_lock s_lock(shmem); // just test nested locks
	    randseed = randseed * factor % modulus;
	    tree* tp = root->search(randseed);
	    assert(tp != NULL && unsigned(tp->val) == randseed);
	}
    }

    randseed = (unsigned)(long)arg;
    for (i = 0; i < n_inserts; i++) { 
	randseed = randseed * factor % modulus;
	root->remove(randseed);
    } 	    
    return 0;
}

int main(int argc, char* argv[]) 
{
    char buf[256];
    shared_memory::status rc;
    root_object* root;
    int n = 1;
 
   
    if (argc >= 2) { 
	if (*argv[1] == '-') { 
	    fprintf(stderr, "Usage: tstshmem [number-of-threads]\n");
	    return EXIT_FAILURE;
	}
	n = atoi(argv[1]);
    }
    if (n <= 0 || n > 32) { 
	fprintf(stderr, "Number of threads should be in range [1..32]\n");
	return EXIT_FAILURE;
    }
	
    rc = shmem.open("test.odb", "test", max_size);
    if (rc != shared_memory::ok) { 
	shmem.get_error_text(rc, buf, sizeof buf);
	fprintf(stderr, "Failed to open file: %s\n", buf);
	return EXIT_FAILURE;
    } else { 
	exclusive_lock x_lock(shmem);
	root = (root_object*)shmem.get_root_object();
	if (root == NULL) { 
	    root = new root_object;
	    shmem.set_root_object(root);
	}
    }

#ifdef _WIN32
    HANDLE h[32];
    DWORD start = GetTickCount();
    for (int i = 0; i < n; i++) { 
	DWORD threadid;
	h[i] = CreateThread(NULL, NULL, do_test, (void*)(i+1), 0, &threadid);
    }
    WaitForMultipleObjects(n, h, TRUE, INFINITE);
    printf("Elapsed time: %d msec\n", GetTickCount() - start);
#else
    struct timeval start;
    gettimeofday(&start, NULL);
    do_test((void*)1);
    struct timeval stop;
    gettimeofday(&stop, NULL);
    unsigned long delta = (stop.tv_sec*1000 + stop.tv_usec/1000)
	                - (start.tv_sec*1000 + start.tv_usec/1000);
    printf("Elapsed time: %lu msec\n", delta);
#endif
    shmem.close();
    return EXIT_SUCCESS;
}



