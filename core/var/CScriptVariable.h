/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
#pragma once

#include "../Common.h"
#include "core/CScope.h"

#define TYPE_SCRIPT_VARIABLE "__ScriptVar__"

namespace zetscript{

	class CScriptClass;

	class  CScriptVariable{//: public CVariable{

	public:

		//----------------------
		// MEM MANNAGER RELATED
		//bool deallocatable;
		//int idx_shared_ptr;
		PInfoSharedPointerNode ptr_shared_pointer_node;
		short idxScriptClass;
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


		bool setIdxClass(int idx);
		bool itHasSetMetamethod();
		void setDelete_C_ObjectOnDestroy(bool _delete_on_destroy);

		tSymbolInfo * addVariableSymbol(const string & symbol_value, int _idxAstNode,tStackElement * sv=NULL);
		tSymbolInfo * getVariableSymbol(const string & varname);
		tSymbolInfo * getVariableSymbolByIndex(unsigned idx);

		tSymbolInfo * addFunctionSymbol(const string & symbol_value,int _idxAstNode,CScriptFunctionObject *irv, bool ignore_duplicates=true);
		tSymbolInfo * getIdxScriptFunctionObjectByClassFunctionName(const string & funname);

		string getMessageMatchingFunctions(const string & varname);
		tSymbolInfo * getFunctionSymbol(const string & varname);
		tSymbolInfo * getFunctionSymbolByIndex(unsigned idx);
		vector <tSymbolInfo> * getVectorFunctionSymbol();

		void * get_C_Object();
		bool is_c_object();
		CScriptClass *get_C_Class();

		CScriptFunctionObject *getConstructorFunction();

		const string & getClassName();

		const string & getPointer_C_ClassName();


		virtual string * toString();
		virtual bool initSharedPtr(bool is_assigned=false);
		virtual bool unrefSharedPtr();

		intptr_t get_C_StructPtr();

		virtual void destroy(bool delete_user_request=false);
		virtual ~CScriptVariable();

	protected:

		/**
		 * This variable tells whether is pointer function or not.
		 */
		CScriptClass *m_infoRegisteredClass;
		CScriptClass *c_scriptclass_info;
		bool delete_c_object;



		virtual void setup();

		vector<tSymbolInfo> m_variableSymbol;

	private:


		void * created_object;
		void * c_object;

		string aux_string;

		void createSymbols(CScriptClass *irv);

		vector<tSymbolInfo> m_functionSymbol;


	};

}
