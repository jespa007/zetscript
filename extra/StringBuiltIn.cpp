#include "zetscript.h"

namespace zetscript{

	ScriptObjectString * StringBuiltIn::format(ScriptObjectString *str, ScriptObjectVector *args){
		ScriptObjectString *str_out=new ScriptObjectString(str->getZetScript());
		str_out->str_value=str->str_value;
		return str_out;
	}
}
