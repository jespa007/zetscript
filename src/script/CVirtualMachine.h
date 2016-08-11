#pragma once


#include "CCompiler.h"





class CScriptClass;
class CVirtualMachine{

public:




	static CVirtualMachine * getInstance();
	static void destroySingletons();



	static bool execute(CScriptClass *this_object, tInfoRegisteredFunctionSymbol *function_info, vector<CObject *> * argv=NULL,int stk=0);

private:


	static CVirtualMachine *m_virtualMachine;
	CVirtualMachine();
	~CVirtualMachine();
};
