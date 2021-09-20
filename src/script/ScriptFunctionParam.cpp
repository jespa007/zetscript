#include "zetscript.h"

namespace zetscript{

	ScriptFunctionParam::ScriptFunctionParam(){
		idx_type=ZS_IDX_UNDEFINED;
		name="";
		line=ZS_IDX_UNDEFINED;
		properties=0;
		default_param_value=k_stk_undefined;
	}

	ScriptFunctionParam::ScriptFunctionParam(const char * _name){
		idx_type=ZS_IDX_UNDEFINED;
		name=_name;
		line=ZS_IDX_UNDEFINED;
		properties=0;
		default_param_value=k_stk_undefined;
	}

	ScriptFunctionParam::ScriptFunctionParam(int _idx_type, const char * _name){
		idx_type=_idx_type;
		name=_name;
		line=ZS_IDX_UNDEFINED;
		properties=0;
		default_param_value=k_stk_undefined;
	}

	ScriptFunctionParam::ScriptFunctionParam( const ScriptFunctionParam & _function_param){
		idx_type=_function_param.idx_type;
		name=_function_param.name;
		line=_function_param.line;
		properties=_function_param.properties;
		default_param_value=_function_param.default_param_value;
	}
}

