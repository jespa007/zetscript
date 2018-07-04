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

// if 0 is in main <> 0, else.
#define SCOPE_IN_MAIN_CLASS(idx)			((CScope::getScopeNodeByIdx(idx))->getIdxBaseScope()==0)


#define SCOPE_LNK_SYM		"@lnk"
#define SCOPE_VAR_TYPE		"var"
#define SCOPE_FUN_TYPE		"fun"
#define GET_SYMBOL_NAME(s)	(CScope::getSymbolNameFromSymbolRef(s))


	class CContext;
	class  CScope{

		tScopeVar * existRegisteredSymbolRecursive(const string & var_name, int n_params=-1);
		tScopeVar * existRegisteredSymbolRecursiveDownScope(const string & ref_symbol);
		tScopeVar * existRegisteredSymbolRecursiveUpScope(const string & ref_symbol);
	public:



		static void 					setVectorScopeNode(vector<CScope *> 	* set_vec);
		static vector<CScope *> 	* 	getVectorScopeNode();

		/**
		 * Get CScope Node by its idx, regarding current state.
		 */

		static CScope	    		*	newScope(int idx_parent_scope=ZS_UNDEFINED_IDX, short idx_base_ast_node=ZS_UNDEFINED_IDX);
		static CScope 				* 	getScopeNodeByIdx(int idx);

		// ref symbol management...
		static string 					makeSymbolVarRef(const string & symbol_name, int idxScope);
		static string 					makeSymbolFunctionRef(const string & symbol_name, int idxScope, int n_params=0);
		static string 					getSymbolNameFromSymbolRef(const string & ref_symbol);
		//static int	 					getIdxScopeFromSymbolRef(const string & ref_symbol);
		//static int	 					getParamsFromSymbolRef(const string & ref_symbol);

		int idxScope;

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
		tScopeVar * registerAnonymouseFunction(PASTNode ast);
		tScopeVar * registerSymbol(const string & var_name, PASTNode ast=NULL, int n_params=-1);

		tScopeVar * existRegisteredSymbol(const string & var_name, int n_params=-1);

		CScope();
		CScope(int idx_this, int idx_parent=ZS_UNDEFINED_IDX, short idx_base_ast_node=ZS_UNDEFINED_IDX);//, int _index);

		int 	 getIdxBaseScope();
		short 	 getIdxBaseAstNode();
		int		 getIdxParent();
		CScope * getCurrentScopePointer();
		int		 getIdxCurrentScopePointer();
		//void         generateScopeList(vector<CScope *> & vector);
		//int          getIndex();
		vector<int> * getLocalScopeList();

		vector<tScopeVar *> * getRegisteredVariableList();

		//int incTotalScopes();

		CScope * pushScope();
		CScope * popScope();

		//void destroyScopes();

		void resetScopePointer();

		~CScope();

	private:

		//void deleteScopeRecursive(CScope *scope_info);

		//void generateScopeListRecursive(CScope * scope, vector<CScope *> & vector);
		//static int getScopeIndexRecursive(CScope * current_scope, CScope *scope_to_find,int & _index);

		vector<int> m_localScopeList;

		// The a parent scope ...
		int idxParentScope, idxCurrentScopePointer, idxBaseScope;
		short idxBaseAstNode;
		//int m_index;

		static vector<CScope *> 					* vec_scope_node;


	};
}
