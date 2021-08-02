#pragma once

namespace zetscript{

	typedef enum{
		 MSK_SCRIPT_FUNCTION_ARG_PROPERTY_BY_REF	=0x1<<0
		,MSK_SCRIPT_FUNCTION_ARG_PROPERTY_VAR_ARGS	=0x1<<1
	} ScriptFunctionArgProperty;

	struct ScriptFunctionArg{
		int idx_type;
		std::string arg_name; //arg c++ type or arg name
		int line;
		uint16_t properties;
		StackElement default_var_value; // constant int/float/bool/string or anonymous function that return an expression or other object

		ScriptFunctionArg();
		ScriptFunctionArg(std::string _arg_name);
		ScriptFunctionArg(int _idx_type, std::string _arg_name);
		ScriptFunctionArg( const ScriptFunctionArg & _function_param);
	};
}
