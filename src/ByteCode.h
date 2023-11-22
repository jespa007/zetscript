/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */

#pragma once


#include "ByteCode.h"

namespace zetscript{

	class ByteCodeHelper{
	public:
		static const char * getName(ByteCode  _byte_code);
		static const char * getNameOperator(ByteCode _byte_code);
		static ByteCode		loadVarTypeToPushStk(ByteCode _byte_code);
		static bool		 	isLoadVarType(ByteCode _byte_code);
	};


}
