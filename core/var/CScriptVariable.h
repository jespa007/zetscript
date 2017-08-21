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
		void init(CScriptClass *info_registered_class, void *  _c_object);

		void fun1(int *);
		void fun1(int *, int *);
		void fun1(string *);

		// test metamethod....
		CScriptVariable * _add(CScriptVariable *v1);
		CScriptVariable * _add(CScriptVariable *v1, CScriptVariable *v2);


		//CScriptVariable();//CScriptClass *info_registered_class, void * _c_object);


		//TYPE m_type;
		//int getIdxClass();


		bool setIdxClass(int idx);

		tSymbolInfo * addVariableSymbol(const string & symbol_value, int _idxAstNode,tStackElement * sv=NULL);
		tSymbolInfo * getVariableSymbol(const string & varname);
		tSymbolInfo * getVariableSymbolByIndex(unsigned idx);

		tSymbolInfo * addFunctionSymbol(const string & symbol_value,int _idxAstNode,CScriptFunctionObject *irv);
		tSymbolInfo * getIdxScriptFunctionObjectByClassFunctionName(const string & funname);
		int getidxScriptFunctionObjectWithMatchArgs(const string & varname, tStackElement * ptrArg, unsigned char n_args, bool match_signature=false);
		string getMessageMatchingFunctions(const string & varname);
		tSymbolInfo * getFunctionSymbolByIndex(unsigned idx);
		vector <tSymbolInfo> * getVectorFunctionSymbol();

		void * get_C_Object();
		bool is_c_object();
		CScriptClass *get_C_Class();

		CScriptFunctionObject *getConstructorFunction();

		//void addArgSymbol(const string & arg_name);

		const string & getClassName();

		const string & getPointer_C_ClassName();


		virtual string * toString();
		virtual bool initSharedPtr(bool is_assigned=false);
		virtual bool unrefSharedPtr();

		intptr_t get_C_StructPtr();

		void destroy(bool delete_user_request=false);
		virtual ~CScriptVariable();

	protected:

		/**
		 * This variable tells whether is pointer function or not.
		 */
		CScriptClass *m_infoRegisteredClass;
		CScriptClass *c_class_create_destroy;



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
