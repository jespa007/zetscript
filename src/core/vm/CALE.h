#pragma once

#include "CVirtualMachine.h"

//#define MAX_PER_TYPE_OPERATIONS 32
#define VM_LOCAL_VAR_MAX_STACK				5000
#define VM_ALE_OPERATIONS_MAX_STACK			4096


/**
 * Arithmetic Logic Expression (ALE) for calculation purposes.
 */



enum VALUE_INSTRUCTION_TYPE{
	INS_TYPE_UNDEFINED=0,
	INS_TYPE_INTEGER, // primitive int
	INS_TYPE_NUMBER, // primitive number
	INS_TYPE_BOOLEAN, // primitive bool
	INS_TYPE_STRING, // primitive string
	INS_TYPE_FUNCTION, // primitive function
	INS_TYPE_VAR, // always is an script class...
	INS_MAX_TYPES
};


class CALE{

public:

	typedef struct{
		VALUE_INSTRUCTION_TYPE 		type; // tells what kind of variable is. By default is object.
		void	   				* 	stkObject; // pointer to pointer ables to modify its pointer when is needed
		CScriptVariable  		** 		ptrAssignableVar; // pointer to pointer in case of replace var
	}tAleObjectInfo;


	static tAleObjectInfo stack[VM_LOCAL_VAR_MAX_STACK];
	static tAleObjectInfo *currentBaseStack;
	static int idxCurrentStack;

	/**
	 * Reserve for N vars. Return base pointer.
	 */
	static tAleObjectInfo *allocStack(unsigned n_vars);
	static tAleObjectInfo *freeStack(unsigned n_vars);




	CALE();


	bool performInstruction( int idx_instruction, tInfoAsmOp * instruction, int & jmp_to_statment,tInfoRegisteredFunctionSymbol *info_function,CScriptVariable *function_object,vector<CScriptVariable *> * argv, int n_stk);

	void reset();



	~CALE();


private:

	/*enum{
		MAX_OPERATIONS_PER_EXPRESSION=(MAX_PER_TYPE_OPERATIONS+1)*CScriptVariable::MAX_VAR_TYPES // +1 because
	};*/

	// some stack vars to have fast push
	static int 				stkInteger[VM_ALE_OPERATIONS_MAX_STACK];
	static float 			stkNumber[VM_ALE_OPERATIONS_MAX_STACK];
	static bool				stkBoolean[VM_ALE_OPERATIONS_MAX_STACK];
	static string 			stkString[VM_ALE_OPERATIONS_MAX_STACK];
	static tInfoRegisteredFunctionSymbol 	*		stkFunction[VM_ALE_OPERATIONS_MAX_STACK];
	static CScriptVariable *	stkVar[VM_ALE_OPERATIONS_MAX_STACK];
	//CScriptVariable  **stkObject[MAX_PER_TYPE_OPERATIONS]; // all variables references to this ...
	//CVector	 * vector[MAX_OPERANDS];

	static int n_stkInteger,
		n_stkNumber,
		n_stkBoolean,
		n_stkString,
		n_stkFunction,
		n_stkVar;





	static tAleObjectInfo result_object_instruction[VM_ALE_OPERATIONS_MAX_STACK];
	int current_asm_instruction;

	vector<CScriptVariable *> m_functionArgs;

	string STR_GET_TYPE_VAR_INDEX_INSTRUCTION(int index);

	bool pushInteger(int  init_value);
	bool pushBoolean(bool init_value, int n_stk=0);
	bool pushNumber(float init_value);
	bool pushString(const string & init_value);
	bool pushFunction(tInfoRegisteredFunctionSymbol * init_value);
	bool pushVar(CScriptVariable * , CScriptVariable ** ptrAssignableVar);
	bool assignVarFromIndex(CScriptVariable **obj, int index);


	CScriptVariable * createVarFromIndex(int index);

	bool performPreOperator(ASM_PRE_POST_OPERATORS pre_post_operator_type, CScriptVariable *obj);
	bool performPostOperator(ASM_PRE_POST_OPERATORS pre_post_operator_type, CScriptVariable *obj);
//	bool loadValue(tInfoAsmOp *iao, int stk);
	bool loadConstantValue(CScriptVariable *bj, int n_stk);
	bool loadVariableValue(tInfoAsmOp *iao,tInfoRegisteredFunctionSymbol *info_function,CScriptVariable *this_object, int n_stk);
	bool loadFunctionValue(tInfoAsmOp *iao,tInfoRegisteredFunctionSymbol *info_function,CScriptVariable *this_object, int n_stk);




};
