#pragma once

#define ZS_MALLOC(s) zetscript::zs_malloc(s)

namespace zetscript{
	void *zs_malloc(size_t _size);

}
