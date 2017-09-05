#pragma once


namespace zetscript{

	#include "core/CCompiler.h"
	//#include "core/CScriptClass.h"


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

	#define NO_PARAMS vector<zetscript::CScriptVariable*>{}

	int 									getIdxClassFromIts_C_Type(const string & c_type_str);
	BASIC_CLASS_TYPE 				getIdxPrimitiveFromIts_C_Type(const string & c_type_str);


	class CScriptFunction;
	class  CVirtualMachine{



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
		tStackElement stk_aux;

		tStackElement * execute_internal(
				CScriptFunctionObject *info_function,
				CScriptVariable * this_object,
				tStackElement 		  * _ptrStartOp=NULL,
				string 		  		  * _ptrStartStr=NULL,
				unsigned char n_args=0);//vector<CScriptVariable *> * argv=NULL,int stk=0);



	public:


	//===================================================================================================
	//
	// POINTER MANAGER ...
	//

		//static CSharedPointerManager * getInstance();
		//static void destroySingletons();

		PInfoSharedPointerNode newSharedPointer(CScriptVariable *var_ptr);
		void sharePointer( PInfoSharedPointerNode _node);
		void unrefSharedScriptVar( PInfoSharedPointerNode _node);
		void removefSharedScriptVar( PInfoSharedPointerNode _node);

		//static CSharedPointerManager *sharedPointerManager;



	//===================================================================================================

		void iniStackVar(unsigned int pos,const tStackElement & stk);

		string stk_C_TypeStr(const tStackElement & stk_v);

		template<typename _T>
		static bool instanceof(const tStackElement & stk_v){
			BASIC_CLASS_TYPE primitiveid = getIdxPrimitiveFromIts_C_Type(typeid(_T).name());
			switch(primitiveid){
			case IDX_CLASS_INT_PTR_C:
				if(stk_v.properties & STK_PROPERTY_TYPE_INTEGER){
					return true;
				}
				break;
			case IDX_CLASS_FLOAT_PTR_C:
				if(stk_v.properties & STK_PROPERTY_TYPE_NUMBER){
					return true;
				}
				break;
			case IDX_CLASS_BOOL_PTR_C:
				if(stk_v.properties & STK_PROPERTY_TYPE_BOOLEAN){
					return true;
				}
				break;
			case IDX_CLASS_STRING_PTR_C:
				if(stk_v.properties & STK_PROPERTY_TYPE_STRING){
					return true;
				}
				break;
			default: // get class
				if(stk_v.properties & STK_PROPERTY_TYPE_SCRIPTVAR){
					int idxScriptVar = getIdxClassFromIts_C_Type(typeid(_T).name());
					return idxScriptVar==((CScriptVariable *)(stk_v.varRef))->idxScriptClass;
				}
				break;
			}
			return false;
		}

		template<typename  _T>
		_T read(const tStackElement & stk_v){
			if(instanceof<_T>(stk_v)){
				BASIC_CLASS_TYPE primitiveid = getIdxPrimitiveFromIts_C_Type(typeid(_T).name());
				switch(primitiveid){
				case IDX_CLASS_INT_PTR_C: // copy int
					if(stk_v.properties & STK_PROPERTY_TYPE_INTEGER){
						return (_T)(&stk_v.stkValue);
					}
					break;
				case IDX_CLASS_FLOAT_PTR_C: // copy number
					if(stk_v.properties & STK_PROPERTY_TYPE_NUMBER){
						return (_T)(&stk_v.stkValue);
					}
					break;
				case IDX_CLASS_BOOL_PTR_C: // copy bool
					if(stk_v.properties & STK_PROPERTY_TYPE_BOOLEAN){
						return (_T)(&stk_v.stkValue);
					}
					break;
				case IDX_CLASS_STRING_PTR_C: // copy string
					if(stk_v.properties & STK_PROPERTY_TYPE_STRING){
						return (_T)(stk_v.stkValue);
					}
					break;
				default: // get class
					if(stk_v.properties & STK_PROPERTY_TYPE_SCRIPTVAR){
						return (_T)stk_v.varRef;
					}
					else{
						zs_print_error_cr("Internal error! (%s, %s)",stk_C_TypeStr(stk_v).c_str(),demangle(typeid(_T).name()).c_str());
					}
					break;
				}
			}else{
				zs_print_error_cr("Cannot read stack value type \"%s\" into \"%s\"",stk_C_TypeStr(stk_v).c_str(),demangle(typeid(_T).name()).c_str());
			}
		}

		template<typename _T>
		bool write(tStackElement & stk_v, const _T & _v){
			if(instanceof<_T>(stk_v)){
				BASIC_CLASS_TYPE primitiveid = getIdxPrimitiveFromIts_C_Type(typeid(_T).name());
				switch(primitiveid){
				case IDX_CLASS_INT_PTR_C: // copy int
					if(stk_v.properties & STK_PROPERTY_TYPE_INTEGER){
						((int)stk_v.stkValue)=(*_v);
						return true;
					}
					break;
				case IDX_CLASS_FLOAT_PTR_C: // copy number
					if(stk_v.properties & STK_PROPERTY_TYPE_NUMBER){
						memcpy(stk_v.stkValue,_v,sizeof(float));
						return true;
					}
					break;
				case IDX_CLASS_BOOL_PTR_C: // copy bool
					if(stk_v.properties & STK_PROPERTY_TYPE_BOOLEAN){
						((bool)stk_v.stkValue)=(*_v);
						return true;
					}
					break;
				case IDX_CLASS_STRING_PTR_C: // copy string
					if(stk_v.properties & STK_PROPERTY_TYPE_STRING){
						*((string *)stk_v.stkValue)=(*_v);
						return true;
					}
					break;
				default: // get class
					zs_print_error_cr("You can only write int,bool,float or string types into stack elements but YOU CANNOT NOT WRITE pointers into C scope (this is not implemented yet).");
					return false;
					break;
				}

			}else{
				zs_print_error_cr("Cannot write value type \"%s\" into \"%s\"",demangle(typeid(_T).name()).c_str(),stk_C_TypeStr(stk_v).c_str());
			}

			return false;
		}


		inline float *setFloatReturnValue(float f){
			f_return_value = f;
			return &f_return_value;
		}

		inline string *setStringReturnValue(string s){
			s_return_value = s;
			return &s_return_value;
		}


		CScriptVariable * execute(
					 CScriptFunctionObject *info_function,
					 CScriptVariable *this_object,
					const vector<CScriptVariable *> &  argv=NO_PARAMS);


		void setCallResult(tStackElement *);
		tStackElement *getLastStackValue();
		tStackElement * getStackElement(unsigned int idx_glb_element);

		CVirtualMachine();
		~CVirtualMachine();



	private:

		struct tVM_ScopeInfo{
			short					   index;
			CScriptFunctionObject *ptr_info_function;
			tStackElement 		  *ptr_local_var;
		};

		char		str_aux[8192];
		float 		f_aux_value1,f_aux_value2;
		 string 	aux_string,symbol_to_find;
		 vector<tSymbolInfo> vec_aux_function_symbol;
		 tVM_ScopeInfo		*current_scope_idx;
		 tVM_ScopeInfo		scope_idx[VM_LOCAL_VAR_MAX_STACK];
		// vector<CASTNode *> *vec_ast_node;
		// CSharedPointerManager *instance_gc;
		CUndefinedScriptVariable				*VM_UNDEFINED; // it holds UNDEFINED_VAR.
		CNullScriptVariable					*VM_NULL;  // it holds NULL_VAR.
		tStackElement callc_result;


		string 				stkString[VM_LOCAL_VAR_MAX_STACK]; // aux values for string ...
		string              *ptrLastStr;
		string              *ptrCurrentStr;

		//unsigned short		idxBaseString,
		 //                   idxCurrentString;

		 tStackElement     stack[VM_LOCAL_VAR_MAX_STACK];
		 //unsigned short	 	idxBaseStk;



		// tStackElement *ptrBaseOp;
		tStackElement *ptrCurrentOp;
		// int idxCurrentOp;


		// std::stack<int>	vecIdxLocalVar,
		// 					vecIdxStkString;

		// std::stack<tStackElement *>		vecPtrCurrentStkResultInstruction;


		 tStackElement * call_C_function(
				 intptr_t fun_ptr,
				 const CScriptFunctionObject *irfs,
				 tStackElement *ptrArg,
				 unsigned char n_args);

		/**
		 * Reserve for N vars. Return base pointer.
		 */
		// inline void pushStack(const CScriptFunctionObject *info_function, int n_args);//,vector<CScriptVariable *> * argv);
		// inline void popStack();


		string STR_GET_TYPE_VAR_INDEX_INSTRUCTION(tStackElement * index);
		void 				stackDumped();

		//inline bool pushInteger(int  init_value, CScriptVariable ** ptrAssignable=NULL, int properties=0);

		//bool pushNumber(float init_value, CScriptVariable ** ptrAssignable=NULL,unsigned short properties=0);
		//bool pushString(const string & init_value, CScriptVariable ** ptrAssignable=NULL,unsigned short properties=0);
		//bool pushFunction(CScriptFunctionObject * init_value, CScriptVariable ** ptrAssignable=NULL,unsigned short properties=0);
		//bool pushVar(CScriptVariable * , CScriptVariable ** varRef=NULL, int properties=0,bool is_new_var=false);

		//bool assignVarFromResultInstruction(CScriptVariable **obj, tStackElement * ptr_result_instruction);


		//bool performPreOperator(ASM_PRE_POST_OPERATORS pre_post_operator_type, CScriptVariable *obj);
		//bool performPostOperator(ASM_PRE_POST_OPERATORS pre_post_operator_type, CScriptVariable *obj);
	//	bool loadValue(tInfoAsmOp *iao, int stk);
		/*bool loadConstantValue(CCompiler::tInfoConstantValue *info_constant);
		bool loadVariableValue(const tInfoAsmOp *iao,
				CScriptFunctionObject *info_function,
				CScriptVariable *this_object,
				tInfoAsmOp *asm_op);*/

	/*	bool loadFunctionValue(const tInfoAsmOp * iao,
				CScriptFunctionObject *info_function,
				CScriptVariable *this_object,
				tInfoAsmOp *asm_op);

		inline void popScope( CScriptFunctionObject *info_function,int index);//, CScriptVariable *ret = NULL);*/


	//private:


	//	CVirtualMachine *m_virtualMachine;

	};

}
