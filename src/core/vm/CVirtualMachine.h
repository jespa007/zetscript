#pragma once


#include "core/CCompiler.h"


//#define MAX_PER_TYPE_OPERATIONS 32
#define VM_LOCAL_VAR_MAX_STACK				2048
#define VM_ALE_OPERATIONS_MAX_STACK			2048	 // max operations ...


//#define MAX_UNIQUE_OBJECTS_POINTERS 2048
#define MAX_STACK 256

#define MAX_SHARES_VARIABLE 64


class CScriptFunction;
class CVirtualMachine{



//===================================================================================================
//
// POINTER MANAGER ...
//


	typedef struct {
		PInfoSharedPointerNode first, last;
	}tInfoSharedList;


	tInfoSharedList zero_shares[MAX_STACK];
	tInfoSharedList shared_var[MAX_STACK];

	//PInfoSharedPointerNode InsertZeroShares(CScriptVariable *shared_ptr);

/*
	tInfoSharedPointer shared_pointer[MAX_STACK][MAX_UNIQUE_OBJECTS_POINTERS];
	short pointers_with_0_shares[MAX_STACK][MAX_UNIQUE_OBJECTS_POINTERS],
		n_pointers_with_0_shares[MAX_STACK];

	short indexFreeCell[MAX_STACK][MAX_UNIQUE_OBJECTS_POINTERS],
		n_freeCell[MAX_STACK];
*/

	int idxCurrentStack;



/*	int getFreeCell();
	void setFreeCell(int index_to_free);


	int insert0Shares(int shared_pointer_idx);
*/
//===================================================================================================

public:



	//static CVirtualMachine * getInstance();
	//static void destroySingletons();



	CScriptVariable * execute(CScriptFunctionObject *info_function, CScriptVariable *this_object, vector<CScriptVariable *> * argv=NULL,int stk=0);

public:

//===================================================================================================
//
// POINTER MANAGER ...
//

	//static CSharedPointerManager * getInstance();
	//static void destroySingletons();

	PInfoSharedPointerNode newSharedPointer(CScriptVariable *var_ptr);
	/*void removeUnSharedPointers();
	//void gc();
	int getIdx0Shares(int index);
	int getNumShares(int index);
	void remove0Shares(int index_0_share_idx);*/

	bool sharePointer( PInfoSharedPointerNode _node);
	void unrefSharedPointer( PInfoSharedPointerNode _node);

	//static CSharedPointerManager *sharedPointerManager;



//===================================================================================================

	CVirtualMachine();
	~CVirtualMachine();


//	CALE();


	/*inline bool performInstruction( tInfoAsmOp * instruction,
			int & jmp_to_statment,
			int & jmp_to_instruction,
			CScriptFunctionObject *info_function,
			CScriptVariable *function_object,
			vector<CScriptVariable *> * argv,
			tInfoAsmOp *asm_op,
			int n_stk);*/

	//inline void reset();

	//inline CScriptVariable * createVarFromResultInstruction(tAleObjectInfo * ptr_result_instruction, bool share_ptr = true);

//	~CALE();




private:


	 string 	aux_string;
	// vector<CASTNode *> *vec_ast_node;
	// CSharedPointerManager *instance_gc;
	CUndefined				*VM_UNDEFINED;
	CNull					*VM_NULL;

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
	tAleObjectInfo 		*ptrStkCurrentResultInstruction,*ptrStartStkResultInstruction;

	//CScriptVariable  **stkResultObject[MAX_PER_TYPE_OPERATIONS]; // all variables references to this ...
	//CVector	 * vector[MAX_OPERANDS];

	int
		startIdxStkNumber,

		startIdxStkString;


	int
		idxStkCurrentNumber,

		idxStkCurrentString;
		//idxStkCurrentResultInstruction;

	// push indexes ...
	std::stack<int>
				vecIdxStkNumber,

				vecIdxStkString;

	std::stack<tAleObjectInfo *>		vecPtrCurrentStkResultInstruction;

	vector<CScriptVariable *> m_functionArgs;

	string STR_GET_TYPE_VAR_INDEX_INSTRUCTION(tAleObjectInfo * index);

	//inline bool pushInteger(int  init_value, CScriptVariable ** ptrAssignable=NULL, int properties=0);

	bool pushNumber(float init_value, CScriptVariable ** ptrAssignable=NULL,unsigned short properties=0);
	bool pushString(const string & init_value, CScriptVariable ** ptrAssignable=NULL,unsigned short properties=0);
	bool pushFunction(CScriptFunctionObject * init_value, CScriptVariable ** ptrAssignable=NULL,unsigned short properties=0);
	//bool pushVar(CScriptVariable * , CScriptVariable ** ptrObjectRef=NULL, int properties=0,bool is_new_var=false);

	//bool assignVarFromResultInstruction(CScriptVariable **obj, tAleObjectInfo * ptr_result_instruction);


	/*inline bool LOAD_VARIABLE(
			tInfoAsmOp *iao,
			CScriptFunctionObject *local_function,
			CScriptVariable *this_object,
			tInfoAsmOp *asm_op);
	//inline bool LOAD_CONSTANT();
	inline bool LOAD_OP(
			CScriptFunctionObject *info_function,
			CScriptVariable *this_object,
			tInfoAsmOp *instruction,
			tInfoAsmOp *asm_op,
			int index_op1,
			int index_op2);*/

	//bool performPreOperator(ASM_PRE_POST_OPERATORS pre_post_operator_type, CScriptVariable *obj);
	//bool performPostOperator(ASM_PRE_POST_OPERATORS pre_post_operator_type, CScriptVariable *obj);
//	bool loadValue(tInfoAsmOp *iao, int stk);
	/*bool loadConstantValue(CCompiler::tInfoConstantValue *info_constant);
	bool loadVariableValue(const tInfoAsmOp *iao,
			CScriptFunctionObject *info_function,
			CScriptVariable *this_object,
			tInfoAsmOp *asm_op);*/

	bool loadFunctionValue(const tInfoAsmOp * iao,
			CScriptFunctionObject *info_function,
			CScriptVariable *this_object,
			tInfoAsmOp *asm_op);

	void popScope(CScriptFunctionObject *info_function,int index);//, CScriptVariable *ret = NULL);


//private:


//	CVirtualMachine *m_virtualMachine;

};
