#pragma once

namespace zetscript{


	class CBaseClassFunctionData { // script function is shared by class and function ...



	public:

		bool searchVarFunctionSymbol( string symbol_to_find,tInstruction *iao, int current_function, bool & symbol_not_found, unsigned int param_scope_type, int n_args_to_find=-1);


		// local symbols for class or function...
		std::vector<tVariableSymbolInfo> m_variable; // member variables to be copied in every new instance
		std::vector<CScriptFunction *>   m_function; // idx member functions (from main vector collection)

		tVariableSymbolInfo symbol_info; // class/function symbol info ...

		short idxScope; // idx function/class starting scope...
		short idxScriptClass; // wich idxClass class itself or function belongs to...



		CBaseClassFunctionData(short idxStartScope);

		/* Registers a function.
		 * Desc: Inserts function at custom scope. It returns the idx vector element on symbol_info.scope_info.[vRegisteredFunction/vRegisteredVariables]
		 */
		int registerFunction( short idxBlockScope, const string & function_name, vector<tArgumentInfo> args={}, int idx_return_type=ZS_UNDEFINED_IDX,intptr_t ref_ptr=0, unsigned short properties=0);
		int registerFunction( const string & function_name, vector<tArgumentInfo> args={}, int idx_return_type=ZS_UNDEFINED_IDX,intptr_t ref_ptr=0, unsigned short properties=0);
		int getFunction(const string & function_ref,char n_args=0);

		/* Registers local variable
		 * Desc: Inserts variable at scope some block scope or by scope info itself.
		 */
		int  registerVariable( short idxBlockScope,const string & variable,const string & variable_ref, const string & c_type="", intptr_t ref_ptr=0, unsigned short properties=0);
		int  registerVariable(const string & variable,const string & variable_ref, const string & c_type="", intptr_t ref_ptr=0, unsigned short properties=0);
		int  getVariable(const string & variable_ref);



	};
}
