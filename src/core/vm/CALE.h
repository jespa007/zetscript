#pragma once

#include "CVirtualMachine.h"

#define MAX_PER_TYPE_OPERATIONS 32



/**
 * Arithmetic Logic Expression (ALE) for calculation purposes.
 */
class CALE{

public:

	CALE();




	bool performInstruction( int idx_instruction, tInfoAsmOp * instruction, int & jmp_to_statment,CScriptClass *function_object,vector<CVariable *> * argv, int n_stk);

	void reset();
	CVariable * getObjectFromIndex(int index);


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
	CScriptClass *stkFunction[MAX_PER_TYPE_OPERATIONS];
	//CVariable  **stkObject[MAX_PER_TYPE_OPERATIONS]; // all variables references to this ...
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
		CVariable   * stkObject; // pointer to pointer ables to modify its pointer when is needed
		CVariable  ** ptrAssignableVar; // pointer to pointer in case of replace var
	}tAleInstructionInfo;

	tAleInstructionInfo result_object_instruction[MAX_OPERATIONS_PER_EXPRESSION];
	int current_asm_instruction;

	vector<CVariable *> m_functionArgs;


	bool pushInteger(int  init_value);
	bool pushBoolean(bool init_value, int n_stk=0);
	bool pushNumber(float init_value);
	bool pushString(const string & init_value);
	bool pushObject(CVariable * , CVariable ** ptrAssignableVar);
	bool pushVector(CVariable * init_value);
	bool pushFunction(tInfoRegisteredFunctionSymbol * init_value);
	bool assignObjectFromIndex(CVariable **obj, int index);


	CVariable * createObjectFromIndex(int index);

	bool performPreOperator(ASM_PRE_POST_OPERATORS pre_post_operator_type, CVariable *obj);
	bool performPostOperator(ASM_PRE_POST_OPERATORS pre_post_operator_type, CVariable *obj);
//	bool loadValue(tInfoAsmOp *iao, int stk);
	bool loadConstantValue(CVariable *bj, int n_stk);
	bool loadVariableValue(tInfoAsmOp *iao,CScriptClass *this_object, int n_stk);
	bool loadFunctionValue(tInfoAsmOp *iao,CScriptClass *this_object, int n_stk);



};
