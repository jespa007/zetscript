/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
#pragma once

#include "common.h"

#define TYPE_SCRIPT_VARIABLE 						"__ScriptObject__"
#define ZS_IDX_SCRIPTVAR_PROPERTY_IS_THIS			-1

namespace zetscript{

class ScriptClass;
	class ScriptClass;
	class ZetScript;
	class VirtualMachine;
	class ScriptClassFactory;
	class  ScriptObject{

	public:

		//----------------------
		// MEM MANNAGER RELATED
		InfoSharedPointerNode 	* ptr_shared_pointer_node;
		ClassTypeIdx	 		idx_class;
		ScriptFunction 		*	info_function_new;
		Instruction 		*	instruction_new;
		bool 					was_created_by_constructor;

		//----------------------

		// public vars ...
		//std::string str_value;
		void *value;

		// Construct ...
		ScriptObject();
		ScriptObject(ZetScript	*_zs);

		/**
		 * info_registered_class: scriptclass info
		 * _c_object: pass C object reference (is not delete_c_object by default)
		 */
		void init(ScriptClass *info_registered_class, void *  _c_object);


		bool isCreatedByContructor();
		bool setIdxClass(unsigned char idx);
		bool itHasSetMetamethod();
		void setDelete_C_ObjectOnDestroy(bool _delete_on_destroy);

		StackElement * addProperty(
				const std::string & symbol_value
				//, const ScriptFunction *info_function
				//, Instruction *src_instruction
				,std::string & error
				,StackElement * stk_element = NULL
				,int * idx_stk_element = NULL

		);


		StackElement *		newSlot();

		Symbol *		getSymbolMemberByIdx(int idx_symbol_member);
		int			   	getPropertyIdx(const std::string & varname);
		StackElement * 	getProperty(const std::string & varname, int * idx=NULL);
		StackElement * 	getProperty(short idx);
		const char *getSymbolNameFromPropertyPtr(StackElement *stk);

		bool eraseProperty(const std::string & symbol_value, const ScriptFunction *info_function=NULL);

		zs_vector * getAllProperties();
		StackElement *getUserProperty(int idx);
		StackElement *getThisProperty();

		void * getNativeObject();
		bool isNativeObject();
		ScriptClass *getNativeClass();

		ScriptFunction *getConstructorFunction();
		StackElement **getStkProperties(){return (StackElement **)&stk_properties->items[this->idx_start_user_properties];}
		zs_int size(){return lenght_user_properties;}


		ScriptClass *getScriptClass();
		const std::string & getClassName();

		const std::string & getNativePointerClassName();

		virtual std::string toString();
		virtual bool initSharedPtr();
		virtual bool unrefSharedPtr(int _idx_current_call);
		ZetScript      * getZetScript() { return zs;}

		virtual bool destroy();
		virtual ~ScriptObject();

	protected:

		StackElement this_variable;
		//StackElement * exist(const char *c);
		ZetScript      *zs;
		VirtualMachine	*virtual_machine;
		ScriptClassFactory *script_class_factory;

		/**
		 * This variable tells whether is pointer function or not.
		 */
		ScriptClass *registered_class_info;
		ScriptClass *c_scriptclass_info;
		bool delete_c_object;

		virtual void setup();
		StackElement *popUserProperty();
	private:

		void * created_object;
		void * c_object;
		zs_vector			*	stk_properties; // vector of stack elements
		zs_map				*	map_property_keys; // to search faster each property by its name
		zs_int lenght_user_properties;
		int idx_start_user_properties;


		//std::string aux_string;
		//zs_vector * stk_properties_built_in; // std::vector<FunctionSymbol>
		//zs_map	  *	properties_keys_built_in; // to search faster each property by its name

		void createSymbols(ScriptClass *irv);
		bool eraseProperty(short idx);
		StackElement * addPropertyBuiltIn(
				const std::string & symbol_value
		);

	};

}
