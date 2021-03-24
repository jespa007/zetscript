/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
#pragma once

namespace zetscript{

/**
 * Stores the basic information to build a object through built AST structure
 */
	struct MemberAttribute{
	public:
		zs_vector setters; // setter that contains a list of script functions C++
		 ScriptFunction *getter; // getter

		 MemberAttribute(){
			 getter= NULL;
		}

		~MemberAttribute(){

		}
	};

	class ScriptFunctionFactory;
	class  ScriptClass{

	public:

		int				idx_starting_this_members;
		int 			idx_class; 	// registered class idx

		Symbol 			symbol_class;		// info symbol class

		//zs_vector *symbol_members_static; // register a collection of symbols to be deleted at the end...
		zs_vector *symbol_members; // a list of pre-registered C symbols to be added as stack element properties when class is instanced through scriptvar ( see ScriptObjectObject::createSymbols)
		zs_vector *symbol_members_built_in; // it has static/const and internal of symbols. Only is destroyed when deletes the class...
		ScriptFunction	*sf_field_initializer;
		//zs_vector *function_members; // a list of function members (script as well as registered native functions) to be registered on create any scriptvar, see ScriptObjectObject::createSymbols)

		//------------- VARIABLES STRUCT ---------------
		int							idx_function_member_constructor;

		void								* 	c_constructor;
		void 								*	c_destructor;
		bool								static_constructor_destructor;
		std::string 							str_class_ptr_type; // type_id().name();
		zs_vector						   	*   idx_base_classes; // list of idx of classes base


		//zs_vector 			*metamethod_operator[BYTE_CODE_METAMETHOD_MAX]; // overrided metamethod

		 ScriptClass(ZetScript *_zs,short _idx_class,Symbol *_symbol_class);

		//---------------------------------------------------
		// VARIABLES
		Symbol				* 	registerMemberVariable(
			std::string & error
			,const std::string & file
			,short line
			,const std::string & symbol_name
			,unsigned short symbol_properties
		);

		Symbol				* 	registerNativeMemberVariable(
			std::string & error
			,const std::string & file
			,short line
			,const std::string & symbol_name
			,const std::string & str_native_type
			,zs_int ref_ptr // it's the offset from pointer or a pointer directly
			,unsigned short symbol_properties
		);

		//---------------------------------------------------
		// ATTRIBUTES
		Symbol 				*	registerMemberAttribute(
				 std::string & error
				,const std::string & file
				,short line
				,const std::string & attrib_name
		);

		Symbol				* 	registerNativeMemberAttributeSetter(
			std::string & error
			,const std::string & file
			,short line
			,const std::string & attribute_name
			, ScriptFunctionArg arg_value
			,zs_int ref_ptr // it's the offset from pointer or a pointer directly
			,unsigned short symbol_properties=0
		);

		Symbol				* 	registerNativeMemberAttributeGetter(
			std::string & error
			,const std::string & file
			,short line
			,const std::string & attribute_name
			, int idx_return_type
			,zs_int ref_ptr // it's the offset from pointer or a pointer directly
			,unsigned short symbol_properties=0
		);

		Symbol				* 	registerMemberAttributeSetter(
			std::string & error
			,const std::string & file
			,short line
			,const std::string & attribute_name
			,ScriptFunction *sf // it's the offset from pointer or a pointer directly
		);

		Symbol				* 	registerMemberAttributeGetter(
			std::string & error
			,const std::string & file
			,short line
			,const std::string & attribute_name
			,ScriptFunction *sf // it's the offset from pointer or a pointer directly
		);
		//---------------------------------------------------
		// FUNCTIONS

		Symbol *	getSuperFunctionSymbol(Symbol *symbol);
		unsigned 	getNumNativeFunctions(const std::string & function_name);

		Symbol				* 	registerMemberFunction(
				std::string & error
				,const std::string & file
				, short line
				,const std::string & function_name
				, std::vector<ScriptFunctionArg> args={}
				, unsigned short properties=0

		);

		Symbol				* 	registerNativeMemberFunction(
				 std::string & error
				,const std::string & file
				, short line
				,const std::string & function_name
				, std::vector<ScriptFunctionArg> args={}
				, int idx_return_type=ZS_IDX_UNDEFINED
				,zs_int ref_ptr=0
				, unsigned short properties=0

		);

		//---------------------------------------------------

		Symbol *    getSymbol(const std::string & symbol_name, char n_params=NO_PARAMS_SYMBOL_ONLY, bool include_inherited_symbols=true);

		//Symbol				* 	getMemberFunction(const std::string & function_name, unsigned int n_args);
		bool isNativeClass();
		bool isNativeSingletonClass();

		virtual ~ScriptClass();

	private:
		ZetScript 				*zs;
		ScriptFunctionFactory 	*script_function_factory;
		ScriptClassFactory 		*script_class_factory;
		ScopeFactory 			*scope_factory;	// reference scope_factory



		Symbol				* 	registerInternalMemberVariable(
			std::string & error
			,const std::string & file
			, short line
			,const std::string & symbol_name
			, unsigned short properties
			,const std::string & str_native_type=""
			,zs_int ref_ptr=0 // it's the offset from pointer or a pointer directly
		);

		Symbol				* 	registerInternalMemberFunction(
		      std::string & error
			, const std::string & file
			, short line
			,const std::string & function_name
			, std::vector<ScriptFunctionArg> args
			, unsigned short properties=0
			, int idx_return_type=ZS_IDX_UNDEFINED
			,zs_int ref_ptr=0
		);


		//zs_map *num_native_functions;
	};
}


