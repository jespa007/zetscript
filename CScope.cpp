/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
#include "CZetScript.h"

#define MAX_STATMENT_LENGTH 2096
#define MAX_VAR_LENGTH 100



/*
//	 _____                        _____       __
//	/  ___|                      |_   _|     / _|
//	\ `--.  ___ ___  _ __   ___    | | _ __ | |_ ___
//	 `--. \/ __/ _ \| '_ \ / _ \   | || '_ \|  _/ _ \
//	/\__/ / (_| (_) | |_) |  __/  _| || | | | || (_) |
//	\____/ \___\___/| .__/ \___|  \___/_| |_|_| \___/
//               | |
//               |_|
// _________________________________________________
//  __________________________________
*/


namespace zetscript{


	void  writeErrorMsg(const char *filename, int line, const  char  *string_text, ...);


	vector<CScope *> 						* CScope::vec_scope_node=NULL;
	int n_anonymouse_func=0;



	/*int	 		CScope::getIdxScopeFromSymbolRef(const string & ref_symbol){

		int idxScope=ZS_INVALID_IDX;

		if(strncmp("@lnk",ref_symbol.c_str(),4)!=0){
					THROW_RUNTIME_ERROR("not symbol ref (expected @lnk_)");
					return idxScope;
		}

		return idxScope;


	}*/


	void 						CScope::setVectorScopeNode(vector<CScope *> 	* set_vec){
		vec_scope_node = set_vec;
	}


	vector<CScope *> 	*		CScope::getVectorScopeNode(){
		return vec_scope_node;
	}

	CScope *	 CScope::newScope(int idxParentScope, short idxBaseAstNode){
		CScope * scope_node = new CScope(vec_scope_node->size(), idxParentScope,idxBaseAstNode);
		vec_scope_node->push_back(scope_node);
		//scope_node->idxScope = vec_scope_node->size()-1;
		return scope_node;
	}

	CScope 		* CScope::getScopeNodeByIdx(int idx){
		if(idx < 0 || (unsigned)idx >= vec_scope_node->size()){
			THROW_RUNTIME_ERROR("CScope node out of bound");
			return NULL;
		}

		return vec_scope_node->at(idx);
	}
	//------------------------------------------------------------------------------------------------
	CScope::CScope(){
		idxParentScope = ZS_UNDEFINED_IDX;
		idxCurrentScopePointer=ZS_UNDEFINED_IDX;
		idxScope = ZS_UNDEFINED_IDX;
		idxBaseScope =ZS_UNDEFINED_IDX;
		idxBaseAstNode =ZS_UNDEFINED_IDX;
	}

	CScope::CScope(int idx_this, int idx_parent, short idx_base_ast_node){//, int _index){
		idxParentScope = idx_parent;
		//m_index = _index;
		idxCurrentScopePointer=ZS_UNDEFINED_IDX;
		//m_baseScope = this;
		idxScope = idx_this;

		if(idx_parent == ZS_UNDEFINED_IDX){ // first node...
			idxBaseAstNode = idx_base_ast_node;
			idxBaseScope = idx_this;
			idxCurrentScopePointer=idx_this;
		}else{
			idxBaseScope = SCOPE_NODE( idx_parent)->getIdxBaseScope();
			idxBaseAstNode = SCOPE_NODE( idx_parent)->getIdxBaseAstNode();
		}
	}

	short CScope::getIdxBaseAstNode(){
		return idxBaseAstNode;
	}

	int CScope::getIdxBaseScope(){
		return idxBaseScope;
	}

	int CScope::getIdxParent(){
		return idxParentScope;
	}

	int CScope::getIdxCurrentScopePointer(){
		return SCOPE_NODE(idxBaseScope)->idxCurrentScopePointer;
	}

	CScope * CScope::getCurrentScopePointer(){
		return SCOPE_NODE(SCOPE_NODE(idxBaseScope)->idxCurrentScopePointer);
	}


	void CScope::resetScopePointer(){
		SCOPE_NODE(idxBaseScope)->idxCurrentScopePointer = idxBaseScope;
	}

	CScope * CScope::pushScope(){

		CScope *new_scope = CScope::newScope(SCOPE_NODE(idxBaseScope)->idxCurrentScopePointer);//, m_baseScope->incTotalScopes());
		SCOPE_NODE(SCOPE_NODE(idxBaseScope)->idxCurrentScopePointer)->m_localScopeList.push_back(new_scope->idxScope);
		SCOPE_NODE(idxBaseScope)->idxCurrentScopePointer = new_scope->idxScope;
		return new_scope;

	}

	CScope * CScope::popScope(){

		CScope *current_scope = SCOPE_NODE(SCOPE_NODE(idxBaseScope)->idxCurrentScopePointer);
		if(current_scope->idxParentScope != ZS_UNDEFINED_IDX){

			SCOPE_NODE(idxBaseScope)->idxCurrentScopePointer = current_scope->idxParentScope;
			//m_baseScope->m_currentScopePointer = m_baseScope->m_currentScopePointer->m_parentScope;
			return SCOPE_NODE(SCOPE_NODE(idxBaseScope)->idxCurrentScopePointer);
		}

		return NULL;
	}

	vector<int> * CScope::getLocalScopeList(){

		return &m_localScopeList;
	}


	//-----------------------------------------------------------------------------------------------------------
	//
	// SCOPE VARIABLE MANAGEMENT
	//
	tScopeVar * CScope::registerAnonymouseFunction(PASTNode ast){ // register anonymous function in the top of scope ...

		tScopeVar irv;// = new tScopeVar;

		PASTNode args_node = AST_NODE(ast->children[0]);
		string var_name ="_afun"+CZetScriptUtils::intToString(n_anonymouse_func++);
		string symbol_ref = "@funp"+CZetScriptUtils::intToString(args_node!=NULL?args_node->children.size():0)+"_"+var_name;


		//int n_params=0;


		//n_params=args_node->children.size();
		//symbol_ref = "_p"+CZetScriptUtils::intToString(args_node->children.size())+"_s"+CZetScriptUtils::intToString(ast->idxScope);
		//symbol_ref=var_name args_node!=NULL?args_node->children.size():0;


		//}
		//else{
		//	symbol_ref=makeSymbolVarRef(var_name,ast->idxScope);
		//}
		//symbol_ref=symbol_ref+"_"+var_name;


		irv.symbol_ref = symbol_ref;
		irv.name=var_name;
		irv.idxAstNode=-1;
		if(ast != NULL){
			irv.idxAstNode=ast->idxAstNode;
		}

		CScope *base = SCOPE_NODE(idxBaseScope);

		base->m_registeredVariableFromBase.push_back(irv);

		return &base->m_registeredVariableFromBase[base->m_registeredVariableFromBase.size()-1];
	}

	tScopeVar * CScope::registerSymbol(const string & var_name, PASTNode ast, int n_params){
		tScopeVar *p_irv=NULL;//idxAstNode=-1;// * irv;


		if((p_irv = existRegisteredSymbol(var_name,n_params))==NULL){ // check whether is local var registered scope ...

			string symbol_ref = "";


			if(n_params>=0){
				//n_params=args_node->children.size();
				//symbol_ref = "_p"+CZetScriptUtils::intToString(args_node->children.size())+"_s"+CZetScriptUtils::intToString(ast->idxScope);
				symbol_ref="@funp"+CZetScriptUtils::intToString(n_params)+"_"+var_name;

			}
			else{
				symbol_ref="@var_"+var_name;
			}

			/*if(n_params<0){ // register var symbol
				symbol_ref = "_s"+CZetScriptUtils::intToString(ast->idxScope)+"_";
			}else{ // register function symbol
				symbol_ref = "_p"+CZetScriptUtils::intToString(n_params)+"_s"+CZetScriptUtils::intToString(ast->idxScope);
			}
			symbol_ref=symbol_ref+"_"+var_name;*/

			tScopeVar irv;// = new tScopeVar;
			//irv->m_obj=NULL;

			irv.symbol_ref = symbol_ref;
			irv.name=var_name;
			irv.idxAstNode=-1;
			if(ast != NULL){
				irv.idxAstNode=ast->idxAstNode;
			}
			m_registeredVariableFromBase.push_back(irv);
			return &m_registeredVariableFromBase[m_registeredVariableFromBase.size()-1];// irv->idxScopeVar;
		}else{

			if(p_irv != NULL) { // if not null is defined in script scope, else is C++ var
				writeErrorMsg(GET_AST_FILENAME_LINE(ast->idxAstNode)," error var \"%s\" already registered at line %i!", var_name.c_str(),AST_LINE(p_irv->idxAstNode));
			}else{
				writeErrorMsg(NULL,0," error var \"%s\" already registered as C++!", var_name.c_str());
			}
		}
		return NULL;//false;//-1;
	}

	tScopeVar * CScope::existRegisteredSymbolRecursiveDownScope(const string & symbol_ref, int n_params){


		for(unsigned i = 0; i < m_registeredVariableFromBase.size(); i++){
			string current_symbol_ref=m_registeredVariableFromBase[i].symbol_ref;
			if(n_params==NO_PARAMS_SYMBOL_ONLY){
				current_symbol_ref=CCompiler::getSymbolNameFromSymbolRef(current_symbol_ref);
			}

			if(current_symbol_ref==symbol_ref){
				return &m_registeredVariableFromBase[i];//.idxScopeVar; // ptr scope ?
			}
		}

		int parent =  getIdxParent();
		if(parent != ZS_UNDEFINED_IDX){
			return SCOPE_NODE(parent)->existRegisteredSymbolRecursiveDownScope(symbol_ref);
		}

		return NULL;//false;//-1;

	}

	tScopeVar * CScope::existRegisteredSymbolRecursiveUpScope(const string & symbol_ref, int n_params){
		tScopeVar *sv=NULL;
		// for each variable in current scope ...

		//if(this->idxBaseScope != ZS_UNDEFINED_IDX){ // don't test var on class cases...

		for(unsigned i = 0; i < m_registeredVariableFromBase.size(); i++){

			string current_symbol_ref=m_registeredVariableFromBase[i].symbol_ref;
			if(n_params==NO_PARAMS_SYMBOL_ONLY){
				current_symbol_ref=CCompiler::getSymbolNameFromSymbolRef(current_symbol_ref);
			}

			if(current_symbol_ref==symbol_ref){
				return &m_registeredVariableFromBase[i];//.idxScopeVar; // ptr scope ?
			}
		}
		//}

		// ok lets iterate through current scope list
		for(unsigned i = 0; i < m_localScopeList.size(); i++){
			CScope *s=SCOPE_NODE(m_localScopeList[i]);

			sv=s->existRegisteredSymbolRecursiveUpScope(symbol_ref);

			if(sv != NULL) return sv;
		}

		return NULL;//false;//-1;

	}

	tScopeVar * CScope::existRegisteredSymbolRecursive(const string & var_name, int n_params){

		string symbol_ref = "";
		tScopeVar *sv=NULL;

		if(n_params>=0){
			//n_params=args_node->children.size();
			//symbol_ref = "_p"+CZetScriptUtils::intToString(args_node->children.size())+"_s"+CZetScriptUtils::intToString(ast->idxScope);
			symbol_ref="@funp"+CZetScriptUtils::intToString(n_params)+"_"+var_name;

		}
		else{
			if(n_params==NO_PARAMS_IS_VARIABLE){
				symbol_ref="@var_"+var_name;
			}
			else{ // else only symbol...
				symbol_ref=var_name;
			}
		}

		sv=existRegisteredSymbolRecursiveDownScope(symbol_ref,n_params);
		if(sv!=NULL) return sv;

		return existRegisteredSymbolRecursiveUpScope(symbol_ref,n_params);

	}

	tScopeVar * CScope::existRegisteredSymbol(const string & var_name, int n_params){
		return existRegisteredSymbolRecursive(var_name, n_params);
	}


	tScopeVar * CScope::getInfoRegisteredSymbol(const string & v, int n_params, bool print_msg){
		tScopeVar *irv = existRegisteredSymbol(v,n_params);
		if(irv == NULL && print_msg){
			THROW_RUNTIME_ERROR("%s not exist",v.c_str());
		}

		return irv;
	}

	//-----------------------------------------------------------------------------------------------------------
	CScope::~CScope(){

		//deleteScope(this);
		/*for(unsigned i = 0; i < m_scopeList.size(); i++){
			delete m_scopeList[i];
		}*/

	}
}
