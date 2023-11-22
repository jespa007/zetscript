/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */

#pragma once


#include "ByteCode.h"

namespace zetscript{

	class ByteCodeHelper{
	public:
		static const char * getByteCodeName(ByteCode  _byte_code);
		static const char * getByteCodeOperatorName(ByteCode _byte_code);
		static ByteCode		byteCodeLoadVarTypeToPushStk(ByteCode _byte_code);
		static bool		 	isByteCodeLoadVarType(ByteCode _byte_code);
	};


}
