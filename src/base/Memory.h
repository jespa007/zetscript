/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
#pragma once

#define ZS_MALLOC(s) zetscript::zs_malloc(s,__FILE__,__LINE__)

namespace zetscript{
	void *zs_malloc(size_t _size,const char *_file, int _line);
}
