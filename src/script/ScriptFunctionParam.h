#pragma once

namespace zetscript{

	typedef enum{
		 MSK_SCRIPT_FUNCTION_ARG_PROPERTY_BY_REF	=0x1<<0
		,MSK_SCRIPT_FUNCTION_ARG_PROPERTY_VAR_ARGS	=0x1<<1
	} ScriptFunctionArgProperty;

	struct ScriptFunctionParam{
		int idx_type;
		zs_string param_name; //arg c++ type or arg name
		int line;
		uint16_t properties;
		StackElement default_param_value; // constant int/float/bool/string or anonymous function that return an expression or other object

		ScriptFunctionParam();
		ScriptFunctionParam(zs_string _param_name);
		ScriptFunctionParam(int _idx_type, zs_string _param_name);
		ScriptFunctionParam( const ScriptFunctionParam & _function_param);
	};
}
