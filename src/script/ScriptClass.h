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

#define SCRIPT_CLASS_PROPERTY_C_OBJECT_REF	0x1

namespace zetscript{

/**
 * Stores the basic information to build a object through built AST structure
 */

	class ScriptFunctionFactory;
	class  ScriptClass{

	public:

		int 			idx_class; 	// registered class idx

		zs_string 		class_name;		// info symbol class
		Scope			*class_scope;
		const char 		*str_class_ptr_type; // type_id().name();
		uint16_t		properties;

		//zs_vector *symbol_member_variables; // symbol_member_variable:  It can be attribute members or script member vars
		//zs_vector *symbol_member_functions; // symbol_member_functions: It can be static or member functions
		zs_vector 		*allocated_member_attributes;
		ScriptFunction	*sf_field_initializer;
		MemberAttribute			*setter_getter;
		int						idx_starting_this_member_functions;
		int						idx_starting_this_member_variables;


		//------------- VARIABLES STRUCT ---------------
		int									idx_function_member_constructor;

		void								* 	c_constructor;
		void 								*	c_destructor;

		zs_vector						   	*   idx_base_classes; // list of idx of classes base




		 ScriptClass(ZetScript *_zs
				 ,short _idx_class
				 , zs_string _name
				 , Scope *_scope
				 ,zs_string  str_class_ptr_type=TYPE_SCRIPT_VARIABLE
				 ,uint16_t _properties=0
		);

		 bool isDerivedFrom(short idx_class);

		//---------------------------------------------------
		// SCRIPT VARIABLES
		Symbol				* 	registerMemberVariable(
			 const zs_string & symbol_name
			,unsigned short symbol_properties
			,const char * file=""
			,short line=-1

		);

		template < typename R>
		void registerNativeMemberVariableStaticConst(
			const zs_string & var_name
			, const R var_pointer
			, const char *registered_file=""
			,short registered_line=-1
		);


		Symbol				* 	registerNativeMemberVariable(
			 const zs_string & symbol_name
			,const zs_string & str_native_type
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
				const zs_string & attrib_name
				,const char * file=""
				,short line=-1

		);
		
		//---------------
		// SETTER
		Symbol				* 	registerNativeMemberAttributeSetter(
			const zs_string & attribute_name
			,ScriptFunctionParam *args
			,size_t args_len
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
				const zs_string & attrib_name
				,F ptr_function
				, const char *registered_file=""
				,short registered_line=-1
		);

		//---------------
		// GETTER
		Symbol				* 	registerNativeMemberAttributeGetter(
			const zs_string & attribute_name
			,ScriptFunctionParam *args
			,size_t args_len
			, int idx_return_type
			,zs_int ref_ptr // it's the offset from pointer or a pointer directly
			,unsigned short symbol_getter_function_properties
			,const char * file=""
			,short line=-1
		);


		Symbol				* 	registerNativeMemberAttributePostIncrement(
			const zs_string & attribute_name
			,ScriptFunctionParam *args
			,size_t args_len
			, int idx_return_type
			,zs_int ref_ptr // it's the offset from pointer or a pointer directly
			,unsigned short symbol_post_inc_function_properties
			,const char * file=""
			,short line=-1
		);

		Symbol				* 	registerNativeMemberAttributePostDecrement(
			const zs_string & attribute_name
			,ScriptFunctionParam *args
			,size_t args_len
			, int idx_return_type
			,zs_int ref_ptr // it's the offset from pointer or a pointer directly
			,unsigned short symbol_post_dec_function_properties
			,const char * file=""
			,short line=-1
		);

		Symbol				* 	registerNativeMemberAttributePreIncrement(
			const zs_string & attribute_name
			,ScriptFunctionParam *args
			,size_t args_len
			, int idx_return_type
			,zs_int ref_ptr // it's the offset from pointer or a pointer directly
			,unsigned short symbol_pre_inc_function_properties
			,const char * file=""
			,short line=-1
		);

		Symbol				* 	registerNativeMemberAttributePreDecrement(
			const zs_string & attribute_name
			,ScriptFunctionParam *args
			,size_t args_len
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
				const zs_string & attrib_name
			,F ptr_function
			, const char *registered_file
			,short registered_line
		);

		template <typename F>
		void registerNativeMemberAttributePostIncrement(
				const zs_string & attrib_name
			,F ptr_function
			, const char *registered_file
			,short registered_line
		);

		template <typename F>
		void registerNativeMemberAttributePostDecrement(
				const zs_string & attrib_name
			,F ptr_function
			, const char *registered_file
			,short registered_line
		);

		template <typename F>
		void registerNativeMemberAttributePreIncrement(
				const zs_string & attrib_name
			,F ptr_function
			, const char *registered_file
			,short registered_line
		);

		template <typename F>
		void registerNativeMemberAttributePreDecrement(
				const zs_string & attrib_name
			,F ptr_function
			, const char *registered_file
			,short registered_line
		);

		//---------------------------------------------------
		// FUNCTIONS
		unsigned 	getNumNativeFunctions(const zs_string & function_name);

		Symbol				* 	registerMemberFunction(
				 const zs_string & function_name
				,ScriptFunctionParam *args=NULL
				,size_t args_len=0
				, unsigned short properties=0
				,const char * file = ""
				, short line=-1
		);

		Symbol				* 	registerNativeMemberFunction(
				 const zs_string & function_name
				,ScriptFunctionParam *args=NULL
				,size_t args_len=0
				,int idx_return_type=ZS_IDX_UNDEFINED
				,zs_int ref_ptr=0
				,unsigned short properties=0
				,const char * file=""
				,short line=-1
		);

		template <typename F>
		void registerNativeMemberFunctionStatic(
				const zs_string & function_name
				,F function_ptr
				, const char *registered_file=""
				,short registered_line=-1
		);

		template <typename F>
		void registerNativeMemberFunction(
				const zs_string & function_name
				,F function_type
				, const char *registered_file=""
				,short registered_line=-1
		);

		//---------------------------------------------------

		Symbol *    getSymbol(const zs_string & symbol_name, char n_params=NO_PARAMS_SYMBOL_ONLY, bool include_inherited_symbols=true);
		Symbol *    getSymbolVariableMember(const zs_string & symbol_name, bool include_inherited_symbols=true);
		Symbol *    getSymbolMemberFunction(const zs_string & symbol_name, char n_params=NO_PARAMS_SYMBOL_ONLY, bool include_inherited_symbols=true);


		bool isNativeClass();
		bool isNativeSingletonClass();

		virtual ~ScriptClass();

	private:
		ZetScript 				*zs;
		ScriptFunctionFactory 	*script_function_factory;
		ScriptClassFactory 		*script_class_factory;
		ScopeFactory 			*scope_factory;	// reference scope_factory

		Symbol				* 	registerInternalMemberVariable(
			const zs_string & symbol_name
			, unsigned short properties
			,const zs_string & str_native_type=""
			,zs_int ref_ptr=0 // it's the offset from pointer or a pointer directly
			,const char *file=""
			, short line=-1

		);

		Symbol				* 	registerInternalMemberFunction(
			 const zs_string & function_name
			,ScriptFunctionParam *args
			,size_t args_len
			, unsigned short properties=0
			, int idx_return_type=ZS_IDX_UNDEFINED
			, zs_int ref_ptr=0
			, const char * file=""
			, short line=-1

		);

		template <typename F>
		int getNativeMemberFunctionRetArgsTypes(
				const zs_string & function_name
				,F function_ptr
				,const char * return_type
				,ScriptFunctionParam **args
				,size_t *args_len

		);

		ScriptClass * 					getScriptClass(short idx_class);
		short							getIdxClassFromItsNativeType(const zs_string & s);
		ScriptClass * 					getScriptClassByNativeClassPtr(const zs_string & class_type);
	};
}

#include "ScriptClass.tcc"

