/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
#pragma once

namespace zetscript{

	class Integer{
	public:


		 static zs_int		* 	parse(const String & );
		 static String 			toString(zs_int _number, const String & _format="");


	};
}
