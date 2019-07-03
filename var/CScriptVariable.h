/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
#pragma once

#include "../Common.h"


#define TYPE_SCRIPT_VARIABLE "__ScriptVar__"

namespace zs{

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
		std::vector<tStackElement> m_variable;
		//----------------------

		// public vars ...
		std::string m_strValue;
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

		virtual tStackElement * addVariableSymbol(const std::string & symbol_value, const CScriptFunction *info_function=NULL, tInstruction *src_instruction = NULL, tStackElement * sv=NULL);
		tStackElement * getVariableSymbol(const std::string & varname);
		tStackElement * getVariableSymbolByIndex(unsigned int idx);
		bool removeVariableSymbolByName(const std::string & symbol_value, const CScriptFunction *info_function=NULL);
		bool removeVariableSymbolByIndex(unsigned int idx, bool remove_vector=false);
		std::vector<tStackElement> * getVectorVariable();


		virtual tFunctionSymbol * addFunctionSymbol(const std::string & symbol_value,const CScriptFunction *irv, bool ignore_duplicates=false);
		tFunctionSymbol * getIdxScriptFunctionObjectByClassFunctionName(const std::string & funname);

		tFunctionSymbol * getFunctionSymbol(const std::string & varname);
		tFunctionSymbol * getFunctionSymbolByIndex(unsigned int idx);
		std::vector <tFunctionSymbol> * getVectorFunctionSymbol();

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

		/**
		 * This variable tells whether is pointer function or not.
		 */
		CScriptClass *m_infoRegisteredClass;
		CScriptClass *c_scriptclass_info;
		bool delete_c_object;



		virtual void setup();

		//std::vector<tFunctionSymbol> m_variableSymbol;

		std::vector<std::string> 		  m_variableKey;

	private:


		void * created_object;
		void * c_object;

		std::string aux_string;

		void createSymbols(CScriptClass *irv);

		std::vector<tFunctionSymbol> m_functionSymbol;




	};

}
