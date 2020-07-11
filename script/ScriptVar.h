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
	class  ScriptVar{//: public CVariable{

	public:

		//----------------------
		// MEM MANNAGER RELATED
		//bool deallocatable;
		//int idx_shared_ptr;
		PInfoSharedPointerNode 	ptr_shared_pointer_node;
		unsigned char	 		idx_class;
		ScriptFunction *		info_function_new;
		Instruction 	*		instruction_new;
		bool 					was_created_by_constructor;
		std::vector<StackElement> variable;

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
		void Init(ScriptClass *info_registered_class, void *  _c_object);


		bool isCreatedByContructor();
		bool setIdxClass(unsigned char idx);
		bool itHasSetMetamethod();
		void setDelete_C_ObjectOnDestroy(bool _delete_on_destroy);

		virtual StackElement * addVariableSymbol(const std::string & symbol_value, const ScriptFunction *info_function=NULL, Instruction *src_instruction = NULL, StackElement * sv=NULL);
		StackElement * getVariableSymbol(const std::string & varname);
		StackElement * getVariableSymbolByIndex(int idx);
		bool removeVariableSymbolByName(const std::string & symbol_value, const ScriptFunction *info_function=NULL);
		bool removeVariableSymbolByIndex(unsigned int idx, bool remove_vector=false);
		std::vector<StackElement> * getVectorVariable();


		virtual FunctionSymbol * addFunctionSymbol(const std::string & symbol_value,const ScriptFunction *irv, bool ignore_duplicates=false);
		FunctionSymbol * getIdxScriptFunctionObjectByClassFunctionName(const std::string & funname);

		FunctionSymbol * getFunctionSymbol(const std::string & varname);
		FunctionSymbol * getFunctionSymbolByIndex(unsigned int idx);
		std::vector <FunctionSymbol> * getVectorFunctionSymbol();

		void * Get_C_Object();
		bool Is_C_Object();
		ScriptClass *Get_C_Class();

		ScriptFunction *getConstructorFunction();


		const std::string & getClassName();

		const std::string & getPointer_C_ClassName();

		virtual std::string * toString();
		virtual bool initSharedPtr(bool is_assigned=false);
		virtual bool unrefSharedPtr();

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

		//std::vector<FunctionSymbol> m_variableSymbol;

		std::vector<std::string> 		  variable_key;

	private:


		void * created_object;
		void * c_object;

		std::string aux_string;
		std::vector<FunctionSymbol> function_symbol;

		void CreateSymbols(ScriptClass *irv);




	};

}
