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


#define SCOPE_NODE(idx) 				CScope::getScopeNodeByIdx(idx)

#define IDX_GLOBAL_SCOPE				0
#define IDX_INVALID_SCOPE				ZS_UNDEFINED_IDX
#define IDX_ANONYMOUSE_SCOPE			-3

// if 0 is in main <> 0, else.
#define SCOPE_IN_MAIN_CLASS(idx)			((CScope::getScopeNodeByIdx(idx))->getIdxBaseScope()==IDX_GLOBAL_SCOPE)




#define NO_PARAMS_IS_VARIABLE	-1
#define NO_PARAMS_SYMBOL_ONLY	-2



	class CContext;
	class  CScope{

		tScopeVar * existRegisteredSymbolRecursive(const string & var_name, int n_params=NO_PARAMS_IS_VARIABLE);
		tScopeVar * existRegisteredSymbolRecursiveDownScope(const string & ref_symbol, int n_params=NO_PARAMS_IS_VARIABLE);
		tScopeVar * existRegisteredSymbolRecursiveUpScope(const string & ref_symbol, int n_params=NO_PARAMS_IS_VARIABLE);
	public:

		static void initStaticVars();
		static void destroyStaticVars();

		static vector<CScope *> 	* 	getVectorScopeNode();


		/**
		 * Get CScope Node by its idx, regarding current state.
		 */


		static CScope	    		*	newScope(short idx_parent_scope=ZS_UNDEFINED_IDX,bool is_c_node=false);
		static CScope 				* 	getScopeNodeByIdx(short idx);

		short idxScope;
		bool is_c_node;

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

		//CScope();
		CScope( short idx_this=ZS_UNDEFINED_IDX, short idx_parent=ZS_UNDEFINED_IDX);
		//CScope( short idx_this, short idx_parent=ZS_UNDEFINED_IDX);//, int _index);

		short 	 getIdxBaseScope();
		short 	 getIdxScriptClass();
		short 	 getIdxBaseAstNode();
		short		 getIdxParent();
		CScope * getCurrentScopePointer();
		short		 getIdxCurrentScopePointer();
		vector<short> * getLocalScopeList();

		vector<tScopeVar *> * getRegisteredVariableList();

		CScope * pushScope();
		CScope * popScope();
		void resetScopePointer();

		~CScope();

	private:

		vector<short> m_localScopeList;

		// The a parent scope ...
		short idxParentScope, idxCurrentScopePointer, idxBaseScope;


		/**
		 * Vector of script scopes. This vector is removed when zetscript reevaluates all scrips.
		 */
		static vector<CScope *> 					* vec_scope_node;

		/**
		 * Vector of C scopes that will be copyed each reeval of all scripts.
		 */
		static vector<CScope *> 					* vec_c_scope_node;



	};
}
