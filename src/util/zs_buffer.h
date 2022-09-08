/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
#pragma once

namespace zetscript{

	class zs_buffer{
	public:
		uint8_t *ptr;
		int		ptr_len;
		zs_buffer(uint8_t *ptr,int   ptr_len);
		~zs_buffer();
	};

}
