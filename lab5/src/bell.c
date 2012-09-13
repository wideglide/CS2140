#include <bell.h>

int bell_errno = 0;

int _bell(int n1, int n2, comb_t *ans)
{
    int result = 0;
	// missing logic
	comb_t ans0, ans1, ans2;		// create locals for return
	if ((n1 == 0 && n2 == 0) || (n1 == 1 && n2 == 1))  {
		*ans = 1;
		return 0;
	}

	if (n2 == 1) {
		_bell(n1-1, n1-1, &ans0);
		*ans =  ans0;
		return 0;
	}
	_bell (n1-1, n2-1, &ans1);
	_bell (n1, n2-1, &ans2);
	*ans = (ans1 + ans2);
    return result;
}


int bell(int n, comb_t *ans)
{
	if (!ans || n < 0 ) {
		return -1;			//check for error conditions
	} 
    return _bell(n, n, ans);
}


