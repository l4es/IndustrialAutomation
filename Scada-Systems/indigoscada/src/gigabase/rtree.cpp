//-< RTREE.CPP >-----------------------------------------------------*--------*
// GigaBASE                  Version 1.0         (c) 1999  GARRET    *     ?  *
// (Post Relational Database Management System)                      *   /\|  *
//                                                                   *  /  \  *
//                          Created:     22-Nov-2001  K.A. Knizhnik  * / [] \ *
//                          Last update: 22-Nov-2001  K.A. Knizhnik  * GARRET *
//-------------------------------------------------------------------*--------*
// R-tree class implementation
//-------------------------------------------------------------------*--------*

#define INSIDE_GIGABASE

#include "gigabase.h"
#include "rtree.h"

BEGIN_GIGABASE_NAMESPACE

void dbRtree::insert(dbDatabase* db, oid_t treeId, oid_t recordId, int offs)
{
    dbGetTie treeTie;
    dbRtree* tree = (dbRtree*)db->getRow(treeTie, treeId);
    dbGetTie tie;
    byte* record = (byte*)db->getRow(tie, recordId);
    rectangle& r = *(rectangle*)(record + offs);
    if (tree->root == 0) { 
        dbPutTie tie;
        dbRtree* t = (dbRtree*)db->putRow(tie, treeId);
        t->root = dbRtreePage::allocate(db, recordId, r);
        t->height = 1;  
    } else { 
        oid_t p = dbRtreePage::insert(db, r, tree->root, recordId, tree->height);
        if (p != 0) { 
            dbPutTie tie;
            dbRtree* t = (dbRtree*)db->putRow(tie, treeId);
            // root splitted
            t->root = dbRtreePage::allocate(db, tree->root, p);
            t->height += 1;
        }
    }
}


void dbRtree::insert(dbDatabase* db, oid_t treeId, oid_t recordId, rectangle const& r)
{
    dbGetTie treeTie;
    dbRtree* tree = (dbRtree*)db->getRow(treeTie, treeId);
    if (tree->root == 0) { 
        dbPutTie tie;
        dbRtree* t = (dbRtree*)db->putRow(tie, treeId);
        t->root = dbRtreePage::allocate(db, recordId, r);
        t->height = 1;  
    } else { 
        oid_t p = dbRtreePage::insert(db, r, tree->root, recordId, tree->height);
        if (p != 0) { 
            dbPutTie tie;
            dbRtree* t = (dbRtree*)db->putRow(tie, treeId);
            // root splitted
            t->root = dbRtreePage::allocate(db, tree->root, p);
            t->height += 1;
        }
    }
}


void dbRtree::remove(dbDatabase* db, oid_t treeId, oid_t recordId, int offs)
{
    dbGetTie treeTie;
    dbRtree* tree = (dbRtree*)db->getRow(treeTie, treeId);
    assert(tree->height != 0);

    dbGetTie getTie;
    byte* record = (byte*)db->getRow(getTie, recordId);
    rectangle& r = *(rectangle*)(record + offs);

    dbRtreePage::reinsert_list rlist;
    bool found = dbRtreePage::remove(db, r, tree->root, recordId, tree->height, rlist);
    assert(found);

    dbPutTie putTie;
    oid_t p = rlist.chain;
    int level = rlist.level;
    bool rootSplitted = false;
    
    while (p != 0) {
        dbRtreePage* pg = (dbRtreePage*)db->get(p);
        for (int i = 0, n = pg->n; i < n; i++) { 
            int q = dbRtreePage::insert(db, pg->b[i].rect, tree->root, 
                                        pg->b[i].p, tree->height-level);
            if (q != 0) { 
                // root splitted
                oid_t oldRoot = tree->root;
                if (!rootSplitted) { 
                    tree = (dbRtree*)db->putRow(putTie, treeId);
                    rootSplitted = true;
                }
                tree->root = dbRtreePage::allocate(db, oldRoot, q);
                tree->height += 1;
            }
        }
        level -= 1;
        oid_t next = pg->next_reinsert_page();
        db->pool.unfix(pg);
        db->freePage(p);
        p = next;
    }
    dbRtreePage* pg = (dbRtreePage*)db->get(tree->root);
    if (pg->n == 1 && tree->height > 1) { 
        oid_t newRoot = pg->b[0].p;
        db->freePage(tree->root);
        if (!rootSplitted) { 
            tree = (dbRtree*)db->putRow(putTie, treeId);
        }
        tree->root = newRoot;
        tree->height -= 1;
    }
    db->pool.unfix(pg);
}

bool dbRtree::find(dbDatabase* db, oid_t treeId, dbSearchContext& sc)
{
    dbGetTie treeTie;
    dbRtree* tree = (dbRtree*)db->getRow(treeTie, treeId);
    if (tree->height > 0) {
        return dbRtreePage::find(db, tree->root, sc, tree->height);
    }
    return true;
}

void dbRtree::purge(dbDatabase* db, oid_t treeId)
{
    dbPutTie treeTie;
    dbRtree* tree = (dbRtree*)db->putRow(treeTie, treeId);
    if (tree->height > 0) {
        dbRtreePage::purge(db, tree->root, tree->height);
    }
    tree->root = 0;
    tree->height = 0;
}

void dbRtree::drop(dbDatabase* db, oid_t treeId)
{
    purge(db, treeId);
    db->free(db->getPos(treeId) & ~dbFlagsMask, sizeof(dbRtree));
    db->freeId(treeId);
}

oid_t dbRtree::allocate(dbDatabase* db)
{
    oid_t oid = db->allocateId();
    offs_t pos = db->allocate(sizeof(dbRtree));
    db->setPos(oid, pos | dbModifiedFlag);
    dbPutTie tie;
    tie.set(db->pool, oid, pos, sizeof(dbRtree));
    dbRtree* tree = (dbRtree*)tie.get();
    tree->size = sizeof(dbRtree);
    tree->root = 0;
    tree->height = 0;
    return oid;
}

//-------------------------------------------------------------------------
// R-tree page methods
//-------------------------------------------------------------------------

//
// Search for objects overlapped with specified rectangle and call
// callback method for all such objects.
//

bool dbRtreePage::find(dbDatabase* db, oid_t pageId, dbSearchContext& sc, int level) 
{
    dbRtreePage* pg = (dbRtreePage*)db->get(pageId);
    bool rc = pg->find(db, sc, level);
    db->pool.unfix(pg);
    return rc;
}

static rectangle::comparator comparators[] = 
{
    &rectangle::operator ==, 
    &rectangle::operator &,
    &rectangle::operator >, 
    &rectangle::operator >=, 
    &rectangle::operator <, 
    &rectangle::operator <= 
};

bool dbRtreePage::find(dbDatabase* db, dbSearchContext& sc, int level) const
{
    assert(level >= 0);
    rectangle& r = *(rectangle*)sc.firstKey;
    sc.probes += 1;
    if (--level != 0) { /* this is an internal node in the tree */
        for (int i = 0; i < n; i++) { 
            if (b[i].rect & r) {
                if (!find(db, b[i].p, sc, level)) { 
                    return false;
                }
            }
        }
    } else { /* this is a leaf node */
        rectangle::comparator cmp = comparators[sc.firstKeyInclusion]; 
        for (int i = 0; i < n; i++) { 
            if ((b[i].rect.*cmp)(r)) {              
                if (sc.condition == NULL 
                    || db->evaluateBoolean(sc.condition, b[i].p, sc.cursor->table, sc.cursor)) 
                {
                    if (!sc.cursor->add(b[i].p)) { 
                        return false;
                    }
                }
            }
        }
    }
//    printf("Level %d, nodes %d, intersects %d\n", level, n, nIntersects);
    return true;
}

//
// Create root page
//
oid_t dbRtreePage::allocate(dbDatabase* db, oid_t recordId, rectangle const& r)
{
    oid_t pageId = db->allocatePage();
    dbRtreePage* pg = (dbRtreePage*)db->put(pageId);
    pg->n = 1;
    pg->b[0].rect = r;
    pg->b[0].p = recordId;
    db->pool.unfix(pg);
    return pageId;
}

//
// Create new root page (root splitting)
//
oid_t dbRtreePage::allocate(dbDatabase* db, oid_t oldRootId, oid_t newPageId)
{
    oid_t pageId = db->allocatePage();
    dbRtreePage* pg = (dbRtreePage*)db->put(pageId);
    pg->n = 2;
    cover(db, oldRootId, pg->b[0].rect);
    pg->b[0].p = oldRootId;
    cover(db, newPageId, pg->b[1].rect);
    pg->b[1].p = newPageId;
    db->pool.unfix(pg);
    return pageId;
}

//
// Calculate cover of all rectangles at page
//
void dbRtreePage::cover(dbDatabase* db, oid_t pageId, rectangle& r) 
{
    dbRtreePage* pg = (dbRtreePage*)db->get(pageId);
    pg->cover(r);
    db->pool.unfix(pg);
}

void dbRtreePage::cover(rectangle& r) const 
{
    r = b[0].rect;
    for (int i = 1; i < n; i++) { 
        r += b[i].rect;
    }
}

#define INFINITY (area_t)1000000000*1000000000

oid_t dbRtreePage::split_page(dbDatabase* db, branch const& br)
{
    int i, j, seed[2];
    area_t rect_area[card+1], waste, worst_waste = -INFINITY;
    //
    // As the seeds for the two groups, find two rectangles which waste 
    // the most area if covered by a single rectangle.
    //
    rect_area[0] = area(br.rect);
    for (i = 0; i < card; i++) { 
        rect_area[i+1] = area(b[i].rect);
    }
    branch const* bp = &br;
    for (i = 0; i < card; i++) { 
        for (j = i+1; j <= card; j++) { 
            waste = area(bp->rect + b[j-1].rect) - rect_area[i] - rect_area[j];
            if (waste > worst_waste) {
                worst_waste = waste;
                seed[0] = i;
                seed[1] = j;
            }
        }
        bp = &b[i];
    }       
    char taken[card];
    rectangle group[2];
    area_t group_area[2];
    int    group_card[2];
    oid_t  pid;
    
    memset(taken, 0, sizeof taken);
    taken[seed[1]-1] = 2;
    group[1] = b[seed[1]-1].rect;
    
    if (seed[0] == 0) { 
        group[0] = br.rect;
        pid = allocate(db, br.p, br.rect);
    } else { 
        group[0] = b[seed[0]-1].rect;
        pid = allocate(db, b[seed[0]-1].p, group[0]);
        b[seed[0]-1] = br;
    }
    dbRtreePage* p = (dbRtreePage*)db->put(pid);
    
    group_card[0] = group_card[1] = 1;
    group_area[0] = rect_area[seed[0]];
    group_area[1] = rect_area[seed[1]];
    //
    // Split remaining rectangles between two groups.
    // The one chosen is the one with the greatest difference in area 
    // expansion depending on which group - the rect most strongly 
    // attracted to one group and repelled from the other.
    //
    while (group_card[0] + group_card[1] < card + 1 
           && group_card[0] < card + 1 - min_fill
           && group_card[1] < card + 1 - min_fill)
    {
        int better_group = -1, chosen = -1;
        area_t biggest_diff = -1;
        for (i = 0; i < card; i++) { 
            if (!taken[i]) { 
                area_t diff = (area(group[0] + b[i].rect) - group_area[0])
                             - (area(group[1] + b[i].rect) - group_area[1]);
                if (diff > biggest_diff || -diff > biggest_diff) { 
                    chosen = i;
                    if (diff < 0) { 
                        better_group = 0;
                        biggest_diff = -diff;
                    } else { 
                        better_group = 1;
                        biggest_diff = diff;
                    }
                }
            }
        }
        assert(chosen >= 0);
        group_card[better_group] += 1;
        group[better_group] += b[chosen].rect;
        group_area[better_group] = area(group[better_group]);
        taken[chosen] = better_group+1;
        if (better_group == 0) { 
            p->b[group_card[0]-1] = b[chosen];
        }
    }
    //
    // If one group gets too full, then remaining rectangle are
    // split between two groups in such way to balance cards of two groups.
    //
    if (group_card[0] + group_card[1] < card + 1) { 
        for (i = 0; i < card; i++) { 
            if (!taken[i]) { 
                if (group_card[0] >= group_card[1]) { 
                    taken[i] = 2;
                    group_card[1] += 1;
                } else { 
                    taken[i] = 1;
                    p->b[group_card[0]++] = b[i];               
                }
            }
        }
    }
    p->n = group_card[0];
    n = group_card[1];
    for (i = 0, j = 0; i < n; j++) { 
        if (taken[j] == 2) {
            b[i++] = b[j];
        }
    }
    db->pool.unfix(p);
    return pid;
}

void dbRtreePage::remove_branch(int i)
{
    n -= 1;
    memmove(&b[i], &b[i+1], (n-i)*sizeof(branch));
}
    
oid_t dbRtreePage::insert(dbDatabase* db, rectangle const& r, oid_t pageId, oid_t recordId, int level)
{
    dbRtreePage* pg = (dbRtreePage*)db->put(pageId);
    oid_t oid = pg->insert(db, r, recordId, level);
    db->pool.unfix(pg);
    return oid;
}

oid_t dbRtreePage::insert(dbDatabase* db, rectangle const& r, oid_t recordId, int level)
{
    branch br;
    if (--level != 0) { 
        // not leaf page
        int i, mini = 0;
        area_t min_incr = INFINITY;
        area_t best_area = INFINITY;
        for (i = 0; i < n; i++) { 
            area_t r_area = area(b[i].rect);
            area_t incr = area(b[i].rect + r) - r_area;
            if (incr < min_incr) { 
                best_area = r_area;
                min_incr = incr;
                mini = i;
            } else if (incr == min_incr && r_area < best_area) { 
                best_area = r_area;
                mini = i;
            }   
        }
        oid_t q = insert(db, r, b[mini].p, recordId, level);
        if (q == 0) { 
            // child was not split
            b[mini].rect += r;
            return 0;
        } else { 
            // child was split
            cover(db, b[mini].p, b[mini].rect);
            br.p = q;
            cover(db, q, br.rect);
            return add_branch(db, br);
        }
    } else { 
        br.p = recordId;
        br.rect = r;
        return add_branch(db, br);
    }
}

bool dbRtreePage::remove(dbDatabase* db, rectangle const& r,  oid_t pageId, oid_t recordId, 
                         int level, reinsert_list& rlist)
{
    dbRtreePage* pg = (dbRtreePage*)db->put(pageId);
    bool rc = pg->remove(db, r, recordId, level, rlist);
    db->pool.unfix(pg);
    return rc;    
}

bool dbRtreePage::remove(dbDatabase* db, rectangle const& r, oid_t recordId, int level,
                         reinsert_list& rlist)
{
    if (--level != 0) { 
        for (int i = 0; i < n; i++) { 
            if (b[i].rect & r) { 
                if (remove(db, r, b[i].p, recordId, level, rlist)) { 
                    dbRtreePage* p = (dbRtreePage*)db->get(b[i].p);
                    if (p->n >= min_fill) { 
                        p->cover(b[i].rect);
                    } else { 
                        // not enough entries in child
                        db->pool.unfix(p);
                        p = (dbRtreePage*)db->put(b[i].p);
                        p->b[card-1].p = rlist.chain;
                        rlist.chain = b[i].p;
                        rlist.level = level - 1; 
                        remove_branch(i);
                    }
                    db->pool.unfix(p);
                    return true;
                }
            }
        }
    } else {
        for (int i = 0; i < n; i++) { 
            if (b[i].p == recordId) { 
                remove_branch(i);
                return true;
            }
        }
    }
    return false;
}

void dbRtreePage::purge(dbDatabase* db, oid_t pageId, int level)
{
    if (--level != 0) { /* this is an internal node in the tree */
        dbRtreePage* pg = (dbRtreePage*)db->get(pageId);
        for (int i = 0; i < pg->n; i++) { 
            purge(db, pg->b[i].p, level);
        }
        db->pool.unfix(pg);
    }
    db->freePage(pageId);
}

END_GIGABASE_NAMESPACE
