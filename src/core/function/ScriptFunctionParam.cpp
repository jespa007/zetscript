/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
#include "zetscript.h"

namespace zetscript{

	void FunctionParam::init(){
		script_type_id=SCRIPT_TYPE_ID_INVALID;
		name="";
		line=ZS_UNDEFINED_IDX;
		properties=0;
		default_param_value=k_stk_undefined;
	}

	FunctionParam::FunctionParam() {
		init();
	}

	FunctionParam::FunctionParam(const String & _name){
		script_type_id=SCRIPT_TYPE_ID_INVALID;
		name=_name;
		line=ZS_UNDEFINED_IDX;
		properties=0;
		default_param_value=k_stk_undefined;
	}

	FunctionParam::FunctionParam(ScriptTypeId _script_type_id, const String & _name){
		script_type_id=_script_type_id;
		name=_name;
		line=ZS_UNDEFINED_IDX;
		properties=0;
		default_param_value=k_stk_undefined;
	}

	FunctionParam::FunctionParam( const FunctionParam & _function_param){
		init();
		copy(_function_param);
	}

	void FunctionParam::copy( const FunctionParam & _function_param){
		script_type_id=_function_param.script_type_id;
		name = _function_param.name;
		line=_function_param.line;
		properties=_function_param.properties;
		default_param_value=_function_param.default_param_value;
	}

	FunctionParam & FunctionParam::operator =( const FunctionParam & _function_param){
		copy(_function_param);
		return *this;
	}

	FunctionParam *FunctionParam::createArrayFromArray(const Vector<FunctionParam *> * _script_function_params){
		FunctionParam *params=new FunctionParam[_script_function_params->length()];

		for(int i=0; i < _script_function_params->length(); i++){
			params[i]=*((FunctionParam *)_script_function_params->get(i));
		}

		return params;

	}

	FunctionParam *FunctionParam::createArrayFromFunction(const ScriptFunction * _sf){
		FunctionParam *params=new FunctionParam[_sf->params_len];

		for(int i=0; i < _sf->params_len; i++){
			params[i]=_sf->params[i];
		}

		return params;

	}

	FunctionParam::~FunctionParam(){

	}
}

