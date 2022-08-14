/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
#pragma once

#include "zetscript.h"

namespace zetscript{
	zs_buffer::zs_buffer(uint8_t *_ptr,size_t  _ptr_len){
		ptr=_ptr;
		ptr_len=_ptr_len;
	}

	zs_buffer::~zs_buffer(){
		if(ptr != NULL){
			free(ptr);
		}
		ptr=NULL;
	}
}
