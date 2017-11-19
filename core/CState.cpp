/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
#include "zs_core.h"


namespace zetscript{

	vector<CState *> *  CState::vec_saved_state=NULL;
	CState * CState::current_state=NULL;
	string * CState::aux_str= NULL;//"1234_unknow_1234";

	void CState::init(){
		if(vec_saved_state == NULL){
			CState *st=new CState();
			vec_saved_state = new vector<CState *> ();


			CASTNode::setVectorASTNode(st->getVectorASTNodeNode());
			CScope::setVectorScopeNode(st->getVectorScopeNode());
			CScriptFunctionObject::setVectorScriptFunctionObjectNode(st->getVectorScriptFunctionObjectNode());
			CScriptClass::setVectorScriptClassNode(st->getVectorScriptClassNode());
			CZetScript::setVectorInfoParsedFiles(st->getVectorInfoParsedSourceNode());

			// add empty ast_node...

			CASTNode *ast =CASTNode::newASTNode();
			CScope *sc=CScope::newScope(ZS_UNDEFINED_IDX);
			ast->idxScope = sc->idxScope;
			ast->node_type = NODE_TYPE::BODY_NODE;

			current_state=st;

		}
	}

	bool  CState::setState(int idx){
		if(idx < 0 || (unsigned)idx >= vec_saved_state->size()){
			zs_print_error_cr("index out of bounds");
			return false;
		}

		// clear globals ...
		CURRENT_VM->clearGlobals();

		CState * state_to_restore = vec_saved_state->at(idx);

		// TODO:maybe is better to re-run vm to optimize vec acceses

		// 1. destroy current state (0)


		// 2. Copy state 1
		vector<CASTNode *> 				* vec_ast_node_src					= state_to_restore->getVectorASTNodeNode();
		vector<CScope *> 				* vec_scope_src						= state_to_restore->getVectorScopeNode();
		vector<CScriptClass *> 			* vec_script_class_src				= state_to_restore->getVectorScriptClassNode();
		vector<CScriptFunctionObject *>	* vec_script_function_object_src	= state_to_restore->getVectorScriptFunctionObjectNode();
		vector<tInfoParsedSource>		* vec_info_parsed_source_src		= state_to_restore->getVectorInfoParsedSourceNode();




		delete current_state;//->destroy();

		current_state=new CState(); // create first state 0.

		vector<CASTNode *> 				* vec_ast_node_dst					= current_state->getVectorASTNodeNode();
		vector<CScope *> 				* vec_scope_dst						= current_state->getVectorScopeNode();
		vector<CScriptClass *> 			* vec_script_class_dst				= current_state->getVectorScriptClassNode();
		vector<CScriptFunctionObject *>	* vec_script_function_object_dst	= current_state->getVectorScriptFunctionObjectNode();
		vector<tInfoParsedSource>		* vec_info_parsed_source_dst		= current_state->getVectorInfoParsedSourceNode();


		// finally set vectors....
		CASTNode::setVectorASTNode(vec_ast_node_dst);
		CScope::setVectorScopeNode(vec_scope_dst);
		CScriptFunctionObject::setVectorScriptFunctionObjectNode(vec_script_function_object_dst);
		CScriptClass::setVectorScriptClassNode(vec_script_class_dst);
		CZetScript::setVectorInfoParsedFiles(vec_info_parsed_source_dst);


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

		if(current_state == NULL){
			zs_print_warning_cr("ZetScript was not inicialized");
			return -1;
		}

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

	bool   CState::restoreLastState(){
		return setState(vec_saved_state->size()-1);
	}

	CState * CState::currentState(){
		return current_state;
	}

	void CState::clearCurrentCompileInformation(){
		current_state->clearCompileInformation();
	}

	void CState::destroySingletons(){

		vector<CScriptClass *> *vec = current_state->getVectorScriptClassNode();

		// we have to destroy all allocated constructor/destructor ...
		for(vector<CScriptClass *>::iterator i = vec->begin()+MAX_BASIC_CLASS_TYPES;i!=vec->end();i++){

			CScriptClass *irv = *i;



			if((irv->metadata_info.object_info.symbol_info.properties & PROPERTY_C_OBJECT_REF) == PROPERTY_C_OBJECT_REF){

				zs_print_debug_cr("* Erasing c destructor/contructor %s...",irv->classPtrType.c_str());
				if(irv->c_constructor){
					delete irv->c_constructor;
				}

				if(irv->c_destructor){
					delete irv->c_destructor;
				}

				// delete CScriptClass
				//delete irv;

				//vec_script_class_node->erase(i);
			}
			//else{
			//	i++;
			//}
		}


		// erase saved states ...
		for(unsigned i = 0; i < vec_saved_state->size(); i++){
			delete vec_saved_state->at(i);
		}

		// delete vector states...
		delete vec_saved_state;

		// erase current state...
		delete current_state;
	}

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
	}

	void CState::destroyScopeNodes(){
		for(unsigned i = 0; i < vec_scope_node->size(); i++){
			delete vec_scope_node->at(i);
		}
	}

	void CState::destroyScriptClassNodes() {

		for(vector<CScriptClass *>::iterator i = vec_script_class_node->begin()+MAX_BASIC_CLASS_TYPES;i!=vec_script_class_node->end();){

			CScriptClass *irv = *i;

			zs_print_debug_cr("* Erasing %s...",irv->classPtrType.c_str());

			//if((irv->metadata_info.object_info.symbol_info.properties & PROPERTY_C_OBJECT_REF) != PROPERTY_C_OBJECT_REF){


				// delete CScriptClass
				delete irv;

				vec_script_class_node->erase(i);
			//}
			//else{
			//	i++;
			//}
		}
	}

	void CState::destroyInfoParsedSourceNode(){
		delete vec_info_parsed_source_node;
		vec_info_parsed_source_node=NULL;
	}

	void  CState::clearCompileInformation(){

		for(unsigned i = 0; i < vec_script_function_object_node->size(); i++){

			CScriptFunctionObject * info_function = vec_script_function_object_node->at(i);

			if((info_function->object_info.symbol_info.properties & PROPERTY_C_OBJECT_REF) != PROPERTY_C_OBJECT_REF){ // is a compiled script ...
#ifdef __DEBUG__
				CScriptClass *sc = CScriptClass::getScriptClassByIdx(info_function->object_info.symbol_info.idxScriptClass);
				if(sc){
				zs_print_debug_cr("Clear function %s::%s...",sc->metadata_info.object_info.symbol_info.symbol_name.c_str(),info_function->object_info.symbol_info.symbol_name.c_str());
				}
#endif

				//zs_print_debug_cr("unloading local function %s...",info_function->object_info.symbol_info.symbol_name.c_str());
				if(info_function->object_info.statment_op!=NULL){
					for(PtrStatment stat =info_function->object_info.statment_op; *stat != NULL; stat++){

						free(*stat);
					}

					free(info_function->object_info.statment_op);
				}

				// unloading scope ...
				if(info_function->object_info.info_var_scope != NULL){
					 for(unsigned j = 0; j < info_function->object_info.n_info_var_scope; j++){
						 free(info_function->object_info.info_var_scope[j].var_index);
					 }

					 free(info_function->object_info.info_var_scope);
				}

				// unregister global variables & functions ...
				 if(i == 0){ // is the main function so don't touch (i.e preserve) C functions/vars ...

					 for(vector<int>::iterator f =info_function->object_info.local_symbols.vec_idx_registeredFunction.begin(); f != info_function->object_info.local_symbols.vec_idx_registeredFunction.end();){
						 // get function info
						 CScriptFunctionObject * local_function = GET_SCRIPT_FUNCTION_OBJECT(*f);

						 if((local_function->object_info.symbol_info.properties & PROPERTY_C_OBJECT_REF) != PROPERTY_C_OBJECT_REF){
							 info_function->object_info.local_symbols.vec_idx_registeredFunction.erase(f);
						 }else{
							 f++;
						 }

					 }

					 for(vector<tInfoVariableSymbol>::iterator v = info_function->object_info.local_symbols.m_registeredVariable.begin();
					     v != info_function->object_info.local_symbols.m_registeredVariable.end(); ){

						 if((v->properties & PROPERTY_C_OBJECT_REF) != PROPERTY_C_OBJECT_REF){

							 info_function->object_info.local_symbols.m_registeredVariable.erase(v);

						 }else{
							 v++;
						 }
					 }
				 }
				 else{ // is not the main function so let's remove it and their local symbols ...
					 // not main function it's safe to unregister all symbols ...
					 info_function->object_info.local_symbols.m_registeredVariable.clear();
					 info_function->object_info.local_symbols.vec_idx_registeredFunction.clear();

					 delete info_function;
				 }
			}
		}
	}

	void CState::destroy(){
		clearCompileInformation();
		destroyScriptClassNodes();
		//destroyScriptFunctionObjectNodes();
		destroyScopeNodes();
		destroyASTNodes();
		destroyInfoParsedSourceNode();
	}

	CState::~CState(){
		destroy();
	}
}

