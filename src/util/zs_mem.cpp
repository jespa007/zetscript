#include "zs_util.h"

namespace zetscript{
	void *zs_malloc(size_t _size){
		void *p=malloc(_size);
		memset(p,0,_size);
		return p;
	}
}
