/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
#pragma once

#define ZS_MALLOC(s) zetscript::Memory::malloc(s,__FILE__,__LINE__)

namespace zetscript{
	class Memory{
	public:
		static void *malloc(size_t _size,const char *_file, int _line);
	}

}
