/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
#include "zetscript.h"

namespace zetscript{

	zs_int IntegerBuiltIn::parse(zs_float *number){

		return (zs_int)(*number);
	}
}
