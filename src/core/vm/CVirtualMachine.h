#pragma once


#include "core/CCompiler.h"


//#define MAX_PER_TYPE_OPERATIONS 32
#define VM_LOCAL_VAR_MAX_STACK				5000
#define VM_ALE_OPERATIONS_MAX_STACK			4096	 // max operations ...


class CScriptFunction;
class CVirtualMachine{

	typedef struct{
		VALUE_INSTRUCTION_TYPE 		type; // tells what kind of variable is. By default is object.
		void			 		* 	stkResultObject; // pointer to pointer ables to modify its pointer when is needed
		CScriptVariable  		** 	ptrObjectRef; // pointer to pointer in case of replace var
	}tAleObjectInfo;

public:



	//static CVirtualMachine * getInstance();
	//static void destroySingletons();



	CScriptVariable * execute(tInfoRegisteredFunctionSymbol *info_function, CScriptVariable *this_object, vector<CScriptVariable *> * argv=NULL,int stk=0);

public:





	CVirtualMachine();
	~CVirtualMachine();


//	CALE();


	bool performInstruction( tInfoAsmOp * instruction,
			int & jmp_to_statment,
			int & jmp_to_instruction,
			tInfoRegisteredFunctionSymbol *info_function,
			CScriptVariable *function_object,
			vector<CScriptVariable *> * argv,
			vector<tInfoAsmOp *> *asm_op,
			int n_stk);

	void reset();

	CScriptVariable * createVarFromResultInstruction(tAleObjectInfo * ptr_result_instruction, bool share_ptr = true);

//	~CALE();


private:




	 tAleObjectInfo stack[VM_LOCAL_VAR_MAX_STACK];
	 tAleObjectInfo *basePtrLocalVar;
	 int idxStkCurrentLocalVar;
	 std::stack<int>	vecIdxLocalVar;

	/**
	 * Reserve for N vars. Return base pointer.
	 */
	 void pushStack(tInfoRegisteredFunctionSymbol *info_function,vector<CScriptVariable *> * argv);
	 void popStack();

	/*enum{
		MAX_OPERATIONS_PER_EXPRESSION=(MAX_PER_TYPE_OPERATIONS+1)*CScriptVariable::MAX_VAR_TYPES // +1 because
	};*/

	// some stack vars to have fast push
	float 				stkNumber[VM_ALE_OPERATIONS_MAX_STACK];
	string 				stkString[VM_ALE_OPERATIONS_MAX_STACK];
	tAleObjectInfo 		stkResultInstruction[VM_ALE_OPERATIONS_MAX_STACK];
	int					idxSavedInstruction;
	//CScriptVariable  **stkResultObject[MAX_PER_TYPE_OPERATIONS]; // all variables references to this ...
	//CVector	 * vector[MAX_OPERANDS];

	int
		startIdxStkNumber,

		startIdxStkString,
		startIdxStkResultInstruction;


	int
		idxStkCurrentNumber,

		idxStkCurrentString,
		idxStkCurrentResultInstruction;

	// push indexes ...
	std::stack<int>
				vecIdxStkNumber,

				vecIdxStkString,
				vecIdxStkResultInstruction;

	vector<CScriptVariable *> m_functionArgs;

	string STR_GET_TYPE_VAR_INDEX_INSTRUCTION(tAleObjectInfo * index);

	bool pushInteger(int  init_value, CScriptVariable ** ptrAssignable=NULL);
	bool pushBoolean(bool init_value, CScriptVariable ** ptrAssignable=NULL, int n_stk=0);
	bool pushNumber(float init_value, CScriptVariable ** ptrAssignable=NULL);
	bool pushString(const string & init_value, CScriptVariable ** ptrAssignable=NULL);
	bool pushFunction(tInfoRegisteredFunctionSymbol * init_value, CScriptVariable ** ptrAssignable=NULL);
	bool pushVar(CScriptVariable * , CScriptVariable ** ptrObjectRef=NULL, bool is_new_var=false);
	bool assignVarFromResultInstruction(CScriptVariable **obj, tAleObjectInfo * ptr_result_instruction);




	//bool performPreOperator(ASM_PRE_POST_OPERATORS pre_post_operator_type, CScriptVariable *obj);
	//bool performPostOperator(ASM_PRE_POST_OPERATORS pre_post_operator_type, CScriptVariable *obj);
//	bool loadValue(tInfoAsmOp *iao, int stk);
	bool loadConstantValue(CCompiler::tInfoConstantValue *info_constant, int n_stk);
	bool loadVariableValue(tInfoAsmOp *iao,
			tInfoRegisteredFunctionSymbol *info_function,
			CScriptVariable *this_object,
			vector<tInfoAsmOp *> *asm_op,
			int n_stk);

	bool loadFunctionValue(tInfoAsmOp *iao,
			tInfoRegisteredFunctionSymbol *info_function,
			CScriptVariable *this_object,
			vector<tInfoAsmOp *> *asm_op,
			int n_stk);

	void popScope(tInfoRegisteredFunctionSymbol *info_function,int index, CScriptVariable *ret = NULL);


//private:


//	CVirtualMachine *m_virtualMachine;

};
