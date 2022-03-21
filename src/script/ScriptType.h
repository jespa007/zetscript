/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
#pragma once


#define	SCRIPT_TYPE_PROPERTY_C_OBJECT_REF		0x1
#define SCRIPT_TYPE_PROPERTY_NON_INSTANTIABLE	0x2	// cannot be instanced


namespace zetscript{

/**
 * Stores the basic information to build a object through built AST structure
 */

	class ScriptFunctionFactory;
	class  ScriptType{

	public:

		int 			idx_script_type; 	// registered class idx
		zs_string 		script_type_name;		// info symbol class
		Scope			*script_type_scope;
		const char 		*script_type_name_ptr; // type_id().name();
		uint16_t		properties;

		//zs_vector *symbol_member_variables; // symbol_member_variable:  It can be property members or script member vars
		//zs_vector *symbol_member_functions; // symbol_member_functions: It can be static or member functions
		zs_vector 				*allocated_member_properties;
		ScriptFunction			*sf_field_initializer;
		MetamethodMembers		metamethod_members;
		int						idx_starting_this_member_functions;
		int						idx_starting_this_member_variables;


		//------------- VARIABLES STRUCT ---------------
		int									idx_function_member_constructor;

		void								* 	c_constructor;
		void 								*	c_destructor;

		zs_vector						   	*   idx_base_types; // list of idx of classes base

		 ScriptType(ZetScript *_zs
				 ,short _idx_script_type
				 , zs_string _name
				 , Scope *_scope
				 ,const char *  script_type_name_ptr=TYPE_SCRIPT_VARIABLE
				 ,uint16_t _properties=0
		);

		 bool isDerivedFrom(short idx_script_type);

		//---------------------------------------------------
		//
		// MEMBER VARIABLES
		//
		Symbol				* 	registerMemberVariable(
			 const zs_string & _symbol_name
			,unsigned short _symbol_properties
			,const char * _file=""
			,short _line=-1

		);

		template < typename R>
		void bindMemberVariableStaticConst(
			const zs_string & _var_name
			, const R _var_pointer
			, const char *_registered_file=""
			,short _registered_line=-1
		);


		Symbol				* 	registerMemberVariable(
			 const zs_string & _symbol_name
			,const zs_string & _str_native_type
			,zs_int _ref_ptr // it's the offset from pointer or a pointer directly
			,unsigned short _symbol_properties
			,const char * _file=""
			,short _line=-1

		);
		//---------------------------------------------------
		//
		// MEMBER PROPERTIES
		//
		Symbol 				*	registerMemberProperty(
				const zs_string & _property_name
				,const char * _file=""
				,short _line=-1

		);

		Symbol				* 	registerMemberPropertyMetamethod(
			const zs_string & _property_name
			,ByteCodeMetamethod _byte_code_metamethod
			,ScriptFunctionParam **_params
			,int _params_len
			,int _idx_return_type
			,zs_int _ref_ptr // it's the offset from pointer or a pointer directly
			,const char * _file=""
			,short _line=-1
		);

		Symbol				* 	registerMemberPropertyGetter(
			const zs_string & _property_name
			 ,ScriptFunctionParam **_params
			 ,char _params_len
			, int _idx_return_type
			,zs_int _ref_ptr // it's the offset from pointer or a pointer directly
			,const char * _file=""
			,short _line=-1
		);

		//---------------------------------------------------
		//
		// FUNCTIONS
		//
		unsigned 	getNumNativeFunctions(const zs_string & name_script_function);

		Symbol				* 	registerMemberFunction(
			 const zs_string & _function_name
			,ScriptFunctionParam **_params=NULL
			,int _params_len=0
			, unsigned short _function_properties=0
			, int _idx_return_type=ZS_IDX_UNDEFINED
			, zs_int _ref_ptr=0
			, const char * _file=""
			, short _line=-1

		);

		template <typename F>
		void bindMemberFunctionStatic(
				const zs_string & _function_name
				,F _function_ptr
				, const char *_registered_file=""
				,short _registered_line=-1
		);

		template <typename F>
		void bindMemberFunction(
				const zs_string & _function_name
				,F _function_type
				, const char *_registered_file=""
				,short _registered_line=-1
		);

		//---------------------------------------------------

		Symbol *    getSymbol(const zs_string & symbol_name, char n_params=NO_PARAMS_SYMBOL_ONLY, bool include_inherited_symbols=true);
		Symbol *    getSymbolVariableMember(const zs_string & symbol_name, bool include_inherited_symbols=true);
		Symbol *    getSymbolMemberFunction(const zs_string & symbol_name, char n_params=NO_PARAMS_SYMBOL_ONLY, bool include_inherited_symbols=true);

		const char *getTypeName();

		bool isNativeType();
		bool isNonInstantiable();

		virtual ~ScriptType();

	private:
		ZetScript 				*zs;
		ScriptFunctionFactory 	*script_function_factory;
		ScriptTypeFactory 		*script_type_factory;
		ScopeFactory 			*scope_factory;	// reference scope_factory

		Symbol				* 	registerInternalMemberVariable(
			const zs_string & symbol_name
			, unsigned short properties
			,const zs_string & str_native_type=""
			,zs_int ref_ptr=0 // it's the offset from pointer or a pointer directly
			,const char *file=""
			, short line=-1

		);





		ScriptType * 					getScriptType(short idx_script_type);
		short							getIdxScriptTypeFromTypeNamePtr(const char  * s);
	};
}

#include "ScriptType.tcc"

