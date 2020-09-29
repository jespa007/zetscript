/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
#pragma once


#ifdef __DEBUG__ // incoment __ZETSCRIPT_VERBOSE_MESSAGE__ to print all messages (wrning is going to be slow because of the prints)
#define __ZETSCRIPT_VERBOSE_MESSAGE__
#endif


#define ZS_IDX_UNDEFINED 							-1
//#define ZS_FUNCTION_NOT_FOUND_IDX	 				-2


#define MAX_NATIVE_FUNCTION_ARGS						 6

#define DEFAULT_NO_FILENAME				"no_file"
#define FUNCTION_MEMBER_CONSTRUCTOR_NAME "constructor"

// HELPER FUNCTIONSs

#define REGISTER_NATIVE_BASE_SYMBOLS(script_class_factory,o)		   			(zs)->registerNativeBaseSymbols(o))

// if 0 is in main <> 0, else.
#define DO_CAST																	((this->zs))->doCast
#define GET_IDX_BUILTIN_TYPE_CLASS_FROM_ITS_C_TYPE								((this->zs))->getIdxClassFromItsNativeType
#define INSTANCE_SCRIPT_VARIABLE_BY_IDX											((this->zs))->instanceScriptVariableByIdx
#define GET_METAMETHOD(m)														((this->zs))->getMetamethod(m)
#define IS_IDX_BUILTIN_TYPE_CLASS_INSTANCEOF(zs,idx_class,idx_class_instance)	((this->script_class_factory))->isClassInheritsFrom(idx_class,idx_class_instance)


namespace zetscript{

	//typedef CASTNode *PASTNode;
	class ScriptFunction;
	class Scope;
	class ScriptClass;
	class ScriptVar;
	struct Symbol;

	//struct ScopeBlockVars;
	typedef unsigned char 									ClassTypeIdx;

	typedef enum
		:unsigned char {

		LOAD_TYPE_NOT_DEFINED = 0,
		LOAD_TYPE_NULL,
		LOAD_TYPE_UNDEFINED,
		LOAD_TYPE_CONSTANT,
		LOAD_TYPE_VARIABLE,
		LOAD_TYPE_FUNCTION,
	}LoadType;


	typedef enum:ClassTypeIdx{

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
		IDX_BUILTIN_TYPE_FUNCTION,
		IDX_BUILTIN_TYPE_CLASS_SCRIPT_VAR,
		IDX_BUILTIN_TYPE_CLASS_STRING,
		IDX_BUILTIN_TYPE_CLASS_VECTOR,
		IDX_BUILTIN_TYPE_CLASS_DICTIONARY,

		IDX_BUILTIN_TYPE_MAX
	}IdxBuiltInType;


	/*typedef enum
		:unsigned char{
		 SCOPE_PROPERTY_BREAK		=0x1 << 0
		,SCOPE_PROPERTY_CONTINUE	=0x1 << 1
		,SCOPE_PROPERTY_FOR_IN		=0x1 << 2
	}ScopeProperty;*/

	typedef void  (* PrintFunctionCallback)(const char *filename, int line, const  char  *string_text);

	typedef intptr_t (*ConversionType)(intptr_t);
	//-----------------------------


	//-----------------------------
	#pragma pack(push, 1)

	struct LinkSymbolFirstAccess{

		short idx_script_function;
		Scope *scope;
		std::string value;
		char n_params;

		LinkSymbolFirstAccess(){

			idx_script_function=ZS_IDX_UNDEFINED;
			scope=NULL;
			value = "";
			n_params=ZS_IDX_UNDEFINED;
		}

		LinkSymbolFirstAccess(
				 int _idx_script_function
				,Scope *_scope
				,const std::string & _value
				,char _n_params=0
				){
			idx_script_function=_idx_script_function;
			scope=_scope;
			value=_value;
			n_params=_n_params;
		}
	};

	//-------------------------------------------------------

	typedef struct _SharedPointerInfo *PInfoSharedPointer;

	typedef struct _SharedPointerInfo {
		ScriptVar *shared_ptr;
		unsigned char n_shares;
	} SharedPointerInfo;

	typedef struct _tNode * PInfoSharedPointerNode;
	typedef struct _tNode {
		SharedPointerInfo data;
//		unsigned short currentStack;
		PInfoSharedPointerNode previous, next;
	} InfoSharedPointerNode;

	typedef struct{
		int idx_type;
		std::string arg_name; //arg c++ type or arg name
		int line;
		bool by_ref;
	}FunctionParam;

	#pragma pack(pop)


}
