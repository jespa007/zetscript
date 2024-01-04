/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
#pragma once

namespace zetscript{

	class Float{
	public:
		 static zs_float	* 	parse(const String & );
		 static String 			toString(zs_float _number);
	};
}
