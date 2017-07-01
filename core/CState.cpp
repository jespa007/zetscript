#include "zs_core.h"

vector<CState *> *  CState::vec_saved_state=NULL;
CState * CState::current_state=NULL;
//vector<CASTNode *> 							* current_vec_ast_node=NULL; // ast collection register...
//vector<tScopeVar *> 					* current_vec_info_scope_var_node=NULL;





string * CState::aux_str= NULL;//"1234_unknow_1234";

void CState::init(){
	if(vec_saved_state == NULL){
		CState *st;
		vec_saved_state = new vector<CState *> ();
		vec_saved_state->push_back(st=new CState()); // create first state 0.

		CASTNode::setVectorASTNode(st->getVectorASTNodeNode());
		CScope::setVectorScopeNode(st->getVectorScopeNode());
		CScriptFunctionObject::setVectorScriptFunctionObjectNode(st->getVectorScriptFunctionObjectNode());
		CScriptClass::setVectorScriptClassNode(st->getVectorScriptClassNode());
		CZetScript::setVectorInfoParsedFiles(st->getVectorInfoParsedSourceNode());

		// create state by default ...
		//current_state = new CState();
		//current_vec_ast_node = current_state->getVectorASTNodeNode();
		//current_vec_scope_node = current_state->getVectorScopeNode();
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

	CState * state_to_restore = vec_saved_state->at(idx);

	/*current_vec_ast_node = current_state->getVectorASTNodeNode();
	current_vec_scope_node = current_state->getVectorScopeNode();
	current_vec_info_scope_var_node = current_state->getVecScopeVarNode();
	current_vec_registered_class_node= current_state->getVecScriptClass();
	current_vec_info_registered_function_symbol_node = current_state->getVecInfoRegisteredFunctionFunctionNode();*/

	// TODO:maybe is better to re-run vm to optimize vec acceses

	// 1. destroy current state (0)
	current_state->destroy();

	// 2. Copy state 1
	vector<CASTNode *> 				* vec_ast_node_src					= state_to_restore->getVectorASTNodeNode();
	vector<CScope *> 				* vec_scope_src						= state_to_restore->getVectorScopeNode();
	vector<CScriptClass *> 			* vec_script_class_src				= state_to_restore->getVectorScriptClassNode();
	vector<CScriptFunctionObject *>	* vec_script_function_object_src	= state_to_restore->getVectorScriptFunctionObjectNode();
	vector<tInfoParsedSource>		* vec_info_parsed_source_src		= state_to_restore->getVectorInfoParsedSourceNode();

	vector<CASTNode *> 				* vec_ast_node_dst					= state_to_restore->getVectorASTNodeNode();
	vector<CScope *> 				* vec_scope_dst						= state_to_restore->getVectorScopeNode();
	vector<CScriptClass *> 			* vec_script_class_dst				= state_to_restore->getVectorScriptClassNode();
	vector<CScriptFunctionObject *>	* vec_script_function_object_dst	= state_to_restore->getVectorScriptFunctionObjectNode();
	vector<tInfoParsedSource>		* vec_info_parsed_source_dst		= state_to_restore->getVectorInfoParsedSourceNode();

	// 1- Copy current state...
	// 1.1-Copy AST...
	for(unsigned int i = 0; i < vec_ast_node_src->size(); i++){
		CASTNode *ast = new CASTNode();
		*ast = *vec_ast_node_src->at(i);
		vec_ast_node_dst->push_back(ast);
	}

	// 1.2-Copy Scope...
	for(unsigned int i = 0; i < vec_scope_src->size(); i++){
		CScope *scope = new CScope();
		*scope = *vec_scope_src->at(i);
		vec_scope_dst->push_back(scope);

	}

	// 1.3-Copy CScriptClass...
	for(unsigned int i = 0; i < vec_script_class_src->size(); i++){
		CScriptClass *sclass = new CScriptClass();
		*sclass = *vec_script_class_src->at(i);
		vec_script_class_dst->push_back(sclass);
	}

	// 1.4-Copy CScriptFunctionObject...
	for(unsigned int i = 0; i < vec_script_function_object_src->size(); i++){
		CScriptFunctionObject *sfo = new CScriptFunctionObject();
		*sfo = *vec_script_function_object_src->at(i);
		vec_script_function_object_dst->push_back(sfo);
	}

	// 1.5-Copy tInfoParsedSource...
	for(unsigned int i = 0; i < vec_info_parsed_source_src->size(); i++){
		tInfoParsedSource ips;
		ips = vec_info_parsed_source_src->at(i);
		vec_info_parsed_source_dst->push_back(ips);
	}

	return false;
}

int   CState::saveState(){



	CState * save_st = new CState();
	vector<CASTNode *> 				* vec_ast_node_dst					= save_st->getVectorASTNodeNode();
	vector<CScope *> 				* vec_scope_dst						= save_st->getVectorScopeNode();
	vector<CScriptClass *> 			* vec_script_class_dst				= save_st->getVectorScriptClassNode();
	vector<CScriptFunctionObject *>	* vec_script_function_object_dst	= save_st->getVectorScriptFunctionObjectNode();
	vector<tInfoParsedSource>		* vec_info_parsed_source_dst		= save_st->getVectorInfoParsedSourceNode();

	vector<CASTNode *> 				* vec_ast_node_src					= current_state->getVectorASTNodeNode();
	vector<CScope *> 				* vec_scope_src						= current_state->getVectorScopeNode();
	vector<CScriptClass *> 			* vec_script_class_src				= current_state->getVectorScriptClassNode();
	vector<CScriptFunctionObject *>	* vec_script_function_object_src	= current_state->getVectorScriptFunctionObjectNode();
	vector<tInfoParsedSource>		* vec_info_parsed_source_src		= current_state->getVectorInfoParsedSourceNode();

	// 1- Copy current state...
	// 1.1-Copy AST...
	for(unsigned int i = 0; i < vec_ast_node_src->size(); i++){
		CASTNode *ast = new CASTNode();
		*ast = *vec_ast_node_src->at(i);
		vec_ast_node_dst->push_back(ast);
	}

	// 1.2-Copy Scope...
	for(unsigned int i = 0; i < vec_scope_src->size(); i++){
		CScope *scope = new CScope();
		*scope = *vec_scope_src->at(i);
		vec_scope_dst->push_back(scope);

	}

	// 1.3-Copy CScriptClass...
	for(unsigned int i = 0; i < vec_script_class_src->size(); i++){
		CScriptClass *sclass = new CScriptClass();
		*sclass = *vec_script_class_src->at(i);
		vec_script_class_dst->push_back(sclass);
	}

	// 1.4-Copy CScriptFunctionObject...
	for(unsigned int i = 0; i < vec_script_function_object_src->size(); i++){
		CScriptFunctionObject *sfo = new CScriptFunctionObject();
		*sfo = *vec_script_function_object_src->at(i);
		vec_script_function_object_dst->push_back(sfo);
	}

	// 1.5-Copy tInfoParsedSource...
	for(unsigned int i = 0; i < vec_info_parsed_source_src->size(); i++){
		tInfoParsedSource ips;
		ips = vec_info_parsed_source_src->at(i);
		vec_info_parsed_source_dst->push_back(ips);
	}


	vec_saved_state->push_back(save_st);

	return (vec_saved_state->size()-1);
}

CState * CState::currentState(){
	return current_state;
}

void CState::destroySingletons(){
	for(unsigned i = 0; i < vec_saved_state->size(); i++){
		delete vec_saved_state->at(i);
	}

	delete vec_saved_state;
}






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

	vec_ast_node = new vector<CASTNode *>; // ast collection register...
	vec_scope_node = new vector<CScope *>;
	//vector<tScopeVar *> 						vec_scope_var_node;
	vec_script_class_node = new vector<CScriptClass *> ;
	vec_script_function_object_node = new vector<CScriptFunctionObject *>;
	vec_info_parsed_source_node = new vector<tInfoParsedSource> ;

}


vector<CASTNode *> 		*		CState::getVectorASTNodeNode(){
	return vec_ast_node;
}
vector<CScope *> 	*		CState::getVectorScopeNode(){
	return vec_scope_node;
}

/*
vector<tScopeVar *> 	*	CState::getVecScopeVarNode(){
	return &vec_scope_var_node;
}*/

vector<CScriptClass *> 	*CState::getVectorScriptClassNode(){
	return vec_script_class_node;
}

vector<tInfoParsedSource> 	*CState::getVectorInfoParsedSourceNode(){
	return vec_info_parsed_source_node;
}

vector<CScriptFunctionObject *> 	*CState::getVectorScriptFunctionObjectNode(){
	return vec_script_function_object_node;
}


void CState::destroyASTNodes(){
	for(unsigned i = 0; i < vec_ast_node->size(); i++){
		delete vec_ast_node->at(i);
	}

	delete vec_ast_node;
	vec_ast_node = NULL;

}
void CState::destroyScopeNodes(){
	for(unsigned i = 0; i < vec_scope_node->size(); i++){
		delete vec_scope_node->at(i);
	}

	delete vec_scope_node;
	vec_scope_node=NULL;
}



void CState::destroyScriptFunctionObjectNodes(){

	for(unsigned i = 0; i < vec_script_function_object_node->size(); i++){

		CScriptFunctionObject * info_function = vec_script_function_object_node->at(i);

		if((info_function->object_info.symbol_info.properties & PROPERTY_C_OBJECT_REF) != PROPERTY_C_OBJECT_REF){
			//return;
		}

		print_debug_cr("unloading local function %s...",info_function->object_info.symbol_info.symbol_name.c_str());
		if(info_function->object_info.statment_op!=NULL){
			for(PtrStatment stat =info_function->object_info.statment_op; *stat != NULL; stat++){

				free(*stat);
			}

			free(info_function->object_info.statment_op);
		}

		// unloading scope ...



		 for(unsigned j = 0; j < info_function->object_info.n_info_var_scope; j++){
			 free(info_function->object_info.info_var_scope[j].var_index);
		 }

		 free(info_function->object_info.info_var_scope);


		delete info_function;

	}

	delete vec_script_function_object_node;
	vec_script_function_object_node=NULL;
}


void CState::destroyScriptClassNodes() {

	for(unsigned i = 0;i<vec_script_class_node->size();i++){

		CScriptClass *irv = vec_script_class_node->at(i);
		for(unsigned j = 0; j < irv->metadata_info.object_info.local_symbols.vec_idx_registeredFunction.size();j++){

			CScriptFunctionObject * info_function = GET_SCRIPT_FUNCTION_OBJECT(irv->metadata_info.object_info.local_symbols.vec_idx_registeredFunction[j]);

			print_debug_cr("unloading %s::%s...",vec_script_class_node->at(i)->metadata_info.object_info.symbol_info.symbol_name.c_str(), info_function->object_info.symbol_info.symbol_name.c_str());

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

	delete vec_script_class_node;
	vec_script_class_node =NULL;
}

void CState::destroyInfoParsedSourceNode(){
	for(unsigned i =0; i < vec_info_parsed_source_node->size(); i++){
		delete vec_info_parsed_source_node->at(i).data;
	}

	delete vec_info_parsed_source_node;
	vec_info_parsed_source_node=NULL;
}

void CState::destroy(){
	destroyScriptClassNodes();
	destroyScriptFunctionObjectNodes();

	destroyScopeNodes();
	destroyASTNodes();
	destroyInfoParsedSourceNode();




	// Destroy scope ...
	//m_rootAstNode->scope_info_ptr->destroyScopes();
	//delete m_rootAstNode->scope_info_ptr;//m_rootScope;
	// End destroy scope ...

	//delete vec_ast_node->at(0);
}

CState::~CState(){

	destroy();

	//m_rootScope=NULL;
}

