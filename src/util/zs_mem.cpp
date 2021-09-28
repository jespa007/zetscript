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
}
