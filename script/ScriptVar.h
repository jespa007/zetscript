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
		zs_vector			*	stk_properties; // vector of stack elements
		zs_map				*	properties_keys; // to search faster each property by its name

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
		StackElement * getProperty(const std::string & varname);
		StackElement * getProperty(short idx);
		void eraseProperty(const std::string & symbol_value, const ScriptFunction *info_function=NULL);

		zs_vector * getProperties();


		virtual FunctionSymbol * addFunction(
				const std::string & function_name
				,const ScriptFunction *irv
				, bool ignore_duplicates=false
		);

		//FunctionSymbol * getIdxScriptFunctionObjectByClassFunctionName(const std::string & funname);

		FunctionSymbol 	* getFunction(const std::string & varname);
		FunctionSymbol 	* getFunction(unsigned int idx);
		zs_vector			* getFunctions();

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
	private:

		void * created_object;
		void * c_object;

		std::string aux_string;
		zs_vector * functions; // std::vector<FunctionSymbol>

		void createSymbols(ScriptClass *irv);
		void eraseProperty(short idx, bool remove_vector=false);

	};

}
