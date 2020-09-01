/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
#pragma once

#include "common.h"

#define TYPE_SCRIPT_VARIABLE "__ScriptVar__"

namespace zetscript{

class ScriptClass;
	class ScriptClass;
	class ZetScript;
	class VirtualMachine;
	class ScriptClassFactory;
	class  ScriptVar{

	public:

		//----------------------
		// MEM MANNAGER RELATED
		PInfoSharedPointerNode 	ptr_shared_pointer_node;
		unsigned char	 		idx_class;
		ScriptFunction 		*	info_function_new;
		Instruction 		*	instruction_new;
		bool 					was_created_by_constructor;

		//----------------------

		// public vars ...
		std::string str_value;
		void *value;

		// Construct ...
		ScriptVar();
		ScriptVar(ZetScript	*_zs);

		/**
		 * info_registered_class: scriptclass info
		 * _c_object: pass C object reference (is not delete_c_object by default)
		 */
		void init(ScriptClass *info_registered_class, void *  _c_object);


		bool isCreatedByContructor();
		bool setIdxClass(unsigned char idx);
		bool itHasSetMetamethod();
		void setDelete_C_ObjectOnDestroy(bool _delete_on_destroy);

		virtual StackElement * addProperty(
				const std::string & symbol_value
				, const ScriptFunction *info_function=NULL
				, Instruction *src_instruction = NULL
				, StackElement * sv=NULL
		);


		StackElement *		newSlot();


		int			   getPropertyIdx(const std::string & varname);
		StackElement * getProperty(const std::string & varname);
		StackElement * getProperty(short idx);
		const char *getSymbolNameFromPropertyPtr(StackElement *stk);

		void eraseProperty(const std::string & symbol_value, const ScriptFunction *info_function=NULL);

		zs_vector * getProperties();

		void * getNativeObject();
		bool isNativeObject();
		ScriptClass *getNativeClass();

		ScriptFunction *getConstructorFunction();


		const std::string & getClassName();

		const std::string & getNativePointerClassName();

		virtual std::string * toString();
		virtual void initSharedPtr(bool is_assigned=false);
		virtual void unrefSharedPtr();

		virtual void destroy();
		virtual ~ScriptVar();

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
		zs_map				*	properties_keys; // to search faster each property by its name
		int lenght_user_properties;
		short idx_start_user_properties;


		std::string aux_string;
		//zs_vector * stk_properties_built_in; // std::vector<FunctionSymbol>
		//zs_map	  *	properties_keys_built_in; // to search faster each property by its name

		void createSymbols(ScriptClass *irv);
		void eraseProperty(short idx, bool remove_vector=false);
		StackElement * addPropertyBuiltIn(
				const std::string & symbol_value
		);

	};

}
