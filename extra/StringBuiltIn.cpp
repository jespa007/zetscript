#include "zetscript.h"

namespace zetscript{

	ScriptVarString * StringBuiltIn::format(ScriptVarString *str, ScriptVarVector *args){
		ScriptVarString *str_out=new ScriptVarString(str->getZetScript());
		str_out->str_value=str->str_value;
		return str_out;
	}
}
