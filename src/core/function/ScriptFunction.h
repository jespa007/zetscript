/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
#pragma once

typedef enum{
	SCRIPT_FUNCTION_PROPERTY_NATIVE_OBJECT_REF	= 0x0001, // function is C
	SCRIPT_FUNCTION_PROPERTY_STATIC				= 0x0002,
	SCRIPT_FUNCTION_PROPERTY_MEMBER_FUNCTION	= 0x0004,// Member function
	SCRIPT_FUNCTION_PROPERTY_DEDUCE_AT_RUNTIME	= 0x0008,// Deduce runtime functions becuse is C and it has functions with same name but different signature
}ScriptFunctionProperty;


#define SFI_GET_FILE_LINE(__FUNC__,__INS__) 	((zetscript::ScriptFunction *)__FUNC__)->getInstructionSourceFile(__INS__), ((zetscript::ScriptFunction *)__FUNC__)->getInstructionLine(__INS__)
#define SFI_GET_FILE(__FUNC__,__INS__) 			((zetscript::ScriptFunction *)__FUNC__)->getInstructionSourceFile(__INS__)
#define SFI_GET_LINE(__FUNC__,__INS__)			((zetscript::ScriptFunction *)__FUNC__)->getInstructionLine(__INS__)
#define SFI_GET_SYMBOL_NAME(__FUNC__,__INS__)	((zetscript::ScriptFunction *)__FUNC__)->getInstructionSymbolName(__INS__)

#define ZS_SCRIPT_FUNCTION_PARAMETER_COUNT_MAX			16
#define ZS_SCRIPT_FUNCTION_RETURN_COUNT_MAX				16

namespace zetscript{

	class ScriptFunctionsFactory;
	class ScriptTypesFactory;
	class ScriptScopesFactory;
	class ScriptEngine;

	/**
	 * Script context refers to function context. A function can contain local variables and local functions
	 */
	class ScriptFunction {
	public:

		// script function properties
		int 				id;	// id from factory
		String				name;
		ScriptScope				*scope;


		// script type
		ScriptTypeId				return_script_type_id; 		// idx return type
		ScriptTypeId				owner_script_type_id; 				// which type belongs to...

		uint16_t			properties;

		int					idx_position;			// idx position in array

		zs_int 				ref_native_function_ptr;
		FunctionParam	*params;
		int 				params_len;
		int					min_code_stack_needed;

		PtrInstruction  	instructions; // The set of byte code instructions that executes the function
		MapInt			instruction_last_native_call;

		// number of instructions
		size_t				instructions_len;

		// local symbols for type or function...
		Vector<Symbol *> *local_variables; // registered variable symbols

		ScriptFunction(
				ScriptEngine *_se
				,int _idx_script_function
				,ScriptTypeId _owner_script_type_id
				,int _idx_position
				,const String & name
				, FunctionParam **_params
				, int _params_len
				,ScriptTypeId  return_script_type_id
				, zs_int _ref_native_function_ptr
				, uint16_t _properties
		);

		//-----------
		//  SYMBOL-INSTRUCTION
		Vector<InstructionSourceInfo *> 			instruction_source_infos; // std::map that gives symbol with at instruction idx given

		short 				getInstructionLine(Instruction * ins);
		const char * 		getInstructionSymbolName(Instruction * ins);
		const char *		getInstructionSourceFile(Instruction * ins);
		ScriptFunction	*	getInstructionFunctionLastCall(Instruction * ins);

		// It has to be called when function is evaluated
		void				clearFunctionLastCalls();
		void				setInstructionFunctionLastCall(Instruction * ins,ScriptFunction *sf);

		//  DEBUG
		//-----------

		static void printGeneratedCode(ScriptFunction *sfo,ScriptType *sc=NULL);
		static void checkNativeFunctionParams(ScriptScope *_scope,int _idx_return_type, const String & _function_name,FunctionParam *_params,int _params_len);


		int existArgumentName(const String & arg_name);


		/* Registers argument variable
		 * Desc: Inserts variable at scope some block scope or by scope info itself.
		 */
		Symbol *registerLocalArgument(
				 ScriptScope * scope_block
				, const char * file
				, short line
				, const String & symbol_name
				, uint16_t properties
		);

		/* Registers local variable
		 * Desc: Inserts variable at scope some block scope or by scope info itself.
		 */
		Symbol *  registerLocalVariable(
				 ScriptScope * scope_block
				, const char * file
				, short line
				, const String & symbol_name
				, unsigned short properties=0
		);


		/* Registers a function.
		 * Desc: Inserts function at custom scope. It returns the idx Vector element on symbol_info.scope_info.[vRegisteredFunction/vRegisteredVariables]
		 */
		Symbol * registerLocalFunction(
				  ScriptScope * scope_block
				, const char * file
				, short line
				, const String & name
				, FunctionParam **_params=NULL
				,int8_t _params_len=0
				, ScriptTypeId _return_script_type_id=SCRIPT_TYPE_ID_INVALID
				, zs_int ref_ptr=0
				, unsigned short properties=0
		);

		void removeUnusuedScopes();



		bool linkUnresolvedSymbols();
		void clearUnresolvedSymbols();

		void addUnresolvedSymbol(zs_int instruction);
		ScriptEngine *getScriptEngine();

		~ScriptFunction();

	protected:
		void updateParams(			FunctionParam **_params
				,int _params_len
);
		void clear();

	private:

		ScriptEngine 				*zs;
		ScriptFunctionsFactory 	*script_function_factory;
		ScriptTypesFactory 		*script_types_factory;
		ScriptScopesFactory 			*scope_factory;	// reference scope_factory
		Vector<zs_int>		unresolved_symbols; // UnresolvedInstructionInfo
		static String 		formatInstructionLoadType(ScriptFunction *function,Instruction *instruction);
		InstructionSourceInfo * getInstructionInfo(Instruction *instruction);
		void clearParams();

	};
}
