/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
#pragma once

#include "../Common.h"


#define TYPE_SCRIPT_VARIABLE "__ScriptVar__"

namespace zetscript{

	class CScriptClass;

	class  CScriptVariable{//: public CVariable{
	protected:
		tStackElement this_variable;

		tStackElement * exist(const char *c);

	public:

		//----------------------
		// MEM MANNAGER RELATED
		//bool deallocatable;
		//int idx_shared_ptr;
		PInfoSharedPointerNode ptr_shared_pointer_node;
		unsigned char idxClass;
		CScriptFunction *info_function_new;
		tInstruction 	*instruction_new;
		bool was_created_by_constructor;
		vector<tStackElement> m_variable;
		//----------------------

		// public vars ...
		string m_strValue;
		void *m_value;

		// Construct ...
		CScriptVariable();

		/**
		 * info_registered_class: scriptclass info
		 * _c_object: pass C object reference (is not delete_c_object by default)
		 */
		void init(CScriptClass *info_registered_class, void *  _c_object);


		bool isCreatedByContructor();
		bool setIdxClass(unsigned char idx);
		bool itHasSetMetamethod();
		void setDelete_C_ObjectOnDestroy(bool _delete_on_destroy);

		virtual tStackElement * addVariableSymbol(const string & symbol_value, CScriptFunction *info_function=NULL, tInstruction *src_instruction = NULL, tStackElement * sv=NULL);
		tStackElement * getVariableSymbol(const string & varname,bool only_var_name=false);
		tStackElement * getVariableSymbolByIndex(unsigned int idx);
		bool removeVariableSymbolByName(const string & symbol_value, CScriptFunction *info_function=NULL);
		bool removeVariableSymbolByIndex(unsigned int idx, bool remove_vector=false);
		vector<tStackElement> * getVectorVariable();


		virtual tFunctionSymbol * addFunctionSymbol(const string & symbol_value,CScriptFunction *irv, bool ignore_duplicates=true);
		tFunctionSymbol * getIdxScriptFunctionObjectByClassFunctionName(const string & funname);

		tFunctionSymbol * getFunctionSymbol(const string & varname,bool only_var_name=false);
		tFunctionSymbol * getFunctionSymbolByIndex(unsigned int idx);
		vector <tFunctionSymbol> * getVectorFunctionSymbol();

		void * get_C_Object();
		bool is_c_object();
		CScriptClass *get_C_Class();

		CScriptFunction *getConstructorFunction();


		const string & getClassName();

		const string & getPointer_C_ClassName();

		virtual string * toString();
		virtual bool initSharedPtr(bool is_assigned=false);
		virtual bool unrefSharedPtr();

		intptr_t get_C_StructPtr();

		virtual void destroy();
		virtual ~CScriptVariable();

	protected:

		/**
		 * This variable tells whether is pointer function or not.
		 */
		CScriptClass *m_infoRegisteredClass;
		CScriptClass *c_scriptclass_info;
		bool delete_c_object;



		virtual void setup();

		//vector<tFunctionSymbol> m_variableSymbol;

		vector<string> 		  m_variableKey;

	private:


		void * created_object;
		void * c_object;

		string aux_string;

		void createSymbols(CScriptClass *irv);

		vector<tFunctionSymbol> m_functionSymbol;




	};

}
