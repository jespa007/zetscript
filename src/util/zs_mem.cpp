/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
#include "zs_util.h"

namespace zetscript{
	void *zs_malloc(size_t _size,const char *_file, int _line){
		void *p=NULL;
#ifdef __MEMMANAGER__
		p=MEMMGR_malloc(_size,_file,_line);
#else
		ZS_UNUSUED_2PARAMS(_file, _line);
		p=malloc(_size);
#endif
		memset(p,0,_size);
		return p;
	}

}
