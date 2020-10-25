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

		int				idx_starting_this_members;
		ClassTypeIdx 	idx_class; 	// registered class idx

		Symbol 			symbol;		// info symbol class


		zs_vector *symbol_members; // a list of pre-registered C symbols to be added as stack element properties when class is instanced through scriptvar ( see ScriptObject::createSymbols)
		zs_vector *symbol_members_built_in; // register a collection of symbols to be deleted at the end...
		//zs_vector *function_members; // a list of function members (script as well as registered native functions) to be registered on create any scriptvar, see ScriptObject::createSymbols)

		//------------- VARIABLES STRUCT ---------------
		unsigned char							idx_function_member_constructor;

		std::function<void * ()>			* 	c_constructor;
		std::function<void (void *  p)> 	*	c_destructor;
		std::string 							str_class_ptr_type; // type_id().name();
		zs_vector						   	*   idx_base_classes; // list of idx of classes base


		zs_vector 			*metamethod_operator[BYTE_CODE_METAMETHOD_MAX]; // overrided metamethod

		 ScriptClass(ZetScript *_zs,ClassTypeIdx _idx_class);

		Symbol				* 	registerNativeVariableMember(
				const std::string & file
				, short line
				,const std::string & var_name
				,const std::string & str_native_type
				,zs_int ref_ptr // it should pass reference always because is built-in
				, unsigned short properties
		);

		Symbol *	getSuperFunctionSymbol(Symbol *symbol);
		Symbol *    getSymbol(const std::string & symbol_name, char n_params=NO_PARAMS_SYMBOL_ONLY);
		unsigned 	getNumNativeFunctions(const std::string & function_name);

		Symbol				* 	registerMemberFunction(
				const std::string & file
				, short line
				,const std::string & function_name
				, std::vector<FunctionParam> args={}
				, int idx_return_type=ZS_IDX_UNDEFINED
				,zs_int ref_ptr=0
				, unsigned short properties=0
		);

		//Symbol				* 	getMemberFunction(const std::string & function_name, unsigned int n_args);
		bool isNativeClass();

		virtual ~ScriptClass();

	private:
		ZetScript 				*zs;
		ScriptFunctionFactory 	*script_function_factory;
		ScriptClassFactory 		*script_class_factory;
		ScopeFactory 			*scope_factory;	// reference scope_factory
		//zs_map *num_native_functions;
	};
}


