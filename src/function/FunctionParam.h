/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
#pragma once

namespace zetscript{

	typedef enum{
		 MSK_FUNCTION_ARG_PROPERTY_BY_REF	=0x1<<0
		,MSK_FUNCTION_ARG_PROPERTY_VAR_ARGS	=0x1<<1
	} FunctionArgProperty;

	class FunctionParam{
	public:
		TypeId type_id;
		String name; //arg c++ type or arg name
		int line;
		uint16_t properties;
		StackElement default_param_value; // constant int/float/bool/string or anonymous function that return an expression or other object

		FunctionParam();
		FunctionParam(const String & _name);
		FunctionParam(TypeId _type_id, const String & _name);
		FunctionParam( const FunctionParam & _function_param);

		FunctionParam & operator=( const FunctionParam & _function_param);

		static FunctionParam *createArrayFromArray(const Vector<FunctionParam *>  * _script_function_params);
		static FunctionParam *createArrayFromFunction(const Function * sf);

		~FunctionParam();
	private:
		void init();
		void copy( const FunctionParam & _function_param);
	};
}
