#include <Interfaces/ParallelsTypes.h>

#ifdef VALGRIND_ENABLED

#include "../External/valgrind/valgrind.h"
#include "../External/valgrind/memcheck.h"

/*
 * Wrapper for RAND_bytes from libssl
 * Running only if prl_vm_app under valgrind.
 * So this wrapper doesn't affect prl_vm_app
 * if valgrind is not used for running.
 *
 * The problem is uninitialized buffer used in libssl
 * as a source of entropy. It's generate a lot of false positives
 * of valgrind.
 *
 * For details on valgrind wrappers:
 * http://valgrind.org/docs/manual/manual-core-adv.html
 */

extern "C"
int RAND_bytes(unsigned char *buf, int num);

extern "C"
int I_WRAP_SONAME_FNNAME_ZU(NONE, RAND_bytes)(unsigned char *buf, int num);

int I_WRAP_SONAME_FNNAME_ZU(NONE, RAND_bytes)(unsigned char *buf, int num)
{
	int result;
	OrigFn fn;
	VALGRIND_GET_ORIG_FN(fn);
	if (buf && num)
		VALGRIND_MAKE_MEM_DEFINED(buf, num);
	// call original function
	CALL_FN_W_WW(result, fn, (long)buf, (long)num);
	return result;
}

#endif // VALGRIND_ENABLED
