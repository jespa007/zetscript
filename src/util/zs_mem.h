/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
#pragma once

#define ZS_MALLOC(s) zetscript::zs_malloc(s,__FILE__,__LINE__)
#define ZS_FREE(ptr) zetscript::zs_free(ptr,__FILE__,__LINE__)

namespace zetscript{
	void *zs_malloc(size_t _size,const char *_file, int _line);
	void zs_free(void *_ptr,const char *_file, int _line);

}
