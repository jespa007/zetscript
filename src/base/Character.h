/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
#pragma once

namespace zetscript{

	class Character{
	public:
		 static char 	toLowerCase(char _c);
		 static char 	toUpperCase(char _c);
		 static bool 	isDigit(char _c);
		 static bool 	isHexaDigit(char _c);
	};
}
