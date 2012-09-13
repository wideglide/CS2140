#include <stdio.h>
#include <combinatorics.h>
#include <lucas.h>
#include <bell.h>
#include <clist.h>

extern int bell_errno;
extern int lucas_errno;

// an inelegant macro for converting enum comb to strings
#define comb_print(a) \
    ((a)->type == FIB) ? "FIB" : \
    ((a)->type == LUCAS) ? "LUCAS" : \
    ((a)->type == BELL) ? "BELL" : \
    ((a)->type == LUCAS) ? "LUCAS" : "???"

#define COMBN 10


int main(int argc, char *argv[], char *env[])
{
    cnode_t *ptr, *head = NULL;
    comb_t f, b;
    linfo_t info;
    int i;

	// for generating the fibonacci sequence
    info.l0 = 0;
    info.l1 = 1;
    info.p = 1;
    info.q = -1;

    printf("%s \n\t This will generate the first %d sequence numbers"
           " in the\n\t Fibonacci and Bell sequences\n\n", argv[0], COMBN);

    for(i = 0; i < COMBN; i++) {
        lucas(&info, i, &f);
        if (lucas_errno) perror("lucas()");
        ordered_add(FIB, i, f, &head);

        bell(i, &b);
        if (bell_errno) perror("bell()");
        ordered_add(BELL, i, b, &head);
    }
    
    for(ptr = head; ptr != NULL; ptr = ptr->next) {
        printf("%s \t Seq(%d) = %lld\n",
               comb_print(ptr), ptr->pos, ptr->value);
    }

    free_clist(head);
    return 0;
}

