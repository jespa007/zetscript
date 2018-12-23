/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
#pragma once

/*
//   _____       __        _____                        _   _
//	|_   _|     / _|      /  ___|                      | | | |
// 	  | | _ __ | |_ ___   \ `--.  ___ ___  _ __   ___  | | | | __ _ _ __
// 	  | || '_ \|  _/ _ \   `--. \/ __/ _ \| '_ \ / _ \ | | | |/ _` | '__|
//   _| || | | | || (_) | /\__/ / (_| (_) | |_) |  __/ \ \_/ / (_| | |
//   \___/_| |_|_| \___/  \____/ \___\___/| .__/ \___|  \___/ \__,_|_|
//                                        | |
//                                        |_|
// 	_________________________________________________
// 	 	__________________________________
*/

namespace zetscript{


	#define NO_PARAMS_IS_VARIABLE	-1
	#define NO_PARAMS_SYMBOL_ONLY	-2


	class  CScope{

		tScopeVar * existRegisteredSymbolRecursive(const string & var_name, int n_params=NO_PARAMS_IS_VARIABLE);
		tScopeVar * existRegisteredSymbolRecursiveDownScope(const string & ref_symbol, int n_params=NO_PARAMS_IS_VARIABLE);
		tScopeVar * existRegisteredSymbolRecursiveUpScope(const string & ref_symbol, int n_params=NO_PARAMS_IS_VARIABLE);
	public:


		short idxScope;
		bool is_c_node;
		CScriptClass *script_class;

		//---------------------------------
		// Register functions

		//--------------------------------------------------------------------
		// Register functions
		vector<tScopeVar> m_registeredVariableFromBase; // vars registered from base.
		vector<tScopeVar> m_registeredAnoymouseFunctionFromBase; // anonymous functions registered from base.

		/**
		 * register/search symbol info
		 * @n_params:
		 */
		tScopeVar * getInfoRegisteredSymbol(const string & v, int n_params=-1, bool print_msg=true);
		tScopeVar * registerAnonymouseFunction(const string & file, int line,int n_args=0);
		tScopeVar * registerSymbol(const string & file, int line,const string & var_name, int n_params=NO_PARAMS_IS_VARIABLE);

		tScopeVar * existRegisteredSymbol(const string & var_name, int n_params=NO_PARAMS_IS_VARIABLE);

		CScope(short idx_this=ZS_UNDEFINED_IDX, short idx_parent=ZS_UNDEFINED_IDX,bool is_c_node=false);

		short 	 	getIdxBaseScope();
		short	 	getIdxParent();
		CScope * 	getCurrentScopePointer();
		short		getIdxCurrentScopePointer();


		void setScriptClass(CScriptClass *sc);
		CScriptClass * getScriptClass();

		vector<tScopeVar *> * getRegisteredVariableList();
		vector<short> 		* getLocalScopeList();

		CScope * pushScope();
		CScope * popScope();
		void resetScopePointer();

		~CScope();

	private:
		static int n_anonymouse_func;
		vector<short> m_localScopeList;

		// The a parent scope ...
		short idxParentScope, idxCurrentScopePointer, idxBaseScope;

	};
}
