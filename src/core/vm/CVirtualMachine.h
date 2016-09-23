#pragma once


#include "core/CCompiler.h"




class CScriptFunction;
class CVirtualMachine{

public:



	static CVirtualMachine * getInstance();
	static void destroySingletons();



	static CScriptVariable * execute(tInfoRegisteredFunctionSymbol *info_function, CScriptVariable *this_object, vector<CScriptVariable *> * argv=NULL,int stk=0);

private:


	static CVirtualMachine *m_virtualMachine;
	CVirtualMachine();
	~CVirtualMachine();
};
