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
		p=malloc(_size);
#endif
		memset(p,0,_size);
		return p;
	}

	void zs_free(void *_ptr,const char *_file, int _line){

#ifdef __MEMMANAGER__
		MEMMGR_free(_ptr,_file,_line);
#else
		free(_ptr);
#endif

	}
}
