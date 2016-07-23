#pragma once


#include "CCompiler.h"





class CScriptFunction;
class CVirtualMachine{

public:




	static CVirtualMachine * getInstance();
	static void destroySingletons();



	static bool execute(CScriptFunction *fs, int stk, vector<CObject *> * argv=NULL);

private:


	static CVirtualMachine *m_virtualMachine;
	CVirtualMachine();
	~CVirtualMachine();
};
