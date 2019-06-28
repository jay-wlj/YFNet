
/*
 * QVOD Atom.
 */

#include "qvod_lock.h"


#ifdef EMBEDED /**** EMBEDED ****/



#ifdef C2_EMBEDED
int QvodAtomAdd(qvod_atom_t *value)
{
	++(*value);
	return QVOD_OK;
}

int QvodAtomDec(qvod_atom_t *value)
{
	--(*value);
	return QVOD_OK;
}
#endif /* C2_EMBEDED end */

#ifdef ARM_EMBEDED
int QvodAtomAdd(qvod_atom_t *value)
{
	__sync_add_and_fetch(value, 1);
	return QVOD_OK;
}

int QvodAtomDec(qvod_atom_t *value)
{
	__sync_sub_and_fetch(value, 1);
	return QVOD_OK;
}
#endif /* ARM_EMBEDED end */



#else /**** non-EMBEDED ****/



int QvodAtomAdd(qvod_atom_t *value)
{
#ifdef WIN32 /* WIN32 */

	InterlockedIncrement(value);

#else /* posix */

	__sync_add_and_fetch(value, 1);

#endif /* posix end */

	return QVOD_OK;
}

int QvodAtomDec(qvod_atom_t *value)
{
#ifdef WIN32 /* WIN32 */

	InterlockedDecrement(value);

#else /* posix */

	__sync_sub_and_fetch(value, 1);

#endif /* posix end */

	return QVOD_OK;
}



#endif /**** non-EMBEDED end ****/


