/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
#pragma once

typedef enum{
	FUNCTION_PROPERTY_C_OBJECT_REF			= 0x0001, // function is C
	FUNCTION_PROPERTY_STATIC				= 0x0002,
	FUNCTION_PROPERTY_MEMBER_FUNCTION		= 0x0004,// Member function
	FUNCTION_PROPERTY_DEDUCE_AT_RUNTIME		= 0x0008,// Deduce runtime functions becuse is C and it has functions with same name but different signature
}FunctionProperty;


#define SFI_GET_FILE_LINE(__FUNC__,__INS__) 	((zetscript::ScriptFunction *)__FUNC__)->getInstructionSourceFile(__INS__), ((zetscript::ScriptFunction *)__FUNC__)->getInstructionLine(__INS__)
#define SFI_GET_FILE(__FUNC__,__INS__) 			((zetscript::ScriptFunction *)__FUNC__)->getInstructionSourceFile(__INS__)
#define SFI_GET_LINE(__FUNC__,__INS__)			((zetscript::ScriptFunction *)__FUNC__)->getInstructionLine(__INS__)
#define SFI_GET_SYMBOL_NAME(__FUNC__,__INS__)	((zetscript::ScriptFunction *)__FUNC__)->getInstructionSymbolName(__INS__)

#define FUNCTION_PARAMETER_COUNT_MAX			16
#define FUNCTION_RETURN_COUNT_MAX				16

namespace zetscript{

	class ScriptFunctionFactory;
	class ScriptTypeFactory;
	class ScopeFactory;
	class ZetScript;

	/**
	 * Script context refers to function context. A function can contain local variables and local functions
	 */
	class ScriptFunction {
	public:

		// script function properties
		zs_string			name_script_function;
		Scope				*scope_script_function;
		int 				idx_script_function;	// idx_script_function from factory

		// script type
		int 				idx_script_type_return; 		// idx return type
		int					idx_script_type_owner; 				// which type belongs to...

		uint16_t			properties;

		int					idx_position;			// idx position in array

		zs_int 				ref_native_function_ptr;
		ScriptFunctionParam	*params;
		int 				params_len;
		int					min_code_stack_needed;

		PtrInstruction  	instructions; // The set of byte code instructions that executes the function

		// number of instructions
		size_t				instructions_len;

		// local symbols for type or function...
		zs_vector<Symbol *> *local_variables; // registered variable symbols

		ScriptFunction(
				ZetScript *_zs
				,int _idx_script_function
				,int _idx_script_type
				,int _idx_position
				,const zs_string & name_script_function
				, ScriptFunctionParam **_params
				, int _params_len
				,int  idx_script_type_return
				, zs_int _ref_native_function_ptr
				, uint16_t _properties
		);

		//-----------
		//  SYMBOL-INSTRUCTION
		zs_vector<InstructionSourceInfo *> 			instruction_source_infos; // std::map that gives symbol with at instruction idx given

		short 				getInstructionLine(Instruction * ins);
		const char * 		getInstructionSymbolName(Instruction * ins);
		const char *		getInstructionSourceFile(Instruction * ins);

		//  DEBUG
		//-----------

		static void printGeneratedCode(ScriptFunction *sfo,ScriptType *sc=NULL);
		static void checkNativeFunctionParams(Scope *_scope,int _idx_return_type, const zs_string & _function_name,ScriptFunctionParam *_params,int _params_len);


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
		 * Desc: Inserts function at custom scope. It returns the idx zs_vector element on symbol_info.scope_info.[vRegisteredFunction/vRegisteredVariables]
		 */
		Symbol * registerLocalFunction(
				  Scope * scope_block
				, const char * file
				, short line
				, const zs_string & name_script_function
				, ScriptFunctionParam **_params=NULL
				,char _params_len=0
				, int idx_script_type_return=ZS_IDX_UNDEFINED
				, zs_int ref_ptr=0
				, unsigned short properties=0
		);

		void removeUnusuedScopes();



		bool linkUnresolvedSymbols();
		void clearUnresolvedSymbols();

		void addUnresolvedSymbol(zs_int instruction);
		ZetScript *getZetScript();

		~ScriptFunction();

	protected:
		void updateParams(			ScriptFunctionParam **_params
				,int _params_len
);
		void clear();

	private:

		ZetScript 				*zs;
		ScriptFunctionFactory 	*script_function_factory;
		ScriptTypeFactory 		*script_type_factory;
		ScopeFactory 			*scope_factory;	// reference scope_factory
		zs_vector<zs_int>		unresolved_symbols; // UnresolvedInstructionInfo
		static zs_string 		formatInstructionLoadType(ScriptFunction *function,Instruction *instruction);
		InstructionSourceInfo * getInstructionInfo(Instruction *instruction);
		void clearParams();

	};
}
