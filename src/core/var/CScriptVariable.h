#pragma once



#include "core/ScriptDefinesStructs.h"
#include "core/ast/CScopeInfo.h"

#define IS_CLASS_C ((m_infoRegisteredClass->metadata_info.object_info.symbol_info.properties & SYMBOL_INFO_PROPERTIES::PROPERTY_C_OBJECT_REF) == SYMBOL_INFO_PROPERTIES::PROPERTY_C_OBJECT_REF)



class CScriptVariable{//: public CVariable{

public:

	static CUndefined *UndefinedSymbol;
	//static CVoid *VoidSymbol;
	static CNull *NullSymbol;




	static void createSingletons();
	static void destroySingletons();

	class tSymbolInfo{
		public:
		void *object; // created object. undefined by default.
		void * proxy_ptr; // for proxy functions...
		tSymbolInfo *super_function; // only for functions ...
		string value_symbol;
		tASTNode *ast; // in case there's ast node...
		tSymbolInfo(){
			proxy_ptr=NULL;
			this->object = CScriptVariable::UndefinedSymbol;
			this->ast = NULL;
			super_function=NULL;
		}

	};

	//----------------------
	// MEM MANNAGER RELATED
	//bool deallocatable;
	int idx_shared_ptr;
	//----------------------

	// public vars ...
	string m_strValue;
	void *m_value;

	//CScriptVariable();
	CScriptVariable();

	void fun1(int *);
	void fun1(string *);

	CScriptVariable(tInfoRegisteredClass *info_registered_class, void * _object_by_user);


	//TYPE m_type;
	int getIdxClass(){return m_infoRegisteredClass->class_idx;}

	tSymbolInfo *  addVariableSymbol(const string & value_symbol,tASTNode *ast);
	tSymbolInfo * getVariableSymbol(const string & varname);
	tSymbolInfo * getVariableSymbolByIndex(unsigned idx);

	tSymbolInfo * addFunctionSymbol(const string & value_symbol,tASTNode *ast,tInfoRegisteredFunctionSymbol *irv);
	tSymbolInfo * getFunctionSymbol(const string & funname);
	int getIdxFunctionSymbolWithMatchArgs(const string & varname, vector<CScriptVariable *> *argv, bool match_signature=false);
	tSymbolInfo * getFunctionSymbolByIndex(unsigned idx);

	vector<tInfoStatementOp> * getCompiledCode(int idx_function);

	tInfoRegisteredFunctionSymbol *getConstructorFunction();

	//void addArgSymbol(const string & arg_name);

	const string & getClassName(){
		return m_infoRegisteredClass->metadata_info.object_info.symbol_info.symbol_name;
	}

	const string & getPointer_C_ClassName(){
		return m_infoRegisteredClass->classPtrType;
	}


    virtual string * toString(){
    	return &m_strValue;
    }

	int get_C_StructPtr(){
		return (int)c_object;
	}

	virtual ~CScriptVariable();

protected:
	virtual void setup();
private:


	void *created_object;
	void *c_object;

	/**
	 * This variable tells whether is pointer function or not.
	 */
	tInfoRegisteredClass *m_infoRegisteredClass;


	void createSymbols(tInfoRegisteredClass *irv);

	vector<tSymbolInfo> m_variableSymbol;
	vector<tSymbolInfo> m_functionSymbol;

};
