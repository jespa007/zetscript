/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
#pragma once

#include "../common.h"


#define TYPE_SCRIPT_VARIABLE "__ScriptVar__"

namespace zetscript{

class CScriptClass;
	class CScriptClass;
	class CZetScript;
	class CVirtualMachine;

	class  CScriptVariable{//: public CVariable{

	public:

		//----------------------
		// MEM MANNAGER RELATED
		//bool deallocatable;
		//int idx_shared_ptr;
		PInfoSharedPointerNode ptr_shared_pointer_node;
		unsigned char idx_class;
		CScriptFunction *info_function_new;
		OpCodeInstruction 	*instruction_new;
		bool was_created_by_constructor;
		std::vector<StackElement> variable;

		//----------------------

		// public vars ...
		std::string m_strValue;
		void *m_value;

		// Construct ...
		CScriptVariable();
		CScriptVariable(CZetScript	*_zs);


		/**
		 * info_registered_class: scriptclass info
		 * _c_object: pass C object reference (is not delete_c_object by default)
		 */
		void init(CScriptClass *info_registered_class, void *  _c_object);


		bool isCreatedByContructor();
		bool setIdxClass(unsigned char idx);
		bool itHasSetMetamethod();
		void setDelete_C_ObjectOnDestroy(bool _delete_on_destroy);

		virtual StackElement * addVariableSymbol(const std::string & symbol_value, const CScriptFunction *info_function=NULL, OpCodeInstruction *src_instruction = NULL, StackElement * sv=NULL);
		StackElement * getVariableSymbol(const std::string & varname);
		StackElement * getVariableSymbolByIndex(unsigned int idx);
		bool removeVariableSymbolByName(const std::string & symbol_value, const CScriptFunction *info_function=NULL);
		bool removeVariableSymbolByIndex(unsigned int idx, bool remove_vector=false);
		std::vector<StackElement> * getVectorVariable();


		virtual FunctionSymbol * addFunctionSymbol(const std::string & symbol_value,const CScriptFunction *irv, bool ignore_duplicates=false);
		FunctionSymbol * getIdxScriptFunctionObjectByClassFunctionName(const std::string & funname);

		FunctionSymbol * getFunctionSymbol(const std::string & varname);
		FunctionSymbol * getFunctionSymbolByIndex(unsigned int idx);
		std::vector <FunctionSymbol> * getVectorFunctionSymbol();

		void * get_C_Object();
		bool is_c_object();
		CScriptClass *get_C_Class();

		CScriptFunction *getConstructorFunction();


		const std::string & getClassName();

		const std::string & getPointer_C_ClassName();

		virtual std::string * toString();
		virtual bool initSharedPtr(bool is_assigned=false);
		virtual bool unrefSharedPtr();

		intptr_t get_C_StructPtr();

		virtual void destroy();
		virtual ~CScriptVariable();

	protected:


		StackElement this_variable;
		StackElement * exist(const char *c);
		CZetScript      *zs;
		CVirtualMachine	*virtual_machine;

		/**
		 * This variable tells whether is pointer function or not.
		 */
		CScriptClass *registered_class_info;
		CScriptClass *c_scriptclass_info;
		bool delete_c_object;



		virtual void setup();

		//std::vector<FunctionSymbol> m_variableSymbol;

		std::vector<std::string> 		  m_variableKey;

	private:


		void * created_object;
		void * c_object;

		std::string aux_string;

		void createSymbols(CScriptClass *irv);

		std::vector<FunctionSymbol> m_functionSymbol;




	};

}
