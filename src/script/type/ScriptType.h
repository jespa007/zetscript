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

		int 								idx_script_type; 	// registered type idx
		zs_string							str_script_type;		// info symbol type
		Scope							*	scope_script_type;
		zs_string 							str_script_type_ptr; // type_id().name();
		uint16_t							properties;

		//zs_vector *symbol_member_variables; // symbol_member_variable:  It can be property members or script member vars
		//zs_vector *symbol_member_functions; // symbol_member_functions: It can be static or member functions
		zs_vector<MemberProperty *>		*	allocated_member_properties;
		ScriptFunction					*	sf_field_initializer;
		MetamethodMembers					metamethod_members;
		int									idx_starting_this_member_functions;
		int									idx_starting_this_member_variables;

		//------------- VARIABLES STRUCT ---------------
		int									idx_function_member_constructor;

		void								* 	c_constructor;
		void 								*	c_destructor;

		zs_vector<zs_int>				   	*   idx_base_types; // list of idx of classes base

		 ScriptType(ZetScript *_zs
				 ,short _idx_script_type
				 , const zs_string & _name
				 , Scope *_scope
				 ,const char *  str_script_type_ptr=ZS_TYPE_NAME_SCRIPT_OBJECT
				 ,uint16_t _properties=0
		);

		 bool extendsFrom(short idx_script_type);

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

		Symbol				* 	registerStaticMemberPropertyGetter(
			 const zs_string & _property_name
			 ,ScriptFunctionParam **_params
			 ,int8_t _params_len
			, int _idx_return_type
			,zs_int _ref_ptr // it's the offset from pointer or a pointer directly
			,const char * _file=""
			,short _line=-1
		);

		Symbol				* 	registerMemberPropertyGetter(
			const zs_string & _property_name
			 ,ScriptFunctionParam **_params
			 ,int8_t _params_len
			, int _idx_return_type
			,zs_int _ref_ptr // it's the offset from pointer or a pointer directly
			,const char * _file=""
			,short _line=-1
		);

		Symbol				* 	registerMemberPropertyNeg(
			const zs_string & _property_name
			 ,ScriptFunctionParam **_params
			 ,int8_t _params_len
			, int _idx_return_type
			,zs_int _ref_ptr // it's the offset from pointer or a pointer directly
			,const char * _file=""
			,short _line=-1
		);

		Symbol				* 	registerMemberPropertyBwc(
			const zs_string & _property_name
			 ,ScriptFunctionParam **_params
			 ,int8_t _params_len
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
		void registerStaticMemberPropertyGetter(
			const zs_string & _property_name
			,F _ptr_function
			, const char *_registered_file=""
			,short _registered_line=-1
		);


		template <typename F>
		void registerStaticMemberFunction(
			const zs_string & _function_name
			,F _function_ptr
			, const char *_registered_file=""
			,short _registered_line=-1
		);

		template <typename F>
		void registerMemberFunction(
			const zs_string & _function_name
			,F _function_type
			, const char *_registered_file=""
			,short _registered_line=-1
		);

		//---------------------------------------------------

		Symbol *    getSymbol(const zs_string & symbol_name, int8_t n_params=ZS_NO_PARAMS_SYMBOL_ONLY, bool include_inherited_symbols=true);
		Symbol *    getSymbolVariableMember(const zs_string & symbol_name, bool include_inherited_symbols=true);
		Symbol *    getSymbolMemberFunction(const zs_string & symbol_name, int8_t n_params=ZS_NO_PARAMS_SYMBOL_ONLY, bool include_inherited_symbols=true);

		const char *getTypeName();

		void printListMemberFunctions();
		bool isNativeType();
		bool isNonInstantiable();

		virtual ~ScriptType();

	private:
		ZetScript 				*zs;
		ScriptFunctionFactory 	*script_function_factory;
		ScriptTypeFactory 		*script_type_factory;
		ScopeFactory 			*scope_factory;	// reference scope_factory

		ScriptType * 					getScriptType(short idx_script_type);
		short							getIdxScriptTypeFromTypeNamePtr(const char  * s);
	};
}

#include "ScriptType.tcc"

