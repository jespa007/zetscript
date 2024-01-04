/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
#pragma once

namespace zetscript{

	class Buffer{
	public:
		uint8_t *ptr;
		int		ptr_len;
		Buffer(uint8_t *ptr,int   ptr_len);
		~Buffer();
	};

}
