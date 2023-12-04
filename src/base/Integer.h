/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
#pragma once

namespace zetscript{

	class Integer{
	public:

		typedef enum{
			INTEGER_TYPE_INVALID=0,
			INTEGER_TYPE_HEXA,
			INTEGER_TYPE_BINARY,
			INTEGER_TYPE_INT,
		}IntegerType;


		 static zs_int		* 	parse(const String & );
		 static String 			toString(zs_int _number, const String & _format="");

	private:
		 static IntegerType		isInteger(const String & test_str);
	};
}
