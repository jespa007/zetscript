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
		int							properties;
	}tAleObjectInfo;

public:



	//static CVirtualMachine * getInstance();
	//static void destroySingletons();



	CScriptVariable * execute(CScriptFunctionObject *info_function, CScriptVariable *this_object, vector<CScriptVariable *> * argv=NULL,int stk=0);

public:





	CVirtualMachine();
	~CVirtualMachine();


//	CALE();


	inline bool performInstruction( tInfoAsmOp * instruction,
			int & jmp_to_statment,
			int & jmp_to_instruction,
			CScriptFunctionObject *info_function,
			CScriptVariable *function_object,
			vector<CScriptVariable *> * argv,
			vector<tInfoAsmOp *> *asm_op,
			int n_stk);

	inline void reset();

	CScriptVariable * createVarFromResultInstruction(tAleObjectInfo * ptr_result_instruction, bool share_ptr = true);

//	~CALE();


private:


	 string 	aux_string;

	 tAleObjectInfo stack[VM_LOCAL_VAR_MAX_STACK];
	 tAleObjectInfo *basePtrLocalVar;
	 int idxStkCurrentLocalVar;
	 std::stack<int>	vecIdxLocalVar;

	/**
	 * Reserve for N vars. Return base pointer.
	 */
	 inline void pushStack(CScriptFunctionObject *info_function,vector<CScriptVariable *> * argv);
	 inline void popStack();

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

	inline bool pushInteger(int  init_value, CScriptVariable ** ptrAssignable=NULL, int properties=0);
	inline bool pushBoolean(bool init_value, CScriptVariable ** ptrAssignable=NULL, int properties=0);
	inline bool pushNumber(float init_value, CScriptVariable ** ptrAssignable=NULL, int properties=0);
	inline bool pushString(const string & init_value, CScriptVariable ** ptrAssignable=NULL, int properties=0);
	inline bool pushFunction(CScriptFunctionObject * init_value, CScriptVariable ** ptrAssignable=NULL, int properties=0);
	inline bool pushVar(CScriptVariable * , CScriptVariable ** ptrObjectRef=NULL, int properties=0,bool is_new_var=false);

	bool assignVarFromResultInstruction(CScriptVariable **obj, tAleObjectInfo * ptr_result_instruction);




	//bool performPreOperator(ASM_PRE_POST_OPERATORS pre_post_operator_type, CScriptVariable *obj);
	//bool performPostOperator(ASM_PRE_POST_OPERATORS pre_post_operator_type, CScriptVariable *obj);
//	bool loadValue(tInfoAsmOp *iao, int stk);
	inline bool loadConstantValue(CCompiler::tInfoConstantValue *info_constant, int n_stk);
	inline bool loadVariableValue(tInfoAsmOp *iao,
			CScriptFunctionObject *info_function,
			CScriptVariable *this_object,
			vector<tInfoAsmOp *> *asm_op,
			int n_stk);

	inline bool loadFunctionValue(tInfoAsmOp *iao,
			CScriptFunctionObject *info_function,
			CScriptVariable *this_object,
			vector<tInfoAsmOp *> *asm_op,
			int n_stk);

	inline void popScope(CScriptFunctionObject *info_function,int index);//, CScriptVariable *ret = NULL);


//private:


//	CVirtualMachine *m_virtualMachine;

};
