/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
#pragma once

#include "@util.h"

namespace zetscript{
	Buffer::Buffer(uint8_t *_ptr,int  _ptr_len){
		ptr=_ptr;
		ptr_len=_ptr_len;
	}

	Buffer::~Buffer(){
		if(ptr != NULL){
			free(ptr);
		}
		ptr=NULL;
	}
}
