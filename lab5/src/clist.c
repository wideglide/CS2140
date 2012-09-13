#include <combinatorics.h>
#include <clist.h>
#include <stdio.h>
#include <stdlib.h>


int ordered_add(int type, unsigned int pos, comb_t value, cnode_t **head)
{
    int result = 0;
    if (head == NULL) { return -1; } // check for bad args
    cnode_t *p = *head, *q = NULL;
    cnode_t *n = NULL;
	// missing logic
	if (type < 0 || pos < 0) { return -1; } // check for errors
	while (p) {		// search for insertion point in list
		if (p->type == type && p->pos > pos ) {
			break;
		}
		if (type > p->type) { break; }
		q = p;
		p = p->next;
	}
	
	n = malloc(sizeof(cnode_t));	// alloc memory for node
	if (!n) {
		return -1;		// check for malloc fail
	}
	n->next = NULL;			// initialize the new node
	n->value = value;
	n->pos = pos;
	n->type = type;
	
	if (q == NULL) {	// insert after head
		*head = n;
		n->next = p;
	} else {		// insert between nodes or at the end
		q->next = n;
		n->next = p;
	}
	
    return result;
}


void free_clist(cnode_t *list)
{
   // missing logic
   cnode_t *p = list;
   while(list) {		// follow the list and free alloc's
	   p = list;
	   list = list->next;
	   free(p);
   }
}

