/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */

#include "zetscript.h"

namespace zetscript{

	typedef struct{
		const char *filename;
		const char *str_start;
	}DeserializeData;

	extern char json_message_error[16836];



	void throw_error(DeserializeData *deserialize_data, const char * str_current, int line, const char *string_text, ...);
	void throw_warning(DeserializeData *deserialize_data, const char * str_current, int line, const char *string_text, ...);


	const char end_char_standard_value[] = {
			',',
			'}',
			' ',
			'\t',
			'\n',
			'\r', // for make compatible windows...
			']',
			0
	};

	#include "JsonBuiltinDeserialize.cpp"
	#include "JsonBuiltinSerialize.cpp"

	ScriptObjectObject * JsonBuiltIn::deserialize(ScriptObjectString *str_json){
		// deserialize ...
	}

	ScriptObjectString * JsonBuiltIn::serialize(ScriptObjectObject *obj,bool minimized){
		// iterate for all elements in the obj

	}
}
