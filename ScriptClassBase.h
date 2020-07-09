#pragma once

namespace zetscript{

	class ScriptFunctionFactory;
	class ScriptClassFactory;
	class ZetScript;
	class ScriptClassBase { // script function is shared by class and function ...



	public:

		ZetScript *zs;
		ScopeFactory *scope_factory;
		VirtualMachine *virtual_machine;
		ScriptFunctionFactory *script_function_factory;
		ScriptClassFactory *script_class_factory;

		bool searchVarFunctionSymbol( std::string symbol_to_find,Instruction *iao, int current_function, bool & symbol_not_found, unsigned int param_scope_type, int n_args_to_find=-1);


		// local symbols for class or function...
		std::vector<SymbolInfo> local_variable; // member variables to be copied in every new instance
		std::vector<ScriptFunction *>   local_function; // idx member functions (from main std::vector collection)

		SymbolInfo symbol_info; // class/function symbol info ...

		unsigned char idx_class; // wich idx_class class itself or function belongs to...

		ScriptClassBase(ZetScript *_zs,unsigned char _idx_class);

		/* Registers local variable
		 * Desc: Inserts variable at scope some block scope or by scope info itself.
		 */
		SymbolInfo *  RegisterVariable(const std::string & file, short line,  short idxBlockScope,const std::string & variable, const std::string & c_type="", intptr_t ref_ptr=0, unsigned short symbol_info_properties=0);
		SymbolInfo *  RegisterVariable(const std::string & file, short line, const std::string & variable, const std::string & c_type="", intptr_t ref_ptr=0, unsigned short symbol_info_properties=0);
		SymbolInfo *  getVariable(const std::string & symbol_ref, short idx_scope);


		/* Registers a function.
		 * Desc: Inserts function at custom scope. It returns the idx std::vector element on symbol_info.scope_info.[vRegisteredFunction/vRegisteredVariables]
		 */
		ScriptFunction * registerFunction(const std::string & file, short line,  short idxBlockScope, const std::string & function_name, std::vector<ParamArgInfo> args={}, int idx_return_type=ZS_UNDEFINED_IDX,intptr_t ref_ptr=0, unsigned short symbol_info_properties=0);
		ScriptFunction * registerFunction(const std::string & file, short line,  const std::string & function_name, std::vector<ParamArgInfo> args={}, int idx_return_type=ZS_UNDEFINED_IDX,intptr_t ref_ptr=0, unsigned short symbol_info_properties=0);
		ScriptFunction * getFunction(const std::string & symbol, short idx_scope,char n_args=0);



	};
}
