#pragma once


class CScriptFunction: public CFunctor{

	CScriptClass *this_object;
	tInfoRegisteredFunctionSymbol *irsv;

	/**
	 * Return variable is assigned at the begin as undefined and when return keyword occurs,
	 * returnVariable is assigned as the result of expression after return. By default return variable is type void.
	 */
	CObject *returnVariable;


public:
	CScriptFunction(CScriptClass *this_object,tInfoRegisteredFunctionSymbol *irsv);
	CScriptClass * getThisObject(){return this_object;}
		tInfoRegisteredFunctionSymbol * getFunctionInfo(){return irsv;}

	CObject ** 	 getReturnObjectPtr();
	void setReturnObject(CObject *);
	CObject 	*getReturnObject();
};
