/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
#pragma once


#define	ZS_SCRIPT_TYPE_PROPERTY_C_OBJECT_REF		0x1
#define ZS_SCRIPT_TYPE_PROPERTY_NON_INSTANTIABLE	0x2	// cannot be instanced


namespace zetscript{

/**
 * Stores the basic information to build a object through built AST structure
 */

	class ScriptFunctionFactory;
	class  ScriptType{

	public:

		ScriptTypeId						id; // registered type idx
		zs_string							name;			// info symbol type
		Scope							*	scope;
		zs_string 							native_name; 	// type_id().name();
		uint16_t							properties;

		zs_vector<MemberProperty *>		*	allocated_member_properties;
		ScriptFunction					*	sf_field_initializer;
		MetamethodMembers					metamethod_members;
		int									idx_starting_this_member_functions;
		int									idx_starting_this_member_variables;

		//------------- VARIABLES STRUCT ---------------
		int									idx_function_member_constructor;

		void								* 	new_native_instance;
		void 								*	delete_native_instance;

		zs_vector<ScriptTypeId>				*   base_script_type_ids; // list of idx of classes base

		 ScriptType(ZetScript *_zs
				 ,ScriptTypeId _id
				 , const zs_string & _name
				 , Scope *_scope
				 ,const char *  native_name=SCRIPT_TYPE_NAME_SCRIPT_OBJECT
				 ,uint16_t _properties=0
		);

		 bool extendsFrom(ScriptTypeId _script_type_id);

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
			,const zs_string & _metamethod_name
			,ScriptFunctionParam **_params
			,int _params_len
			,uint16_t _function_properties
			,ScriptTypeId _return_script_type_id
			,zs_int _ref_ptr // it's the offset from pointer or a pointer directly
			,const char * _file=""
			,short _line=-1
		);

		template <typename F>
		void registerMemberPropertyMetamethod(
			const zs_string & _property_name
			,const zs_string & _metamethod_name
			,F _function_ptr
			, const char *_registered_file=""
			,short _registered_line=-1
		);

		template <typename F>
		void registerConstMemberProperty(
			 const zs_string & _property_name
			,F _function_type
			, const char *_registered_file=""
			,short _registered_line=-1
		);


		//---------------------------------------------------
		//
		// FUNCTIONS
		//
		unsigned 	getNumNativeFunctions(const zs_string & name);

		Symbol				* 	registerMemberFunction(
			 const zs_string & _function_name
			,ScriptFunctionParam **_params=NULL
			,int _params_len=0
			, uint16_t _function_properties=0
			,ScriptTypeId  _idx_return_type=SCRIPT_TYPE_ID_INVALID
			, zs_int _ref_ptr=0
			, const char * _file=""
			, short _line=-1

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

		Symbol *    getSymbol(const char * _symbol_name, int8_t n_params=ZS_NO_PARAMS_SYMBOL_ONLY, bool include_inherited_symbols=true);
		Symbol *    getSymbolVariableMember(const char * _symbol_name, bool include_inherited_symbols=true);
		Symbol *    getSymbolMemberFunction(const char * _symbol_name, int8_t n_params=ZS_NO_PARAMS_SYMBOL_ONLY, bool include_inherited_symbols=true);

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

		ScriptType * 			getScriptType(ScriptTypeId _script_type_id);
		ScriptTypeId			getScriptTypeIdFromTypeNamePtr(const char  * s);
	};
}

#include <script/script_type/ScriptType.tcc>

