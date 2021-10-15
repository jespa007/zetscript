/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
#pragma once

#define SFI_GET_FILE_LINE(__FUNC__,__INS__) 	((zetscript::ScriptFunction *)__FUNC__)->getInstructionSourceFile(__INS__), ((zetscript::ScriptFunction *)__FUNC__)->getInstructionLine(__INS__)
#define SFI_GET_FILE(__FUNC__,__INS__) 			((zetscript::ScriptFunction *)__FUNC__)->getInstructionSourceFile(__INS__)
#define SFI_GET_LINE(__FUNC__,__INS__)			((zetscript::ScriptFunction *)__FUNC__)->getInstructionLine(__INS__)
#define SFI_GET_SYMBOL_NAME(__FUNC__,__INS__)	((zetscript::ScriptFunction *)__FUNC__)->getInstructionSymbolName(__INS__)

namespace zetscript{

	class ScriptFunctionFactory;
	class ScriptClassFactory;
	class ScopeFactory;
	class ZetScript;

	/**
	 * Script context refers to function context. A function can contain local variables and local functions
	 */
	class ScriptFunction {
	public:

		Symbol 	  			symbol;	 		// function registered by scope
		int					idx_class; 		// which class belongs to...
		short 				idx_script_function;		// idx_script_function from factory
		int 				idx_return_type; 			// idx return type
		zs_int 				ref_native_function_ptr;
		ScriptFunctionParam	*params;
		size_t 				params_len;
		int					min_stack_needed;

		PtrInstruction  instructions; // The set of byte code instructions that executes the function

		// number of instructions
		size_t			instructions_len;

		// local symbols for class or function...
		zs_vector   		*local_variables; // registered variable symbols
		//zs_vector   		*symbol_registered_functions; // registered variable symbols

		ScriptFunction(
				ZetScript *_zs
				,int _idx_class
				,short _idx_script_function
				, ScriptFunctionParam **_params
				,size_t _params_len

				,int  idx_return_type
				, Symbol *symbol
				, zs_int _ref_native_function_ptr
		);

		//-----------
		//  SYMBOL-INSTRUCTION
		zs_vector 			instruction_source_info; // std::map that gives symbol with at instruction idx given

		short 				getInstructionLine(Instruction * ins);
		const char * 		getInstructionSymbolName(Instruction * ins);
		const char *		getInstructionSourceFile(Instruction * ins);

		//  DEBUG
		//-----------

		static void printGeneratedCode(ScriptFunction *sfo,ScriptClass *sc=NULL);

		int existArgumentName(const zs_string & arg_name);


		/* Registers argument variable
		 * Desc: Inserts variable at scope some block scope or by scope info itself.
		 */
		Symbol *registerLocalArgument(
				 Scope * scope_block
				, const char * file
				, short line
				, const zs_string & symbol_name
				, uint16_t properties
		);

		/* Registers local variable
		 * Desc: Inserts variable at scope some block scope or by scope info itself.
		 */
		Symbol *  registerLocalVariable(
				 Scope * scope_block
				, const char * file
				, short line
				, const zs_string & symbol_name
				, const zs_string & str_native_type=""
				, zs_int ref_ptr=0
				, unsigned short properties=0
		);


		/* Registers a function.
		 * Desc: Inserts function at custom scope. It returns the idx std::vector element on symbol_info.scope_info.[vRegisteredFunction/vRegisteredVariables]
		 */
		Symbol * registerLocalFunction(
				  Scope * scope_block
				, const char * file
				, short line
				, const zs_string & function_name
				, ScriptFunctionParam **_params=NULL
				,size_t _params_len=0
				, int idx_return_type=ZS_IDX_UNDEFINED
				, zs_int ref_ptr=0
				, unsigned short properties=0
		);

		~ScriptFunction();

	protected:
		void updateParams(			ScriptFunctionParam **_params
				,size_t _params_len
);
		void clear();

	private:

		ZetScript 				*zs;
		ScriptFunctionFactory 	*script_function_factory;
		ScriptClassFactory 		*script_class_factory;
		ScopeFactory 			*scope_factory;	// reference scope_factory
		static zs_string formatInstructionLoadType(ScriptFunction *function,Instruction *instruction);
		InstructionSourceInfo * getInstructionInfo(Instruction *instruction);
		void clearParams();

	};
}
