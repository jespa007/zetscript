#include "zetscript.h"

namespace zetscript{

	ScriptFunctionParam::ScriptFunctionParam(){
		idx_type=ZS_IDX_UNDEFINED;
		name="";
		line=ZS_IDX_UNDEFINED;
		properties=0;
		default_param_value=k_stk_undefined;
	}

	ScriptFunctionParam::ScriptFunctionParam(const zs_string & _name){
		idx_type=ZS_IDX_UNDEFINED;
		name=_name;
		line=ZS_IDX_UNDEFINED;
		properties=0;
		default_param_value=k_stk_undefined;
	}

	ScriptFunctionParam::ScriptFunctionParam(int _idx_type, const zs_string & _name){
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

	ScriptFunctionParam *ScriptFunctionParam::createArrayFromVector(const zs_vector * _s){
		ScriptFunctionParam *params=new ScriptFunctionParam[_s->count];
		size_t params_len=_s->count;

		for(unsigned i=0; i < _s->count; i++){
			params[i]=*((ScriptFunctionParam *)_s->items[i]);
		}

		return params;

	}

	ScriptFunctionParam *ScriptFunctionParam::createArrayFromScriptFunction(const ScriptFunction * _sf){
		ScriptFunctionParam *params=new ScriptFunctionParam[_sf->params_count];
		size_t params_len=_sf->params_count;

		for(unsigned i=0; i < _sf->params_count; i++){
			params[i]=_sf->params[i];
		}

		return params;

	}
}

