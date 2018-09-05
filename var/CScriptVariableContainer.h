#pragma once


namespace zetscript{

	class CScriptVariableContainer:public CScriptVariable{

	protected:
		typedef struct {
			string key;
			int		idx;
			tStackElement *stk;
		}tNamedElementVector;

		vector<tNamedElementVector> vecNamedElement;
	public:

		virtual tSymbolInfo * addVariableSymbol(const string & symbol_value, int _idxAstNode,tStackElement * sv=NULL);
		virtual tSymbolInfo * addFunctionSymbol(const string & symbol_value,int _idxAstNode,CScriptFunctionObject *irv, bool ignore_duplicates=true);

		virtual tStackElement * getValue(const string & s);
		virtual tStackElement * getValue(unsigned int idx);
		const char 			  * getKey(unsigned int idx);

		void add(const char *key);
		void remove(unsigned int idx);
		void remove(const char *key);

	};

};
