/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
#pragma once


#define	ZS_TYPE_PROPERTY_C_OBJECT_REF		0x1
#define ZS_TYPE_PROPERTY_NON_INSTANTIABLE	0x2	// cannot be instanced


namespace zetscript{

/**
 * Stores the basic information to build a object through built AST structure
 */

	class FunctionFactory;
	class  Type{

	public:

		TypeId						id; // registered type idx
		String							name;			// info symbol type
		Scope							*	scope;
		String 							native_name; 	// type_id().name();
		uint16_t							properties;

		Vector<MemberProperty *>		*	allocated_member_properties;
		Function					*	sf_field_initializer;
		MetamethodMembers					metamethod_members;
		int									idx_starting_this_member_functions;
		int									idx_starting_this_member_variables;

		//------------- VARIABLES STRUCT ---------------
		int									idx_function_member_constructor;

		void								* 	new_native_instance;
		void 								*	delete_native_instance;

		Vector<TypeId>				*   base_type_ids; // list of idx of classes base

		 Type(ZetScript *_zs
				 ,TypeId _id
				 , const String & _name
				 , Scope *_scope
				 ,const char *  native_name=TYPE_NAME_OBJECT
				 ,uint16_t _properties=0
		);

		 bool extendsFrom(TypeId _type_id);

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
			,TypeId _return_type_id
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
			,TypeId  _idx_return_type=TYPE_ID_INVALID
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

		const char *getTypeName();

		void printListMemberFunctions();
		bool isNativeType();
		bool isNonInstantiable();

		virtual ~Type();

	private:
		ZetScript 				*zs;
		FunctionFactory 	*script_function_factory;
		TypeFactory 		*type_factory;
		ScopeFactory 			*scope_factory;	// reference scope_factory

		Type * 			getType(TypeId _type_id);
		TypeId			getTypeIdFromTypeNamePtr(const char  * s);
	};
}

#include "core/type/Type.tcc"

