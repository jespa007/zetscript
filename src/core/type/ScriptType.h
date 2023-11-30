/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
#pragma once


namespace zetscript{

	typedef enum{
		SCRIPT_TYPE_PROPERTY_NATIVE_OBJECT_REF=0x1
		,SCRIPT_TYPE_PROPERTY_NON_INSTANTIABLE=0x2	// cannot be instanced
	}ScriptTypeProperty;

/**
 * Stores the basic information to build a object through built AST structure
 */

	class ScriptFunctionsFactory;
	class  ScriptType{

	public:

		ScriptTypeId							id; // registered type idx
		String									name;			// info symbol type
		ScriptScope						*		scope;
			String 								native_name; 	// script_type_id().name();
		uint16_t								properties;

		Vector<MemberProperty *>		*		allocated_member_properties;
		ScriptFunction					*		sf_field_initializer;
		MetamethodMembers						metamethod_members;
		int										idx_starting_this_member_functions;
		int										idx_starting_this_member_variables;

		//------------- VARIABLES STRUCT ---------------
		int										idx_constructor_member_function;

		void								* 	new_native_instance;
		void 								*	delete_native_instance;

		Vector<ScriptTypeId>				*   base_script_type_ids; // list of idx of classes base

		 ScriptType(ZetScript *_zs
				 ,ScriptTypeId _id
				 , const String & _name
				 , ScriptScope *_scope
				 ,const char *  native_name=ZS_SCRIPT_TYPE_NAME_UNKNOWN
				 ,uint16_t _properties=0
		);

		 bool extendsFrom(ScriptTypeId _script_type_id);

		//---------------------------------------------------
		//
		// MEMBER VARIABLES
		//
		Symbol				* 	registerMemberVariable(
			 const String & _symbol_name
			,unsigned short _symbol_properties
			,const char * _file=""
			,short _line=-1

		);

		//---------------------------------------------------
		//
		// MEMBER PROPERTIES
		//
		Symbol 				*	registerMemberProperty(
				const String & _property_name
				,const char * _file=""
				,short _line=-1

		);

		Symbol				* 	registerMemberPropertyMetamethod(
			const String & _property_name
			,const String & _metamethod_name
			,FunctionParam **_params
			,int _params_len
			,uint16_t _function_properties
			,ScriptTypeId _return_script_type_id
			,zs_int _ref_ptr // it's the offset from pointer or a pointer directly
			,const char * _file=""
			,short _line=-1
		);

		template <typename F>
		void registerMemberPropertyMetamethod(
			const String & _property_name
			,const String & _metamethod_name
			,F _function_ptr
			, const char *_registered_file=""
			,short _registered_line=-1
		);

		template <typename F>
		void registerConstMemberProperty(
			 const String & _property_name
			,F _function_type
			, const char *_registered_file=""
			,short _registered_line=-1
		);


		//---------------------------------------------------
		//
		// FUNCTIONS
		//
		unsigned 	getNumNativeFunctions(const String & name);

		Symbol				* 	registerMemberFunction(
			 const String & _function_name
			,FunctionParam **_params=NULL
			,int _params_len=0
			, uint16_t _function_properties=0
			,ScriptTypeId  _idx_return_type=SCRIPT_TYPE_ID_INVALID
			, zs_int _ref_ptr=0
			, const char * _file=""
			, short _line=-1

		);

		template <typename F>
		void registerStaticMemberFunction(
			const String & _function_name
			,F _function_ptr
			, const char *_registered_file=""
			,short _registered_line=-1
		);

		template <typename F>
		void registerMemberFunction(
			const String & _function_name
			,F _function_type
			, const char *_registered_file=""
			,short _registered_line=-1
		);

		//---------------------------------------------------

		Symbol *    getSymbol(const char * _symbol_name, int8_t n_params=ZS_NO_PARAMS_SYMBOL_ONLY, bool include_inherited_symbols=true);
		Symbol *    getSymbolVariableMember(const char * _symbol_name, bool include_inherited_symbols=true);
		Symbol *    getSymbolMemberFunction(const char * _symbol_name, int8_t n_params=ZS_NO_PARAMS_SYMBOL_ONLY, bool include_inherited_symbols=true);

		const char *getScriptTypeName();

		void printListMemberFunctions();
		bool isNativeType();
		bool isNonInstantiable();

		virtual ~ScriptType();

	private:
		ZetScript 				*zs;
		ScriptFunctionsFactory 	*script_function_factory;
		ScriptTypesFactory 		*script_types_factory;
		ScriptScopesFactory 			*scope_factory;	// reference scope_factory

		ScriptType * 			getScriptType(ScriptTypeId _script_type_id);
		ScriptTypeId			getScriptTypeIdFromNamePtr(const char  * s);
	};
}

#include "core/type/ScriptType.tcc"

