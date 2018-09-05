#include "../CZetScript.h"

namespace zetscript{

	tStackElement 		  * CScriptVariableContainer::getValue(const string & s){
		tStackElement 		  * se=NULL;
		for(unsigned i=0; i < vecNamedElement.size() && se==NULL; i++){
			if(vecNamedElement[i].key==s){
				se=vecNamedElement[vecNamedElement[i].idx].stk;
			}
		}

		return se;
	}

	tSymbolInfo * CScriptVariableContainer::addVariableSymbol(const string & symbol_value, int _idxAstNode,tStackElement * sv){
		tSymbolInfo *si = CScriptVariable::addVariableSymbol(symbol_value,_idxAstNode,sv);
		if(!si){
			return NULL;
		}

		// add symbol in the end ...
		tNamedElementVector nev;
		nev.idx=this->vecNamedElement.size();
		nev.key=symbol_value;
		nev.stk=&si->object;

		return si;
	}

	tSymbolInfo * CScriptVariableContainer::addFunctionSymbol(const string & symbol_value,int _idxAstNode,CScriptFunctionObject *irv, bool ignore_duplicates){
		tSymbolInfo *si = CScriptVariable::addFunctionSymbol(symbol_value,_idxAstNode,irv,ignore_duplicates);
		if(!si){
			return NULL;

		}

		// add symbol in the end ...
		tNamedElementVector nev;
		nev.idx=this->vecNamedElement.size();
		nev.key=symbol_value;
		nev.stk=&si->object;

		return si;

	}

	tStackElement * CScriptVariableContainer::getValue(unsigned int idx){
		if(idx >= this->vecNamedElement.size()){
			writeErrorMsg(NULL,0,"getValue:vector out of bounds");
			return NULL;
		}

		return vecNamedElement[idx].stk;

	}

	const char 			  * CScriptVariableContainer::getKey(unsigned int idx){
		if(idx >= this->vecNamedElement.size()){
			writeErrorMsg(NULL,0,"getKey:vector out of bounds");
			return NULL;
		}

		return vecNamedElement[idx].key.c_str();
	}

	void CScriptVariableContainer::add(const char *key){

	}

	void CScriptVariableContainer::remove(unsigned int idx){

	}

	void CScriptVariableContainer::remove(const char *key){

	}


};
