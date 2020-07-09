/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
#pragma once


#ifdef __DEBUG__ // incoment __ZETSCRIPT_VERBOSE_MESSAGE__ to print all messages (wrning is going to be slow because of the prints)
#define __ZETSCRIPT_VERBOSE_MESSAGE__
#endif


#define ZS_SOLVE_AT_RUNTIME				 0
#define ZS_ERROR						-1
#define ZS_UNDEFINED_IDX 				-1
//#define ZS_FUNCTION_NOT_FOUND_IDX	 	-2
#define ZS_THIS_IDX						-3

#define MAX_N_ARGS						 6
#define NO_PARAMS_IS_VARIABLE			-1
//#define NO_PARAMS_SYMBOL_ONLY			-2
#define NO_PARAMS_IS_CLASS				-2
#define DEFAULT_NO_FILENAME				"no_file"

// HELPER FUNCTIONS
#define NEW_CLASS_VAR_BY_IDX(idx)	 					((this->script_class_factory)->instanceScriptVariableByIdx(idx))
#define GET_SCRIPT_CLASS(idx)	 						((this->script_class_factory)->getScriptClass(idx))
#define GET_SCRIPT_CLASS_NAME(idx) 						((this->script_class_factory)->getScriptClassName(idx))
#define SCRIPT_CLASS_MAIN								((this->script_class_factory)->getScriptClass(IDX_BUILTIN_TYPE_CLASS_MAIN))    // 0 is the main class
#define SCRIPT_CLASS_STRING								((this->script_class_factory)->getScriptClass(IDX_BUILTIN_TYPE_CLASS_STRING))
#define SCRIPT_CLASS_DICTIONARY							((this->script_class_factory)->getScriptClass(IDX_BUILTIN_TYPE_CLASS_DICTIONARY))
#define SCRIPT_CLASS_VECTOR								((this->script_class_factory)->getScriptClass(IDX_BUILTIN_TYPE_CLASS_VECTOR))
#define SCRIPT_CLASS_FUNCTOR							((this->script_class_factory)->getScriptClass(IDX_BUILTIN_TYPE_CLASS_FUNCTOR))
#define GET_SCRIPT_CLASS_INFO_BY_C_PTR_NAME(s)			((this->script_class_factory)->getScriptClassBy_C_ClassPtr(s))    // 0 is the main class
#define GET_IDX_2_CLASS_C_STR(idx) 						((this->script_class_factory)->getScriptClass(idx)->str_class_ptr_type)
#define REGISTER_C_BASE_SYMBOLS(o)		   				((this->zs)->register_C_BaseSymbols(o))
#define GET_SCOPE						 				((this->scope_factory))->getScope
#define MAIN_SCOPE										((this->scope_factory))->getScope(IDX_GLOBAL_SCOPE)
#define NEW_SCOPE										((this->scope_factory))->newScope

// if 0 is in main <> 0, else.
#define SCOPE_IN_MAIN_CLASS(idx)						((this->scope_factory)->getScope(idx)->getIdxBaseScope()==IDX_GLOBAL_SCOPE)

#define DO_CAST											((this->zs))->doCast
#define GET_IDX_BUILTIN_TYPE_CLASS_FROM_ITS_C_TYPE					((this->zs))->GetIdxClassFromIts_C_Type
#define INSTANCE_SCRIPT_VARIABLE_BY_IDX					((this->zs))->instanceScriptVariableByIdx
#define GET_METAMETHOD(m)								((this->zs))->getMetamethod(m)
#define IS_IDX_BUILTIN_TYPE_CLASS_INSTANCEOF(zs,_class,_instance)	((this->script_class_factory))->isIdxClassInstanceOf(_class,_instance)

#define NEW_SCRIPT_FUNCTION								((this->script_function_factory))->newScriptFunction
#define GET_SCRIPT_FUNCTION(idx)	 					((this->script_function_factory))->GetScriptFunction(idx)
#define MAIN_FUNCTION									GET_SCRIPT_FUNCTION(IDX_MAIN_FUNCTION)


namespace zetscript{

	//typedef CASTNode *PASTNode;
	class ScriptFunction;
	class Scope;
	class ScriptClass;
	class ScriptVar;
	struct Symbol;
	struct FunctionInfo;
	struct ScopeVarInnerBlockInfo;


	typedef enum
		:unsigned char {

		LOAD_TYPE_NOT_DEFINED = 0,
		LOAD_TYPE_NULL,
		LOAD_TYPE_UNDEFINED,
		LOAD_TYPE_CONSTANT,
		LOAD_TYPE_VARIABLE,
		LOAD_TYPE_FUNCTION,
		LOAD_TYPE_ARGUMENT
	}LoadType;


	// properties shared by compiler + instruction ..
	typedef enum{
		//-- PRE/POST OPERATORS (Byy default there's no operators involved)
		BIT_INSTRUCTION_PROPERTY_PRE_INC = 0,	// 0x1
		BIT_INSTRUCTION_PROPERTY_POST_INC,		// 0x2
		BIT_INSTRUCTION_PROPERTY_PRE_DEC,		// 0x4
		BIT_INSTRUCTION_PROPERTY_POST_DEC,		// 0x8
		BIT_INSTRUCTION_PROPERTY_PRE_NOT,		// 0x10
		BIT_INSTRUCTION_PROPERTY_PRE_POST_OP,

		//-- SCOPE TYPE (By default is global scope)
		BIT_INSTRUCTION_PROPERTY_SCOPE_TYPE_ACCESS = BIT_INSTRUCTION_PROPERTY_PRE_POST_OP, 	// 0x20
		BIT_INSTRUCTION_PROPERTY_SCOPE_TYPE_THIS,							// 0x40
		BIT_INSTRUCTION_PROPERTY_SCOPE_TYPE_SUPER,						// 0x80
		BIT_INSTRUCTION_PROPERTY_SCOPE_TYPE_ACCESS,						// 0x100
		BIT_INSTRUCTION_PROPERTY_SCOPE_TYPE_MAX,

		//-- BYTE_CODE_CALL TYPE
		BIT_INSTRUCTION_PROPERTY_CHECK_IS_FUNCTION = BIT_INSTRUCTION_PROPERTY_SCOPE_TYPE_MAX,// 0x200
		BIT_INSTRUCTION_PROPERTY_DIRECT_CALL_RETURN,					// 0x400
		BIT_INSTRUCTION_PROPERTY_DEDUCE_C_CALL,						// 0x800
		BIT_INSTRUCTION_PROPERTY_CONSTRUCT_CALL,						// 0x1000
		BIT_INSTRUCTION_PROPERTY_NO_FUNCTION_CALL,					// 0x2000
		BIT_INSTRUCTION_PROPERTY_MAX,
	}BitInstructionProperty;

	// PRE/POST OP
	#define MSK_INSTRUCTION_PROPERTY_PRE_INC					(0x1<<BIT_INSTRUCTION_PROPERTY_PRE_INC) // ++
	#define MSK_INSTRUCTION_PROPERTY_POST_INC					(0x1<<BIT_INSTRUCTION_PROPERTY_POST_INC) // ++
	#define MSK_INSTRUCTION_PROPERTY_PRE_DEC					(0x1<<BIT_INSTRUCTION_PROPERTY_PRE_DEC) // --
	#define MSK_INSTRUCTION_PROPERTY_POST_DEC					(0x1<<BIT_INSTRUCTION_PROPERTY_POST_DEC) // --
	#define MSK_INSTRUCTION_PROPERTY_PRE_NOT					(0x1<<BIT_INSTRUCTION_PROPERTY_PRE_NOT)  // !
	#define MSK_INSTRUCTION_PROPERTY_PRE_POST_OP				(((0x1<<(BIT_INSTRUCTION_PROPERTY_PRE_POST_OP-BIT_INSTRUCTION_PROPERTY_PRE_INC))-1)<<(BIT_INSTRUCTION_PROPERTY_PRE_INC))
	#define GET_MSK_INSTRUCTION_PROPERTY_PRE_POST_OP(prop)		((prop)&MSK_INSTRUCTION_PROPERTY_PRE_POST_OP)

	//GLOBAL_SCOPE,   // by default
	#define	MSK_INSTRUCTION_PROPERTY_SCOPE_TYPE_ACCESS			(0x1<<BIT_INSTRUCTION_PROPERTY_SCOPE_TYPE_ACCESS)
	#define MSK_INSTRUCTION_PROPERTY_SCOPE_TYPE_THIS			(0x1<<BIT_INSTRUCTION_PROPERTY_SCOPE_TYPE_THIS)
	#define MSK_INSTRUCTION_PROPERTY_SCOPE_TYPE_SUPER			(0x1<<BIT_INSTRUCTION_PROPERTY_SCOPE_TYPE_SUPER)
	#define MSK_INSTRUCTION_PROPERTY_SCOPE_TYPE_ACCESS			(0x1<<BIT_INSTRUCTION_PROPERTY_SCOPE_TYPE_ACCESS)
	#define MSK_INSTRUCTION_PROPERTY_SCOPE_TYPE					(((0x1<<(BIT_INSTRUCTION_PROPERTY_SCOPE_TYPE_MAX-BIT_INSTRUCTION_PROPERTY_SCOPE_TYPE_ACCESS))-1)<<(BIT_INSTRUCTION_PROPERTY_SCOPE_TYPE_ACCESS))

	#define GET_MSK_INSTRUCTION_PROPERTY_SCOPE_TYPE(prop)		((prop)&MSK_INSTRUCTION_PROPERTY_SCOPE_TYPE)
	//#define REMOVE_MSK_INSTRUCTION_PROPERTY_SCOPE_TYPES(prop)	((prop)&=~MSK_INSTRUCTION_PROPERTY_SCOPE_TYPE)

	// BYTE_CODE_CALL TYPE
	#define	MSK_INSTRUCTION_PROPERTY_CHECK_IS_FUNCTION			(0x1<<BIT_INSTRUCTION_PROPERTY_CHECK_IS_FUNCTION)
	#define MASK_INSTRUCTION_PROPERTY_DIRECT_CALL_RETURN		(0x1<<BIT_INSTRUCTION_PROPERTY_DIRECT_CALL_RETURN)
	#define MSK_INSTRUCTION_PROPERTY_DEDUCE_C_CALL				(0x1<<BIT_INSTRUCTION_PROPERTY_DEDUCE_C_CALL)
	#define MSK_INSTRUCTION_PROPERTY_CONSTRUCT_CALL				(0x1<<BIT_INSTRUCTION_PROPERTY_CONSTRUCT_CALL)
	#define MSK_INSTRUCTION_PROPERTY_NO_FUNCTION_CALL			(0x1<<BIT_INSTRUCTION_PROPERTY_NO_FUNCTION_CALL)
	//#define MSK_INSTRUCTION_PROPERTY_CALL_TYPE					(((0x1<<(BIT_INSTRUCTION_PROPERTY_MAX-INS_PROPERTY_CALLING_OBJECT))-1)<<(INS_PROPERTY_CALLING_OBJECT))
	//#define GET_MSK_INSTRUCTION_PROPERTY_CALL_TYPE(prop)		((prop)&MSK_INSTRUCTION_PROPERTY_CALL_TYPE)

	#define MAIN_SCRIPT_FUNCTION_NAME 			"__MainFunction__"

	typedef enum {

		// built-in types...
		IDX_BUILTIN_TYPE_CLASS_MAIN = 0, 	// Main class ...

		// built in C types...
		IDX_BUILTIN_TYPE_VOID_C,
		IDX_BUILTIN_TYPE_INT_PTR_C,
		IDX_BUILTIN_TYPE_FLOAT_PTR_C,
		IDX_BUILTIN_TYPE_CONST_CHAR_PTR_C,
		IDX_BUILTIN_TYPE_STRING_PTR_C,
		IDX_BUILTIN_TYPE_BOOL_PTR_C,
		IDX_BUILTIN_TYPE_INT_C,
		IDX_BUILTIN_TYPE_UNSIGNED_INT_C,
		IDX_BUILTIN_TYPE_INTPTR_T_C,
		IDX_BUILTIN_TYPE_FLOAT_C,
		IDX_BUILTIN_TYPE_BOOL_C,

		// built in classes...
		IDX_BUILTIN_TYPE_STACK_ELEMENT,
		IDX_BUILTIN_TYPE_CLASS_SCRIPT_VAR,
		IDX_BUILTIN_TYPE_CLASS_STRING,
		IDX_BUILTIN_TYPE_CLASS_VECTOR,
		IDX_BUILTIN_TYPE_CLASS_FUNCTOR,
		IDX_BUILTIN_TYPE_CLASS_DICTIONARY,

		IDX_BUILTIN_TYPE_MAX
	}IdxBuiltInType;

	//typedef tInfoStatementOp *PInfoStatementOp;
	typedef enum {
		SYMBOL_INFO_PROPERTY_C_OBJECT_REF 					= 0x1 << 0,
		SYMBOL_INFO_PROPERTY_IS_DERIVATED 					= 0x1 << 1,
		SYMBOL_INFO_PROPERTY_STATIC_REF 					= 0x1 << 2, // C function or C++ static functions
		SYMBOL_INFO_PROPERTY_IS_POLYMORPHIC					= 0x1 << 3,
		SYMBOL_INFO_PROPERTY_SET_FIRST_PARAMETER_AS_THIS	= 0x1 << 4  // will pass object this as first parameter
	}SymbolInfoProperty;


	typedef enum
		:unsigned char{
		 SCOPE_PROPERTY_BREAK=0x1 << 0
		,SCOPE_PROPERTY_CONTINUE=0x1 << 1
		,SCOPE_PROPERTY_FOR_IN=0x1 << 2

	}ScopeProperty;


	typedef void  (* PrintFunctionCallback)(const char *filename, int line, const  char  *string_text);

	typedef intptr_t (*ConversionType)(intptr_t);



	//-----------------------------

	struct Symbol {
		//public:
		std::string file;
		short line;
		short idx_scope;

		std::string name;

		char n_params;

		Symbol(){
			file="";
			line=-1;

			idx_scope = ZS_UNDEFINED_IDX;
			name="";
			n_params = NO_PARAMS_IS_VARIABLE;
		}

		/*Symbol(const std::string & _name, char _n_params= NO_PARAMS_IS_VARIABLE){
			file=ZS_UNDEFINED_IDX;
			line=-1;
			idx_scope = ZS_UNDEFINED_IDX;

			name=_name;
			n_params=_n_params;
		}*/

		bool operator == (const Symbol & s1){
			return this->name == s1.name
				  && this->idx_scope == s1.idx_scope
				  && this->n_params == s1.n_params;
		}

		bool IsFunction(){
			return n_params != NO_PARAMS_IS_VARIABLE;
		}

		bool MatchSymbol(const Symbol & s1){
			return this->name == s1.name
				  && this->n_params == s1.n_params;
		}
	};

	//-----------------------------


	#pragma pack(push, 1)


	struct OpCodeInstructionSourceInfo {

			const char * file;
			short line;
			//Symbol * _symbol;
			std::string * str_symbol;

			OpCodeInstructionSourceInfo(){
				str_symbol=NULL;
				file="unknow_file";
				line=-1;
			}

			OpCodeInstructionSourceInfo(const char * _file, short _line,std::string *_str_symbol){

				file=_file;
				line=_line;
				str_symbol=_str_symbol;
			}
	};

	struct LinkSymbolFirstAccess{

		short idx_script_function;
		short idx_scope;
		std::string value;
		char n_params;

		LinkSymbolFirstAccess(){

			idx_script_function=ZS_UNDEFINED_IDX;
			idx_scope=ZS_UNDEFINED_IDX;
			value = "";
			n_params=ZS_UNDEFINED_IDX;
		}

		LinkSymbolFirstAccess(
				 int _idxScriptFunction
				,short _idxScope
				,const std::string & _value
				,char _n_params=0
				){
			idx_script_function=_idxScriptFunction;
			idx_scope=_idxScope;
			value=_value;
			n_params=_n_params;
		}
	};



	struct FunctionSymbol {

		StackElement object; // created object. undefined by default.
		intptr_t  proxy_ptr; // for proxy functions...
		//FunctionSymbol *super_function; // only for functions ...
		std::string key_value;
		//short idxAstNode; // in case there's ast node...

		FunctionSymbol() {
			proxy_ptr = 0;
			object= {

				0,// 0 value
				0,// no var ref related
				MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_UNDEFINED|MSK_STACK_ELEMENT_PROPERTY_IS_VAR_C // undefined and c ?.
			};
			key_value="";

			//idxAstNode = ZS_UNDEFINED_IDX;
			//super_function = NULL;
		}

	};

	//-------------------------------------------------------


	/**
	 * Scope register
	 */
	struct ScopeVarInnerBlockInfo {
		int *var_index;
		char n_var_index;
		short idx_scope;
	};

	typedef struct _SharedPointerInfo *PInfoSharedPointer;

	typedef struct _SharedPointerInfo {
		ScriptVar *shared_ptr;
		unsigned char n_shares;

		//short idx_0_shares;
		//PInfoSharedPointer next;
	} SharedPointerInfo;

	typedef struct _tNode * PInfoSharedPointerNode;
	typedef struct _tNode {
		SharedPointerInfo data;
		unsigned short currentStack;
		PInfoSharedPointerNode previous, next;
	} tInfoSharedPointerNode;

	typedef struct{
		int idx_type;
		std::string arg_name; //arg c++ type or arg name
	}ParamArgInfo;

	#pragma pack(pop)

}
