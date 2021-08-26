/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
#pragma once

#define ZS_PREFIX_SYMBOL_NAME_SETTER	"_set@"
#define ZS_PREFIX_SYMBOL_NAME_GETTER	"_get@"
#define ZS_PREFIX_SYMBOL_NAME_POST_INC	"_post_inc@"
#define ZS_PREFIX_SYMBOL_NAME_POST_DEC	"_post_dec@"
#define ZS_PREFIX_SYMBOL_NAME_PRE_INC	"_pre_inc@"
#define ZS_PREFIX_SYMBOL_NAME_PRE_DEC	"_pre_dec@"

namespace zetscript{

/**
 * Stores the basic information to build a object through built AST structure
 */

	class ScriptFunctionFactory;
	class  ScriptClass{

	public:

		int 			idx_class; 	// registered class idx

		Symbol 			symbol_class;		// info symbol class

		zs_vector *symbol_member_variables; // symbol_member_variable:  It can be attribute members or script member vars
		zs_vector *symbol_member_functions; // symbol_member_functions: It can be static or member functions
		zs_vector *symbol_member_variables_allocated;
		ScriptFunction	*sf_field_initializer;
		MemberAttribute			*setter_getter;
		int						idx_starting_this_member_functions;
		int						idx_starting_this_member_variables;


		//------------- VARIABLES STRUCT ---------------
		int							idx_function_member_constructor;

		void								* 	c_constructor;
		void 								*	c_destructor;
		std::string 							str_class_ptr_type; // type_id().name();
		zs_vector						   	*   idx_base_classes; // list of idx of classes base



		 ScriptClass(ZetScript *_zs,short _idx_class,Symbol *_symbol_class);

		//---------------------------------------------------
		// SCRIPT VARIABLES
		Symbol				* 	registerMemberVariable(
			 const std::string & symbol_name
			,unsigned short symbol_properties
			,const char * file=""
			,short line=-1

		);

		template < typename R>
		void registerNativeMemberVariableStaticConst(
			const std::string & var_name
			, const R var_pointer
			, const char *registered_file=""
			,short registered_line=-1
		);


		Symbol				* 	registerNativeMemberVariable(
			 const std::string & symbol_name
			,const std::string & str_native_type
			,zs_int ref_ptr // it's the offset from pointer or a pointer directly
			,unsigned short symbol_properties
			,const char * file=""
			,short line=-1

		);

		//---------------------------------------------------
		//
		// ATTRIBUTES
		//
		Symbol 				*	registerMemberAttribute(
				const std::string & attrib_name
				,const char * file=""
				,short line=-1

		);
		
		//---------------
		// SETTER
		Symbol				* 	registerNativeMemberAttributeSetter(
			const std::string & attribute_name
			, std::vector<ScriptFunctionArg> arg_value
			,zs_int ref_ptr // it's the offset from pointer or a pointer directly
			,unsigned short symbol_getter_function_properties
			,const char * file=""
			,short line=-1

		);

		/*
		 * register C setter
		 */
		template <typename F>
		void registerNativeMemberAttributeSetter(
				const std::string & attrib_name
				,F ptr_function
				, const char *registered_file=""
				,short registered_line=-1
		);

		//---------------
		// GETTER
		Symbol				* 	registerNativeMemberAttributeGetter(
			const std::string & attribute_name
			,std::vector<ScriptFunctionArg> arg_value
			, int idx_return_type
			,zs_int ref_ptr // it's the offset from pointer or a pointer directly
			,unsigned short symbol_getter_function_properties
			,const char * file=""
			,short line=-1
		);


		Symbol				* 	registerNativeMemberAttributePostIncrement(
			const std::string & attribute_name
			,std::vector<ScriptFunctionArg> arg_value
			, int idx_return_type
			,zs_int ref_ptr // it's the offset from pointer or a pointer directly
			,unsigned short symbol_post_inc_function_properties
			,const char * file=""
			,short line=-1
		);

		Symbol				* 	registerNativeMemberAttributePostDecrement(
			const std::string & attribute_name
			,std::vector<ScriptFunctionArg> arg_value
			, int idx_return_type
			,zs_int ref_ptr // it's the offset from pointer or a pointer directly
			,unsigned short symbol_post_dec_function_properties
			,const char * file=""
			,short line=-1
		);

		Symbol				* 	registerNativeMemberAttributePreIncrement(
			const std::string & attribute_name
			,std::vector<ScriptFunctionArg> arg_value
			, int idx_return_type
			,zs_int ref_ptr // it's the offset from pointer or a pointer directly
			,unsigned short symbol_pre_inc_function_properties
			,const char * file=""
			,short line=-1
		);

		Symbol				* 	registerNativeMemberAttributePreDecrement(
			const std::string & attribute_name
			,std::vector<ScriptFunctionArg> arg_value
			, int idx_return_type
			,zs_int ref_ptr // it's the offset from pointer or a pointer directly
			,unsigned short symbol_pre_dec_function_properties
			,const char * file=""
			,short line=-1

		);
		/*
		 * register C getter
		 */
		template <typename F>
		void registerNativeMemberAttributeGetter(
				const std::string & attrib_name
			,F ptr_function
			, const char *registered_file
			,short registered_line
		);

		template <typename F>
		void registerNativeMemberAttributePostIncrement(
				const std::string & attrib_name
			,F ptr_function
			, const char *registered_file
			,short registered_line
		);

		template <typename F>
		void registerNativeMemberAttributePostDecrement(
				const std::string & attrib_name
			,F ptr_function
			, const char *registered_file
			,short registered_line
		);

		template <typename F>
		void registerNativeMemberAttributePreIncrement(
				const std::string & attrib_name
			,F ptr_function
			, const char *registered_file
			,short registered_line
		);

		template <typename F>
		void registerNativeMemberAttributePreDecrement(
				const std::string & attrib_name
			,F ptr_function
			, const char *registered_file
			,short registered_line
		);

		//---------------------------------------------------
		// FUNCTIONS

		Symbol *	getSuperFunctionSymbol(Symbol *symbol);
		unsigned 	getNumNativeFunctions(const std::string & function_name);

		Symbol				* 	registerMemberFunction(
				 const std::string & function_name
				, std::vector<ScriptFunctionArg> args={}
				, unsigned short properties=0
				,const char * file = ""
				, short line=-1
		);

		Symbol				* 	registerNativeMemberFunction(
				 const std::string & function_name
				,std::vector<ScriptFunctionArg> args={}
				,int idx_return_type=ZS_IDX_UNDEFINED
				,zs_int ref_ptr=0
				,unsigned short properties=0
				,const char * file=""
				,short line=-1
		);

		template <typename F>
		void registerNativeMemberFunctionStatic(
				const std::string & function_name
				,F function_ptr
				, const char *registered_file=""
				,short registered_line=-1
		);

		template <typename F>
		void registerNativeMemberFunction(
				const std::string & function_name
				,F function_type
				, const char *registered_file=""
				,short registered_line=-1
		);

		//---------------------------------------------------

		Symbol *    getSymbol(const std::string & symbol_name, char n_params=NO_PARAMS_SYMBOL_ONLY, bool include_inherited_symbols=true);
		Symbol *    getSymbolVariableMember(const std::string & symbol_name, bool include_inherited_symbols=true);
		Symbol *    getSymbolMemberFunction(const std::string & symbol_name, char n_params=NO_PARAMS_SYMBOL_ONLY, bool include_inherited_symbols=true);


		bool isNativeClass();
		bool isNativeSingletonClass();

		virtual ~ScriptClass();

	private:
		ZetScript 				*zs;
		ScriptFunctionFactory 	*script_function_factory;
		ScriptClassFactory 		*script_class_factory;
		ScopeFactory 			*scope_factory;	// reference scope_factory

		Symbol				* 	registerInternalMemberVariable(
			const std::string & symbol_name
			, unsigned short properties
			,const std::string & str_native_type=""
			,zs_int ref_ptr=0 // it's the offset from pointer or a pointer directly
			,const char *file=""
			, short line=-1

		);

		Symbol				* 	registerInternalMemberFunction(
			 const std::string & function_name
			, std::vector<ScriptFunctionArg> args
			, unsigned short properties=0
			, int idx_return_type=ZS_IDX_UNDEFINED
			,zs_int ref_ptr=0
			, const char * file=""
			, short line=-1

		);

		template <typename F>
		int getNativeMemberFunctionRetArgsTypes(
				const std::string & function_name
				,F function_ptr
				,std::string & return_type
				,std::vector<ScriptFunctionArg> & arg_info
		);

		ScriptClass * 					getScriptClass(short idx_class);
		short							getIdxClassFromItsNativeType(const std::string & s);
		ScriptClass * 					getScriptClassByNativeClassPtr(const std::string & class_type);
	};
}

#include "ScriptClass.tcc"

