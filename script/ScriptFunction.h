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

		Symbol 	  		symbol;	 		// function registered by scope
		int				idx_class; 		// which class belongs to...
		short 			idx_script_function;		// idx_script_function from factory
		int 			idx_return_type; 			// idx return type
		zs_int 			ref_native_function_ptr;
		zs_vector 	* 	params;  // std::vector<FunctionArg> tells var arg name or var type name (in of C )
		int				min_stack_needed;

		PtrInstruction  instructions; // The set of byte code instructions that executes the function

		// number of instructions
		size_t			instructions_len;

		// local symbols for class or function...
		zs_vector   *	registered_symbols; // registered symbols

		ScriptFunction(
				ZetScript *_zs
				,int _idx_class
				,short _idx_script_function
				, std::vector<FunctionArg> _params
				,int  idx_return_type
				, Symbol *symbol
				, zs_int _ref_native_function_ptr
		);

		//-----------
		//  SYMBOL-INSTRUCTION
		std::map<short,InstructionSourceInfo> instruction_source_info; // std::map that gives symbol with at instruction idx given

		short 		 getInstructionLine(Instruction * ins);
		const char * getInstructionSymbolName(Instruction * ins);
		const char * getInstructionSourceFile(Instruction * ins);

		//  DEBUG
		//-----------

		static void printGeneratedCode(ScriptFunction *sfo,ScriptClass *sc=NULL);

		int existArgumentName(const std::string & arg_name);


		/* Registers argument variable
		 * Desc: Inserts variable at scope some block scope or by scope info itself.
		 */
		Symbol *registerLocalArgument(
				 Scope * scope_block
				, const std::string & file
				, short line
				, const std::string & symbol_name
		);

		/* Registers local variable
		 * Desc: Inserts variable at scope some block scope or by scope info itself.
		 */
		Symbol *  registerLocalVariable(
				 Scope * scope_block
				, const std::string & file
				, short line
				, const std::string & symbol_name
				, const std::string & str_native_type=""
				, zs_int ref_ptr=0
				, unsigned short properties=0
		);


		Symbol *  getSymbol(Scope *scope,const std::string & symbol_name, char n_params=NO_PARAMS_SYMBOL_ONLY);

		/* Registers a function.
		 * Desc: Inserts function at custom scope. It returns the idx std::vector element on symbol_info.scope_info.[vRegisteredFunction/vRegisteredVariables]
		 */
		Symbol * registerLocalFunction(
				  Scope * scope_block
				, const std::string & file
				, short line
				, const std::string & function_name
				, std::vector<FunctionArg> args={}
				, int idx_return_type=ZS_IDX_UNDEFINED
				, zs_int ref_ptr=0
				, unsigned short properties=0
		);

		~ScriptFunction();

	protected:
		void updateParams(std::vector<FunctionArg> args);
		void clear();

	private:

		ZetScript 				*zs;
		ScriptFunctionFactory 	*script_function_factory;
		ScriptClassFactory 		*script_class_factory;
		ScopeFactory 			*scope_factory;	// reference scope_factory
		static std::string formatInstructionLoadType(ScriptFunction *function,Instruction *instruction);
		InstructionSourceInfo * getInstructionInfo(Instruction *instruction);
		void clearParams();

	};
}
