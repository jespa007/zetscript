#pragma once


#include "core/CCompiler.h"





class CScriptFunction;
class CVirtualMachine{

public:




	static CVirtualMachine * getInstance();
	static void destroySingletons();



	static bool execute(CScriptFunction *function_object, vector<CVariable *> * argv=NULL,int stk=0);

private:


	static CVirtualMachine *m_virtualMachine;
	CVirtualMachine();
	~CVirtualMachine();
};
