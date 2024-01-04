/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */

#pragma once

namespace zetscript{

	class ByteCodeHelper{
	public:
		static const char * getByteCodeName(ByteCode  _byte_code);
		static const char * getByteCodeOperatorName(ByteCode _byte_code);
	};

}
