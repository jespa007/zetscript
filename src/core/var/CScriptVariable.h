#pragma once



#include "../Common.h"
#include "core/CScope.h"

#define TYPE_SCRIPT_VARIABLE "__ScriptVar__"

#define IS_CLASS_C ((m_infoRegisteredClass->metadata_info.object_info.symbol_info.properties & SYMBOL_INFO_PROPERTIES::PROPERTY_C_OBJECT_REF) == SYMBOL_INFO_PROPERTIES::PROPERTY_C_OBJECT_REF)



class CScriptVariable{//: public CVariable{

public:


	class tSymbolInfo{
		public:
		void *object; // created object. undefined by default.
		void * proxy_ptr; // for proxy functions...
		tSymbolInfo *super_function; // only for functions ...
		string symbol_value;
		int idxAstNode; // in case there's ast node...

		tSymbolInfo();
	};

	//----------------------
	// MEM MANNAGER RELATED
	//bool deallocatable;
	int idx_shared_ptr;
	//----------------------

	// public vars ...
	string m_strValue;
	void *m_value;

	// Construct ...
	CScriptVariable();
	void init(CScriptClass *info_registered_class, void * _c_object);

	void fun1(int *);
	void fun1(string *);

	//CScriptVariable();//CScriptClass *info_registered_class, void * _c_object);


	//TYPE m_type;
	int getIdxClass();
	bool setIdxClass(int idx);

	tSymbolInfo *  addVariableSymbol(const string & symbol_value,int _idxAstNode);
	tSymbolInfo * getVariableSymbol(const string & varname);
	tSymbolInfo * getVariableSymbolByIndex(unsigned idx);

	tSymbolInfo * addFunctionSymbol(const string & symbol_value,int _idxAstNode,CScriptFunctionObject *irv);
	tSymbolInfo * getIdxScriptFunctionObjectByClassFunctionName(const string & funname);
	int getidxScriptFunctionObjectWithMatchArgs(const string & varname, vector<CScriptVariable *> *argv, bool match_signature=false);
	tSymbolInfo * getFunctionSymbolByIndex(unsigned idx);

	void * get_C_Object();

	vector<tInfoStatementOp> * getCompiledCode(int idx_function);

	CScriptFunctionObject *getConstructorFunction();

	//void addArgSymbol(const string & arg_name);

	const string & getClassName();

	const string & getPointer_C_ClassName();


    virtual string * toString();
    virtual bool refSharedPtr();
    virtual bool unrefSharedPtr();

	int get_C_StructPtr();

	virtual ~CScriptVariable();

protected:

	/**
	 * This variable tells whether is pointer function or not.
	 */
	CScriptClass *m_infoRegisteredClass;


	virtual void setup();

	vector<tSymbolInfo> m_variableSymbol;

private:


	void *created_object;
	void *c_object;



	void createSymbols(CScriptClass *irv);

	vector<tSymbolInfo> m_functionSymbol;

};
