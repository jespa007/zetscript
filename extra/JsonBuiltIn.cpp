/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */

#include "zetscript.h"

namespace zetscript{

	std::string JsonBuiltIn::deserialize(ScriptObject *obj){

		return "";
	}


	ScriptObject * JsonBuiltIn::serialize(std::string *){

		return (zs_int)(*number);
	}
}
