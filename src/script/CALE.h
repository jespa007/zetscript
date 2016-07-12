#pragma once

#include "CVirtualMachine.h"

#define MAX_PER_TYPE_OPERATIONS 32



/**
 * Arithmetic Logic Expression (ALE) for calculation purposes.
 */
class CALE{

public:

	CALE();




	bool performInstruction(int idx_instruction, CCompiler::tInfoAsmOp * instruction,CScriptFunction *sf, int & jmp_to_statment);

	void reset();


	~CALE();


private:

	enum{
		MAX_OPERATIONS_PER_EXPRESSION=(MAX_PER_TYPE_OPERATIONS+1)*CVariable::MAX_VAR_TYPES // +1 because
	};

	// some stack vars to have fast push
	CInteger 	*stkInteger[MAX_PER_TYPE_OPERATIONS];
	CNumber		*stkNumber[MAX_PER_TYPE_OPERATIONS];
	CBoolean 	*stkBoolean[MAX_PER_TYPE_OPERATIONS];
	CString  *stkString[MAX_PER_TYPE_OPERATIONS];
	CVector  *stkVector[MAX_PER_TYPE_OPERATIONS];
	CScriptFunction *stkFunction[MAX_PER_TYPE_OPERATIONS];
	//CObject  **stkObject[MAX_PER_TYPE_OPERATIONS]; // all variables references to this ...
	//CVector	 * vector[MAX_OPERANDS];

	int n_stkInteger,
	n_stkNumber,
	n_stkBoolean,
	n_stkString,
	n_stkObject,
	n_stkVector,
	n_stkFunction,
	n_totalIntegerPointers,
	n_totalNumberPointers,
	n_totalStringPointers,
	n_totalBooleanPointers;



	typedef struct{
		CVariable::VAR_TYPE type; // tells what kind of variable is. By default is object.
		CObject  **stkObject; // pointer to pointer ables to modify its pointer when is needed
		bool isAssignable;
	}tAleInstructionInfo;

	tAleInstructionInfo result_object_instruction[MAX_OPERATIONS_PER_EXPRESSION];
	int current_asm_instruction;

	vector<CObject *> m_functionArgs;


	bool pushInteger(int  init_value);
	bool pushBoolean(bool init_value);
	bool pushNumber(float init_value);
	bool pushString(const string & init_value);
	bool pushObject(CObject ** , bool isAssignable=false);
	bool pushVector(CObject * init_value);
	bool pushFunction(CObject ** init_value);
	bool assignObjectFromIndex(CObject **obj, int index);

	CObject * getObjectFromIndex(int index);
	CObject * createObjectFromIndex(int index);

	bool performPreOperator(CCompiler::ASM_PRE_POST_OPERATORS pre_post_operator_type, CObject *obj);
	bool performPostOperator(CCompiler::ASM_PRE_POST_OPERATORS pre_post_operator_type, CObject *obj);
	bool loadValue(CCompiler::tInfoAsmOp *iao);
	bool loadConstantValue(CObject *bj);


};
