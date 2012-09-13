#include <lucas.h>

int lucas_errno = 0;

int lucas(linfo_t *gen, int n, comb_t *ans)
{
    int result = 0;
    // missing logic
	if (!ans || !gen || n < 0) { 
		return -1;		// check for error conditions 
	}
	comb_t ans1, ans2;		// create locals to store return vals
	if (n == 0) {
		*ans = gen->l0;
		return result;
	}
	if (n == 1) { 
		*ans = gen->l1;
		return result;
	}
	lucas (gen, n-1, &ans1);
	lucas (gen, n-2, &ans2);
	*ans = (gen->p * ans1 - gen->q * ans2 );
    return result;
}

