#pragma once

namespace zs{


	class CScriptClassBase { // script function is shared by class and function ...



	public:

		bool searchVarFunctionSymbol( std::string symbol_to_find,tInstruction *iao, int current_function, bool & symbol_not_found, unsigned int param_scope_type, int n_args_to_find=-1);


		// local symbols for class or function...
		std::vector<tVariableSymbolInfo> m_variable; // member variables to be copied in every new instance
		std::vector<CScriptFunction *>   m_function; // idx member functions (from main std::vector collection)

		tVariableSymbolInfo symbol_info; // class/function symbol info ...

		unsigned char idxClass; // wich idxClass class itself or function belongs to...

		CScriptClassBase(unsigned char _idxClass);

		/* Registers local variable
		 * Desc: Inserts variable at scope some block scope or by scope info itself.
		 */
		tVariableSymbolInfo *  registerVariable(const std::string & file, short line,  short idxBlockScope,const std::string & variable, const std::string & c_type="", intptr_t ref_ptr=0, unsigned short properties=0);
		tVariableSymbolInfo *  registerVariable(const std::string & file, short line, const std::string & variable, const std::string & c_type="", intptr_t ref_ptr=0, unsigned short properties=0);
		tVariableSymbolInfo *  getVariable(const std::string & symbol_ref, short idxScope);


		/* Registers a function.
		 * Desc: Inserts function at custom scope. It returns the idx std::vector element on symbol_info.scope_info.[vRegisteredFunction/vRegisteredVariables]
		 */
		CScriptFunction * registerFunction(const std::string & file, short line,  short idxBlockScope, const std::string & function_name, std::vector<tArgumentInfo> args={}, int idx_return_type=ZS_UNDEFINED_IDX,intptr_t ref_ptr=0, unsigned short properties=0);
		CScriptFunction * registerFunction(const std::string & file, short line,  const std::string & function_name, std::vector<tArgumentInfo> args={}, int idx_return_type=ZS_UNDEFINED_IDX,intptr_t ref_ptr=0, unsigned short properties=0);
		CScriptFunction * getFunction(const std::string & symbol, short idxScope,char n_args=0);


	};
}
