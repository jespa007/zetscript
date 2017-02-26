#include "zs_core.h"

vector<CState *> *  CState::vec_saved_state=NULL;
CState * CState::current_state=NULL;
//vector<CASTNode *> 							* current_vec_ast_node=NULL; // ast collection register...
//vector<tScopeVar *> 					* current_vec_info_scope_var_node=NULL;





string * CState::aux_str= NULL;//"1234_unknow_1234";

void CState::createSingletons(){
	if(vec_saved_state == NULL){
		CState *st;
		vec_saved_state = new vector<CState *> ();
		vec_saved_state->push_back(st=new CState()); // create first state 0.

		CASTNode::setVectorASTNode(st->getVecAstNode());
		CScope::set(st->getVecScopeNode());
		CScriptFunctionObject::set(st->getVecScriptFunctionObjectNode());
		CScriptClass::set(st->getScriptClassNode());
		CZetScript::setVectorParsedFiles(st->getVectorInfoParsedSourceNode());

		// create state by default ...
		//current_state = new CState();
		//current_vec_ast_node = current_state->getVecAstNode();
		//current_vec_scope_node = current_state->getVecScopeNode();
		//current_vec_info_scope_var_node = current_state->getVecScopeVarNode();
		//current_vec_registered_class_node= current_state->getScriptClassNode();
		//current_vec_info_registered_function_symbol_node = current_state->getVecInfoFunctionSymbolNode();
	}
}

bool  CState::setState(int idx){
	if(idx < 0 || (unsigned)idx >= vec_saved_state->size()){
		print_error_cr("index out of bounds");
		return false;
	}

	current_state = vec_saved_state->at(idx);

	/*current_vec_ast_node = current_state->getVecAstNode();
	current_vec_scope_node = current_state->getVecScopeNode();
	current_vec_info_scope_var_node = current_state->getVecScopeVarNode();
	current_vec_registered_class_node= current_state->getVecScriptClass();
	current_vec_info_registered_function_symbol_node = current_state->getVecInfoRegisteredFunctionFunctionNode();*/

	// TODO:maybe is better to re-run vm to optimize vec acceses

	return false;
}

int   CState::saveState(){

	CState * state = new CState();

	/*(*state->getVecAstNode()) 	= (*CASTNode::getVecAstNode());
	(*state->getVecScopeNode()) = (*CScope::getVecScopeNode());
	(*state->getScriptClassNode()) = (*CScriptClass::getVectorScriptClassList());
	(*state->getVecScriptFunctionObjectNode()) = (*CScriptFunctionObject::getVecScriptFunctionObjectNode());
*/
	// 1- Copy current state...
	// 1.1-Copy AST...
	/*for(unsigned int i = 0; i < current_vec_ast_node->size(); i++){

	}*/

	// 1.2-Copy Scope...
	/*for(unsigned int i = 0; i < current_vec_scope_node->size(); i++){

	}*/

	// 1.3-Copy InfoVar...
	/*for(unsigned int i = 0; i < current_vec_info_scope_var_node->size(); i++){

	}*/

	// 1.4-Copy Class...
	/*for(unsigned int i = 0; i < current_vec_registered_class_node->size(); i++){

	}*/

	// 1.5-Copy FunctionInfo...
	/*for(unsigned int i = 0; i < current_vec_info_registered_function_symbol_node->size(); i++){

	}*/


	vec_saved_state->push_back(state);

	return -1;
}

CState * CState::currentState(){
	return current_state;
}






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
/*
vector<tScopeVar *> 				*	CState::getCurrentVecScopeVarNode(){
	return current_vec_scope_var_node;
}

int 				  	CState::registerScopeVar(tScopeVar *info_scope_var){
	current_vec_info_scope_var_node->push_back(info_scope_var);
	info_scope_var->idxScopeVar = current_vec_info_scope_var_node->size()-1;
	return info_scope_var->idxScopeVar;
}

tScopeVar 	* CState::getScopeVarNodeByIdx(int idx){
	if(idx < 0 || (unsigned)idx >= current_vec_info_scope_var_node->size()){
		print_error_cr("tScopeVar node out of bound");
		return NULL;
	}

	return current_vec_info_scope_var_node->at(idx);
}
*/




//------------------------------------------------------------------------------------------------------

CState::CState(){

}


vector<CASTNode *> 		*		CState::getVecAstNode(){
	return &vec_ast_node;
}
vector<CScope *> 	*		CState::getVecScopeNode(){
	return &vec_scope_node;
}

/*
vector<tScopeVar *> 	*	CState::getVecScopeVarNode(){
	return &vec_scope_var_node;
}*/

vector<CScriptClass *> 	*CState::getScriptClassNode(){
	return &vec_script_class_node;
}

vector<CScriptFunctionObject *> 	*CState::getVecScriptFunctionObjectNode(){
	return &vec_scipt_function_object_node;
}

// TODO: This should be into script function object!!!!
void CState::unloadRecursiveFunctions(CScriptFunctionObject * info_function){
	// TODO: This should be into script function object!!!!
	// TODO: This should be into script function object!!!!
	// TODO: This should be into script function object!!!!
	// TODO: This should be into script function object!!!!
	// TODO: This should be into script function object!!!!
	// TODO: This should be into script function object!!!!
	// TODO: This should be into script function object!!!!
	// TODO: This should be into script function object!!!!
	// TODO: This should be into script function object!!!!s

	if((info_function->object_info.symbol_info.properties & PROPERTY_C_OBJECT_REF) == PROPERTY_C_OBJECT_REF){
		return;
	}

	print_info_cr("unloading local function %s...",info_function->object_info.symbol_info.symbol_name.c_str());
	for(unsigned k = 0; k < info_function->object_info.statment_op.size(); k++){

		for(unsigned a = 0; a  <info_function->object_info.statment_op[k].asm_op.size(); a++){

			delete info_function->object_info.statment_op[k].asm_op[a];
		}



	}

	for(unsigned f = 0; f < info_function->object_info.local_symbols.vec_idx_registeredFunction.size(); f++){
		unloadRecursiveFunctions(&info_function->object_info.local_symbols.vec_idx_registeredFunction[f]);
	}
}


void CState::destroyScriptClassNodes() {

	for(unsigned i = 0;i<vec_script_class_node.size();i++){

		CScriptClass *irv = vec_script_class_node[i];
		for(unsigned j = 0; j < irv->metadata_info.object_info.local_symbols.vec_idx_registeredFunction.size();j++){

			CScriptFunctionObject * info_function = GET_SCRIPT_FUNCTION_OBJECT(irv->metadata_info.object_info.local_symbols.vec_idx_registeredFunction[j]);

			print_info_cr("unloading %s::%s...",vec_script_class_node[i]->metadata_info.object_info.symbol_info.symbol_name.c_str(), info_function->object_info.symbol_info.symbol_name.c_str());

			// C related functions ...
			if(i==0 && j==0){ // MAIN FUNCTION (C functions)

				// check external C functions defined inside main function ...
				for(unsigned f=0; f < info_function->object_info.local_symbols.vec_idx_registeredFunction.size();f++){

					CScriptFunctionObject * irv_main_function =  GET_SCRIPT_FUNCTION_OBJECT(info_function->object_info.local_symbols.vec_idx_registeredFunction[f]);

					if((irv_main_function->object_info.symbol_info.properties & PROPERTY_C_OBJECT_REF) == PROPERTY_C_OBJECT_REF){

						/*if(irv_main_function->idx_return_type == getIdxClassVoid()){
							delete_proxy_function<void>(irv_main_function->m_arg.size(),(void *)irv_main_function->object_info.symbol_info.ref_ptr);//=(int)new_proxy_function<void>(irs.m_arg.size(),function_ptr))==0){//(int)function_ptr;

						}
						else{
							delete_proxy_function<int>(irv_main_function->m_arg.size(),(void *)irv_main_function->object_info.symbol_info.ref_ptr);
						}*/
					}
				}
			}
			else{ // normal class function member ...
				/*if(((info_function->object_info.symbol_info.properties & PROPERTY_C_OBJECT_REF) == PROPERTY_C_OBJECT_REF)
				 &&((info_function->object_info.symbol_info.properties & PROPERTY_IS_DERIVATED) != PROPERTY_IS_DERIVATED)
				 ){

					delete ((std::function<void *(void *,PROXY_CREATOR)> *)	info_function->object_info.symbol_info.ref_ptr);
				}*/
			}

			info_function->object_info.symbol_info.ref_ptr = 0;


			// this is not valid!!! -> must be in script function object.
			// this is not valid!!! -> must be in script function object.
			// this is not valid!!! -> must be in script function object.
			// this is not valid!!! -> must be in script function object.
			unloadRecursiveFunctions(info_function);//->object_info.local_symbols.vec_idx_registeredFunction[f]);


			//bool is_main_function = i==0&&j==0;

			//if(!is_main_function){

			//}

		}



		if(irv->c_constructor != NULL){
			delete irv->c_constructor;
		}

		if(irv->c_destructor != NULL){
			delete irv->c_destructor;
		}

		if((irv->metadata_info.object_info.symbol_info.properties & PROPERTY_C_OBJECT_REF) == PROPERTY_C_OBJECT_REF){ //ast created for compatibility reasons, we need to deallocate

			//delete irv->metadata_info.object_info.symbol_info.ast;
		}
		else{

			if(i==0){ // MAIN CLASS...


				//delete_proxy_function


			}else{ // generic class ...

				/*CScope *scope_info_ptr = AST_SCOPE_INFO(irv->metadata_info.object_info.symbol_info.idxAstNode);

				if(scope_info_ptr != NULL){

					scope_info_ptr->destroyScopes();
					delete scope_info_ptr;
				}*/
				//delete irv->metadata_info.object_info.symbol_info.ast;
			}
		}


		// delete CScriptClass
		delete irv;
	}
}

CState::~CState(){

	destroyScriptClassNodes();

	for(unsigned i =0; i < vec_info_parsed_source_node.size(); i++){
		delete vec_info_parsed_source_node[i].data;
	}

	// Destroy scope ...
	//m_rootAstNode->scope_info_ptr->destroyScopes();
	//delete m_rootAstNode->scope_info_ptr;//m_rootScope;
	// End destroy scope ...

	delete vec_ast_node[0];

	//m_rootScope=NULL;
}

