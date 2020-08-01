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
		unsigned char idx_class; 		// which class belongs to...
		short idx_script_function;		// idx_script_function from factory
		int idx_return_type; 			// idx return type

		zs_vector * function_params;  // std::vector<FunctionParam> tells var arg name or var type name (in of C )

		PtrInstruction  instructions; // The set of byte code instructions that executes the function


		// local symbols for class or function...
		zs_vector   *	registered_symbols; // registered symbols
		zs_vector   *	registered_functions; // registered functions


		ScriptFunction(
				ZetScript *_zs
				,unsigned char _idx_class
				,short _idx_script_function
				, std::vector<FunctionParam> function_params
				,int  idx_return_type
				, Symbol *symbol
		);

		//-----------
		//  SYMBOL-INSTRUCTION
		std::map<short,InstructionSourceInfo> instruction_source_info; // std::map that gives symbol with at instruction idx given

		short 		 getInstructionLine(Instruction * ins);
		const char * getInstructionSymbolName(Instruction * ins);
		const char * getInstructionSourceFile(Instruction * ins);

		//  DEBUG
		//-----------

		static void printGeneratedCode(ScriptFunction *sfo);

		int existArgumentName(const std::string & arg_name);
		/*bool searchVarFunctionSymbol(
				std::string symbol_to_find,Instruction *iao
				, int current_function
				, bool & symbol_not_found
				, unsigned int param_scope_type
				, int n_args_to_find=-1
		);*/

		/* Registers local variable
		 * Desc: Inserts variable at scope some block scope or by scope info itself.
		 */
		Symbol *  addSymbol(
				 Scope * scope_block
				, const std::string & file
				, short line
				, const std::string & symbol_name
				, const std::string & c_type=""
				, intptr_t ref_ptr=0
				, unsigned short symbol_properties=0
		);

		/*ScopeSymbolInfo *  registerVariable(
				const std::string & file
				, short line
				, const std::string & variable
				, const std::string & c_type=""
				, intptr_t ref_ptr=0
				, unsigned short symbol_properties=0
		);*/

		Symbol *  getSymbol(Scope *scope,const std::string & symbol_name);

		/* Registers a function.
		 * Desc: Inserts function at custom scope. It returns the idx std::vector element on symbol_info.scope_info.[vRegisteredFunction/vRegisteredVariables]
		 */
		ScriptFunction * addFunction(
				  Scope * scope_block
				, const std::string & file
				, short line
				, const std::string & function_name
				, std::vector<FunctionParam> args={}
				, int idx_return_type=ZS_IDX_UNDEFINED
				, intptr_t ref_ptr=0
				, unsigned short symbol_properties=0
		);

		/*ScriptFunction * registerFunction(
				const std::string & file
				, short line
				,  const std::string & function_name
				, std::vector<FunctionParam> args={}
				, int idx_return_type=ZS_IDX_UNDEFINED
				,intptr_t ref_ptr=0
				, unsigned short symbol_properties=0
		);*/

		ScriptFunction * getFunction(Scope * scope,const std::string & symbol, char n_args=0);
		~ScriptFunction();

	private:
		ZetScript 				*zs;
		ScriptFunctionFactory 	*script_function_factory;
		ScriptClassFactory 		*script_class_factory;
		ScopeFactory 			*scope_factory;	// reference scope_factory


		static std::string formatInstructionLoadType(ScriptFunction *current_function,PtrInstruction list_statements, int current_instruction);
		inline InstructionSourceInfo * getInstructionInfo(Instruction *ins){
			short idx= (ins-this->instructions);///sizeof(Instruction *);
			if(instruction_source_info.count(idx)==1){
				return &instruction_source_info[idx];
			}
			return NULL;
		}

	};
}
