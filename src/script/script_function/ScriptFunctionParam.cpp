/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
#include "zetscript.h"

namespace zetscript{

	void ScriptFunctionParam::init(){
		id=ZS_UNDEFINED_IDX;
		name="";
		line=ZS_UNDEFINED_IDX;
		properties=0;
		default_param_value=k_stk_undefined;
	}

	ScriptFunctionParam::ScriptFunctionParam() {
		init();
	}

	ScriptFunctionParam::ScriptFunctionParam(const zs_string & _name){
		id=ZS_UNDEFINED_IDX;
		name=_name;
		line=ZS_UNDEFINED_IDX;
		properties=0;
		default_param_value=k_stk_undefined;
	}

	ScriptFunctionParam::ScriptFunctionParam(int _idx_type, const zs_string & _name){
		id=_idx_type;
		name=_name;
		line=ZS_UNDEFINED_IDX;
		properties=0;
		default_param_value=k_stk_undefined;
	}

	ScriptFunctionParam::ScriptFunctionParam( const ScriptFunctionParam & _function_param){
		init();
		copy(_function_param);
	}

	void ScriptFunctionParam::copy( const ScriptFunctionParam & _function_param){
		id=_function_param.id;
		name = _function_param.name;
		line=_function_param.line;
		properties=_function_param.properties;
		default_param_value=_function_param.default_param_value;
	}

	ScriptFunctionParam & ScriptFunctionParam::operator =( const ScriptFunctionParam & _function_param){
		copy(_function_param);
		return *this;
	}

	ScriptFunctionParam *ScriptFunctionParam::createArrayFromArray(const zs_vector<ScriptFunctionParam *> * _script_function_params){
		ScriptFunctionParam *params=new ScriptFunctionParam[_script_function_params->size()];

		for(int i=0; i < _script_function_params->size(); i++){
			params[i]=*((ScriptFunctionParam *)_script_function_params->get(i));
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

