#include "zetscript.h"

namespace zetscript{

	ScriptFunctionArg::ScriptFunctionArg(){
		idx_type=ZS_IDX_UNDEFINED;
		arg_name="";
		line=ZS_IDX_UNDEFINED;
		properties=0;
		default_var_value=k_stk_undefined;
	}

	ScriptFunctionArg::ScriptFunctionArg(std::string _arg_name){
		idx_type=ZS_IDX_UNDEFINED;
		arg_name=_arg_name;
		line=ZS_IDX_UNDEFINED;
		properties=0;
		default_var_value=k_stk_undefined;
	}

	ScriptFunctionArg::ScriptFunctionArg(int _idx_type, std::string _arg_name){
		idx_type=_idx_type;
		arg_name=_arg_name;
		line=ZS_IDX_UNDEFINED;
		properties=0;
		default_var_value=k_stk_undefined;
	}

	ScriptFunctionArg::ScriptFunctionArg( const ScriptFunctionArg & _function_param){
		idx_type=_function_param.idx_type;
		arg_name=_function_param.arg_name;
		line=_function_param.line;
		properties=_function_param.properties;
		default_var_value=_function_param.default_var_value;
	}
}

