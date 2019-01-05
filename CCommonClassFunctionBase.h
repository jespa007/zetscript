#pragma once

namespace zetscript{


	class CCommonClassFunctionBase { // script function is shared by class and function ...



	public:

		bool searchVarFunctionSymbol( string symbol_to_find,tInstruction *iao, int current_function, bool & symbol_not_found, unsigned int param_scope_type, int n_args_to_find=-1);


		// local symbols for class or function...
		std::vector<tVariableSymbolInfo> m_variable; // member variables to be copied in every new instance
		std::vector<CScriptFunction *>   m_function; // idx member functions (from main vector collection)

		tVariableSymbolInfo symbol_info; // class/function symbol info ...

		unsigned char idxClass; // wich idxClass class itself or function belongs to...

		CCommonClassFunctionBase(unsigned char _idxClass);

		/* Registers local variable
		 * Desc: Inserts variable at scope some block scope or by scope info itself.
		 */
		tVariableSymbolInfo *  registerVariable(const string & file, short line,  short idxBlockScope,const string & variable, const string & c_type="", intptr_t ref_ptr=0, unsigned short properties=0);
		tVariableSymbolInfo *  registerVariable(const string & file, short line, const string & variable, const string & c_type="", intptr_t ref_ptr=0, unsigned short properties=0);
		tVariableSymbolInfo *  getVariable(const string & symbol_ref, short idxScope);


		/* Registers a function.
		 * Desc: Inserts function at custom scope. It returns the idx vector element on symbol_info.scope_info.[vRegisteredFunction/vRegisteredVariables]
		 */
		CScriptFunction * registerFunction(const string & file, short line,  short idxBlockScope, const string & function_name, vector<tArgumentInfo> args={}, int idx_return_type=ZS_UNDEFINED_IDX,intptr_t ref_ptr=0, unsigned short properties=0);
		CScriptFunction * registerFunction(const string & file, short line,  const string & function_name, vector<tArgumentInfo> args={}, int idx_return_type=ZS_UNDEFINED_IDX,intptr_t ref_ptr=0, unsigned short properties=0);
		CScriptFunction * getFunction(const string & symbol, short idxScope,char n_args=0);


	};
}
