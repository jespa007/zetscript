#pragma once


#include "CCompiler.h"

#define MAX_PER_TYPE_OPERATIONS 32



class CScriptFunction;
class CVirtualMachine{

public:




	static CVirtualMachine * getInstance();
	static void destroySingletons();



	bool execute(CScriptFunction *fs, vector<CObject *> * argv=NULL);

private:


	static CVirtualMachine *m_virtualMachine;
	CVirtualMachine();
	~CVirtualMachine();
};
