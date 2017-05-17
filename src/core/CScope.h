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


#define SCOPE_INFO_NODE(idx) 				CScope::getScopeNodeByIdx(idx)
#include "Common.h"

class CContext;




class CScope{

public:



	static void 					setVectorScopeNode(vector<CScope *> 	* set_vec);
	static vector<CScope *> 	* 	getVectorScopeNode();

	/**
	 * Get CScope Node by its idx, regarding current state.
	 */

	static CScope	    		*	newScope(int idx_parent_scope=ZS_UNDEFINED_IDX);
	static CScope 				* 	getScopeNodeByIdx(int idx);

	int idxScope;

	//---------------------------------
	// Register functions

	//--------------------------------------------------------------------
	// Register functions
	vector<tScopeVar> m_registeredVariableFromBase; // vars registered from base.
	vector<tScopeVar> m_registeredAnoymouseFunctionFromBase; // anonymous functions registered from base.

	tScopeVar * getInfoRegisteredSymbol(const string & v, bool print_msg=true);
	tScopeVar * registerAnonymouseFunction(PASTNode ast);
	tScopeVar * registerSymbol(const string & var_name, PASTNode ast=NULL);
	tScopeVar * existRegisteredSymbol(const string & var_name);


	CScope();
	CScope(int idx_this, int idx_parent=ZS_UNDEFINED_IDX);//, int _index);

	int 	 getIdxBaseScope();
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
	//int m_index;

	static vector<CScope *> 					* vec_scope_node;


};
