#pragma once


#include "core/CCompiler.h"


//#define MAX_PER_TYPE_OPERATIONS 32
#define VM_LOCAL_VAR_MAX_STACK				2048
//#define VM_ALE_OPERATIONS_MAX_STACK			2048	 // max operations ...
#define VM_AUX_VALUES						512

//#define MAX_UNIQUE_OBJECTS_POINTERS 2048
#define MAX_FUNCTION_CALL 256

#define MAX_SHARES_VARIABLE 64


#define SET_INTEGER_RETURN(i)	CURRENT_VM->setIntegerReturnValue(i)
#define SET_BOOLEAN_RETURN(b) 	CURRENT_VM->setBooleanReturnValue(b)
#define SET_FLOAT_RETURN(f)   	CURRENT_VM->setFloatReturnValue(f)
#define SET_STRING_RETURN(s)  	CURRENT_VM->setStringReturnValue(s)


class CScriptFunction;
class CVirtualMachine{



//===================================================================================================
//
// POINTER MANAGER ...
//


	typedef struct {
		PInfoSharedPointerNode first, last;
	}tInfoSharedList;


	tInfoSharedList zero_shares[MAX_FUNCTION_CALL];
	tInfoSharedList shared_var[MAX_FUNCTION_CALL];


	int idxCurrentStack;

//===================================================================================================


	float f_return_value;
	string s_return_value;

	tAleObjectInfo * execute_internal(
			CScriptFunctionObject *info_function,
			CScriptVariable *this_object,
			unsigned int n_args=0);//vector<CScriptVariable *> * argv=NULL,int stk=0);



public:


//===================================================================================================
//
// POINTER MANAGER ...
//

	//static CSharedPointerManager * getInstance();
	//static void destroySingletons();

	PInfoSharedPointerNode newSharedPointer(CScriptVariable *var_ptr);
	bool sharePointer( PInfoSharedPointerNode _node);
	void unrefSharedPointer( PInfoSharedPointerNode _node);

	//static CSharedPointerManager *sharedPointerManager;



//===================================================================================================


	inline float *setFloatReturnValue(float f){
		f_return_value = f;
		return &f_return_value;
	}

	inline string *setStringFloatReturnValue(float s){
		s_return_value = s;
		return &s_return_value;
	}


	CScriptVariable * execute(
				CScriptFunctionObject *info_function,
				CScriptVariable *this_object,
				vector<CScriptVariable *> * argv=NULL);





	CVirtualMachine();
	~CVirtualMachine();



private:

	char		str_aux[8192];
	 string 	aux_string;
	// vector<CASTNode *> *vec_ast_node;
	// CSharedPointerManager *instance_gc;
	CUndefined				*VM_UNDEFINED; // it holds UNDEFINED_VAR.
	CNull					*VM_NULL;  // it holds NULL_VAR.
	tAleObjectInfo callc_result;


	string 				stkString[VM_LOCAL_VAR_MAX_STACK]; // aux values for string ...
	unsigned short		idxBaseString,
	                    idxCurrentString;

	 tAleObjectInfo     stack[VM_LOCAL_VAR_MAX_STACK];
	 unsigned short	 	idxBaseStk;

	 tAleObjectInfo *ptrLocalVar;
	 tAleObjectInfo *ptrArg;

	 tAleObjectInfo *ptrBaseOp;
	 tAleObjectInfo *ptrCurrentOp;
	 int idxCurrentOp;


	 std::stack<int>	vecIdxLocalVar,
	 					vecIdxStkString;

	 std::stack<tAleObjectInfo *>		vecPtrCurrentStkResultInstruction;


	 tAleObjectInfo * call_C_function(void *fun_ptr,CScriptFunctionObject *irfs, unsigned int n_args);

	/**
	 * Reserve for N vars. Return base pointer.
	 */
	 inline void pushStack(CScriptFunctionObject *info_function, int n_args);//,vector<CScriptVariable *> * argv);
	 inline void popStack();


	string STR_GET_TYPE_VAR_INDEX_INSTRUCTION(tAleObjectInfo * index);

	//inline bool pushInteger(int  init_value, CScriptVariable ** ptrAssignable=NULL, int properties=0);

	//bool pushNumber(float init_value, CScriptVariable ** ptrAssignable=NULL,unsigned short properties=0);
	//bool pushString(const string & init_value, CScriptVariable ** ptrAssignable=NULL,unsigned short properties=0);
	//bool pushFunction(CScriptFunctionObject * init_value, CScriptVariable ** ptrAssignable=NULL,unsigned short properties=0);
	//bool pushVar(CScriptVariable * , CScriptVariable ** varRef=NULL, int properties=0,bool is_new_var=false);

	//bool assignVarFromResultInstruction(CScriptVariable **obj, tAleObjectInfo * ptr_result_instruction);


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
