/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
#pragma once

namespace zetscript{

	typedef enum{
		 MSK_SCRIPT_FUNCTION_ARG_PROPERTY_BY_REF	=0x1<<0
		,MSK_SCRIPT_FUNCTION_ARG_PROPERTY_VAR_ARGS	=0x1<<1
	} ScriptFunctionArgProperty;

	class ScriptFunctionParam{
	public:
		int idx_script_type;
		zs_string name; //arg c++ type or arg name
		int line;
		uint16_t properties;
		StackElement default_param_value; // constant int/float/bool/string or anonymous function that return an expression or other object

		ScriptFunctionParam();
		ScriptFunctionParam(const zs_string & _name);
		ScriptFunctionParam(int _idx_script_type, const zs_string & _name);
		ScriptFunctionParam( const ScriptFunctionParam & _function_param);

		ScriptFunctionParam & operator=( const ScriptFunctionParam & _function_param);

		static ScriptFunctionParam *createArrayFromVector(const zs_vector<ScriptFunctionParam *>  * _script_function_params);
		static ScriptFunctionParam *createArrayFromScriptFunction(const ScriptFunction * sf);

		~ScriptFunctionParam();
	private:
		void init();
		void copy( const ScriptFunctionParam & _function_param);
	};
}
