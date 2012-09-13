/*  clist.h
    -------
    Header for the library providing a linked list, suitable for storing
    combinatorial sequence members, associated with <combinatorics.h>
*/

#ifndef _CLIST_H_
#define _CLIST_H_

#include <combinatorics.h>

// struct for the nodes of a linked list
//
typedef struct cnode cnode_t;
struct cnode {
    cnode_t *next;
    comb_t value;
    unsigned int pos;
    int type;
};

// DESCRIPTION
//    ordered_add() can be used to manage a list of type cnode_t nodes
//    such that adding a new node maintains their order. In
//    particular, the new node will appear in a position based
//    on the cnode_t value, with smallest values appearing
//    toward the head of the list.
//
//    If the input list is unordered, the behavior is undefined.
//    Caller is resonsible for freeing the new node inserted
//    into the list.
// INPUT 
//    type - the type of the combinatorial sequence, e.g. BELL
//    pos - the sequence number, e.g. n
//    value - value for the pos in the combinatorial sequence, e.g. B(n)
//    head - pointer to an (ordered) list of combinatorial values
// OUTPUT
//    head - points to an (ordered) list, which includes this new member
// RETURN VALUE
//    0 on success, -1 on error
//
int ordered_add(int type, unsigned int pos, comb_t value, cnode_t **head);


// DESCRIPTION
//    free_clist() frees all cnode_t objects in the input linked list
// INPUT 
//    list - a list of combinatorial values
// RETURN VALUE
//    none
//
void free_clist(cnode_t *list);

#endif
