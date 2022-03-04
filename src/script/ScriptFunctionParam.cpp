/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
#include "zetscript.h"

namespace zetscript{

	ScriptFunctionParam::ScriptFunctionParam(){
		idx_script_type=ZS_IDX_UNDEFINED;
		name="";
		line=ZS_IDX_UNDEFINED;
		properties=0;
		default_param_value=k_stk_null;
	}

	ScriptFunctionParam::ScriptFunctionParam(const zs_string & _name){
		idx_script_type=ZS_IDX_UNDEFINED;
		name=_name;
		line=ZS_IDX_UNDEFINED;
		properties=0;
		default_param_value=k_stk_null;
	}

	ScriptFunctionParam::ScriptFunctionParam(int _idx_script_type, const zs_string & _name){
		idx_script_type=_idx_script_type;
		name=_name;
		line=ZS_IDX_UNDEFINED;
		properties=0;
		default_param_value=k_stk_null;
	}

	ScriptFunctionParam::ScriptFunctionParam( const ScriptFunctionParam & _function_param){
		copy(_function_param);
	}

	void ScriptFunctionParam::copy( const ScriptFunctionParam & _function_param){
		idx_script_type=_function_param.idx_script_type;
		name=_function_param.name;
		line=_function_param.line;
		properties=_function_param.properties;
		default_param_value=_function_param.default_param_value;
	}

	ScriptFunctionParam & ScriptFunctionParam::operator =( const ScriptFunctionParam & _function_param){
		copy(_function_param);
		return *this;
	}

	ScriptFunctionParam *ScriptFunctionParam::createArrayFromVector(const zs_vector * _s){
		ScriptFunctionParam *params=new ScriptFunctionParam[_s->count];

		for(int i=0; i < _s->count; i++){
			params[i]=*((ScriptFunctionParam *)_s->items[i]);
		}

		return params;

	}

	ScriptFunctionParam *ScriptFunctionParam::createArrayFromScriptFunction(const ScriptFunction * _sf){
		ScriptFunctionParam *params=new ScriptFunctionParam[_sf->params_len];

		for(int i=0; i < _sf->params_len; i++){
			params[i]=_sf->params[i];
		}

		return params;

	}

	ScriptFunctionParam::~ScriptFunctionParam(){

	}
}

