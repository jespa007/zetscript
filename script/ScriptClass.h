/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
#pragma once

namespace zetscript{

/**
 * Stores the basic information to build a object through built AST structure
 */

	class ScriptFunctionFactory;
	class  ScriptClass{

	public:

		unsigned char idx_class; 	// registered class idx

		Symbol symbol; 				// info symbol class


		zs_vector *symbol_c_variable_members; // a list of pre-registered C symbols to be added as stack element properties when class is instanced through scriptvar ( see ScriptVar::createSymbols)
		zs_vector *function_members; // a list of function members to be registered on create any scriptvar, see ScriptVar::createSymbols)

		//------------- VARIABLES STRUCT ---------------
		char  idx_function_member_constructor;

		std::function<void * ()>			* 	c_constructor;
		std::function<void (void *  p)> 	*	c_destructor;
		std::string 							str_class_ptr_type; // type_id().name();
		zs_vector 							*   idx_base_classes; // list of idx of classes base


		zs_vector 			*metamethod_operator[BYTE_CODE_METAMETHOD_MAX]; // overrided metamethod

		 ScriptClass(ZetScript *_zs,unsigned char _idx_class);

		Symbol				* 	register_C_SymbolVariableMember(
				const std::string & file
				, short line
				,const std::string & var_name
				,const std::string & c_type
				,intptr_t ref_ptr=0
				, unsigned short symbol_properties=0
		);

		Symbol *	 get_C_SymbolVariableMember(const std::string & symbol_name);


		ScriptFunction				* 	registerFunctionMember(
				const std::string & file
				, short line
				,const std::string & function_name
				, std::vector<FunctionParam> args={}
				, int idx_return_type=ZS_IDX_UNDEFINED
				,intptr_t ref_ptr=0
				, unsigned short symbol_properties=0
		);

		ScriptFunction				* 	getFunctionMember(const std::string & function_name, unsigned int n_args);






		bool is_C_Class();

		virtual ~ScriptClass();

	private:
		ZetScript 				*zs;
		ScriptFunctionFactory 	*script_function_factory;
		ScriptClassFactory 		*script_class_factory;
		ScopeFactory 			*scope_factory;	// reference scope_factory
	};
}


