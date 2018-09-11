/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
#include "CZetScript.h"


namespace zetscript{

	vector<CState *> *  CState::vec_saved_state=NULL;
	CState * CState::current_state=NULL;
	string * CState::aux_str= NULL;

	void CState::init(){
		if(vec_saved_state == NULL){
			current_state=new CState();
			vec_saved_state = new vector<CState *> ();


			CASTNode::setVectorASTNode(current_state->getVectorASTNodeNode());
			CScope::setVectorScopeNode(current_state->getVectorScopeNode());
			CScriptFunctionObject::setVectorScriptFunctionObjectNode(current_state->getVectorScriptFunctionObjectNode());
			CScriptClass::setVectorScriptClassNode(current_state->getVectorScriptClassNode());
			CZetScript::setVectorInfoParsedFiles(current_state->getVectorInfoParsedSourceNode());


			// add empty ast_node...

			CASTNode *ast =CASTNode::newASTNode();
			CScope *sc=CScope::newScope(ast);

			ast->node_type = NODE_TYPE::BODY_BLOCK_NODE;

			

			CASTNode::init();


			if (!CScriptClass::init()) {
				THROW_RUNTIME_ERROR("CScriptClass not initialized");
				return;
			}

			// init custom registers...


			// push undefined file by default ...
			tInfoParsedSource ps;
			ps.filename = "undefined";
			current_state->getVectorInfoParsedSourceNode()->push_back(ps);


			// after init, let's save/restore state...
			CState::saveState();
			CState::setState(0);

			// save first state ...
			//vec_saved_state->push_back(st);
			//current_state = st;
			
		}
	}

	bool  CState::setState(int idx){
		if(idx < 0 || (unsigned)idx >= vec_saved_state->size()){
			THROW_RUNTIME_ERROR("index out of bounds");
			return false;
		}

		// clear globals ...
		CURRENT_VM->clearGlobals();

		// clean before save ...
		current_state->clearCompileInformation();

		CState * state_to_restore = vec_saved_state->at(idx);

		// TODO:maybe is better to re-run vm to optimize vec acceses

		// 1. destroy current state (0)


		// 2. Copy state 1
		vector<CASTNode *> 				* vec_ast_node_src					= state_to_restore->getVectorASTNodeNode();
		vector<CScope *> 				* vec_scope_src						= state_to_restore->getVectorScopeNode();
		vector<CScriptClass *> 			* vec_script_class_src				= state_to_restore->getVectorScriptClassNode();
		vector<CScriptFunctionObject *>	* vec_script_function_object_src	= state_to_restore->getVectorScriptFunctionObjectNode();
		vector<tInfoParsedSource>		* vec_info_parsed_source_src		= state_to_restore->getVectorInfoParsedSourceNode();



		CState * current_state_new=new CState(); // create first state 0.

		vector<CASTNode *> 				* vec_ast_node_dst					= current_state_new->getVectorASTNodeNode();
		vector<CScope *> 				* vec_scope_dst						= current_state_new->getVectorScopeNode();
		vector<CScriptClass *> 			* vec_script_class_dst				= current_state_new->getVectorScriptClassNode();
		vector<CScriptFunctionObject *>	* vec_script_function_object_dst	= current_state_new->getVectorScriptFunctionObjectNode();
		vector<tInfoParsedSource>		* vec_info_parsed_source_dst		= current_state_new->getVectorInfoParsedSourceNode();





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
			sfo->object_info.asm_op=NULL;
			sfo->object_info.info_var_scope=NULL;
			vec_script_function_object_dst->push_back(sfo);
		}

		// 1.5-Copy tInfoParsedSource...
		for(unsigned int i = 0; i < vec_info_parsed_source_src->size(); i++){
			tInfoParsedSource ips;
			ips = vec_info_parsed_source_src->at(i);
			vec_info_parsed_source_dst->push_back(ips);
		}

		if (current_state != NULL) {
			delete current_state;//->destroy();
			current_state = NULL;
		}


		// set main function pointer ...


		current_state = current_state_new;

		// finally set vectors....
		CASTNode::setVectorASTNode(vec_ast_node_dst);
		CScope::setVectorScopeNode(vec_scope_dst);
		CScriptFunctionObject::setVectorScriptFunctionObjectNode(vec_script_function_object_dst);
		CScriptClass::setVectorScriptClassNode(vec_script_class_dst);
		CZetScript::setVectorInfoParsedFiles(vec_info_parsed_source_dst);

		CZetScript::getInstance()->destroyMainFunction();

		return true;
	}

	int   CState::saveState(){

		if(current_state == NULL){
			THROW_RUNTIME_ERROR ("ZetScript was not inicialized");
			return -1;

		}

		// before save ...

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
			sfo->object_info.asm_op=NULL;
			sfo->object_info.info_var_scope=NULL;
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

		if (current_state != NULL) {

			vector<CScriptClass *> *vec = current_state->getVectorScriptClassNode();

			// we have to destroy all allocated constructor/destructor ...
			for (unsigned i = 0; i < vec->size(); i++) {

				CScriptClass *irv = vec->at(i);

				if ((irv->metadata_info.object_info.symbol_info.properties & PROPERTY_C_OBJECT_REF) == PROPERTY_C_OBJECT_REF) {

					//zs_print_debug_cr("* Erasing c destructor/contructor %s...", irv->classPtrType.c_str());

					if (irv->c_constructor) {
						delete irv->c_constructor;
					}

					if (irv->c_destructor) {
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
		}


		// erase saved states ...
		for(unsigned i = 0; i < vec_saved_state->size(); i++){
			delete vec_saved_state->at(i);
		}

		// delete vector states...
		delete vec_saved_state;

		// erase current state...
		if (current_state != NULL) {
			delete current_state;
		}
	}

	//------------------------------------------------------------------------------------------------------

	CState::CState(){

		vec_ast_node = new vector<CASTNode *>; // ast collection register...
		vec_scope_node = new vector<CScope *>;

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

	void  CState::clearCompileInformation() {

		CScriptFunctionObject * main_function = vec_script_function_object_node->at(0);

		// clean main functions ... remove script functions and leave c functions...
		for (unsigned f = 0;
			f < main_function->object_info.local_symbols.vec_idx_registeredFunction.size()
			;) {
			// get function info
			CScriptFunctionObject * local_function = GET_SCRIPT_FUNCTION_OBJECT(main_function->object_info.local_symbols.vec_idx_registeredFunction[f]);

			if ((local_function->object_info.symbol_info.properties & PROPERTY_C_OBJECT_REF) != PROPERTY_C_OBJECT_REF) {
				main_function->object_info.local_symbols.vec_idx_registeredFunction.erase(main_function->object_info.local_symbols.vec_idx_registeredFunction.begin() + f);
			}
			else {
				f++;
			}

		}

		// remove c variables ...
		for (unsigned v = 0;
			v < main_function->object_info.local_symbols.m_registeredVariable.size(); ) {

			if ((main_function->object_info.local_symbols.m_registeredVariable[v].properties & PROPERTY_C_OBJECT_REF) != PROPERTY_C_OBJECT_REF) {

				main_function->object_info.local_symbols.m_registeredVariable.erase(main_function->object_info.local_symbols.m_registeredVariable.begin() + v);

			}
			else {
				v++;
			}
		}

		//int i = vec_script_function_object_node->size()-1;
		bool end=false;
		do{
			CScriptFunctionObject * info_function = vec_script_function_object_node->at(vec_script_function_object_node->size()-1);
			end=(info_function->object_info.symbol_info.properties & PROPERTY_C_OBJECT_REF) == PROPERTY_C_OBJECT_REF;

			if(!end){

				if (info_function->object_info.asm_op != NULL) {
					//for (PtrAsmOp stat = info_function->object_info.asm_op; *stat != NULL; stat++) {

						//free(*stat);
					//}

					free(info_function->object_info.asm_op);
					info_function->object_info.asm_op=NULL;
				}

				// unloading scope ...
				if (info_function->object_info.info_var_scope != NULL) {
					for (unsigned j = 0; j < info_function->object_info.n_info_var_scope; j++) {
						free(info_function->object_info.info_var_scope[j].var_index);
					}

					free(info_function->object_info.info_var_scope);
					info_function->object_info.info_var_scope=NULL;
				}

				vec_script_function_object_node->pop_back();
				delete info_function;

			}

		}while(!end);


		// clean script classes ...

		end=false;
		do{
			CScriptClass * sc = vec_script_class_node->at(vec_script_class_node->size()-1);
			end=(sc->metadata_info.object_info.symbol_info.properties & PROPERTY_C_OBJECT_REF) == PROPERTY_C_OBJECT_REF;

			if(!end){

				delete sc;
				vec_script_class_node->pop_back();

			}

		}while(!end);

	}

	void CState::destroyASTNodes(){
		for(unsigned i = 0; i < vec_ast_node->size(); i++){
			delete vec_ast_node->at(i);
		}
		vec_ast_node->clear();
		delete vec_ast_node;
		vec_ast_node=NULL;
	}

	void CState::destroyScopeNodes(){
		for(unsigned i = 0; i < vec_scope_node->size(); i++){
			delete vec_scope_node->at(i);
		}
		vec_scope_node->clear();
		delete vec_scope_node;
		vec_scope_node=NULL;
	}

	void CState::destroyScriptClassNodes() {

		for(unsigned i = 0;i < vec_script_class_node->size();i++){
			//zs_print_debug_cr("* Erasing %s...", vec_script_class_node->at(i)->classPtrType.c_str());
			delete vec_script_class_node->at(i);

		}

		vec_script_class_node->clear();
		delete vec_script_class_node;
		vec_script_class_node=NULL;
	}	
	
	
	void CState::destroyScriptFunctionObjectNodes() {

		for(unsigned i = 0;i < vec_script_function_object_node->size();i++){
			//zs_print_debug_cr("* Erasing function %s...", vec_script_function_object_node->at(i)->object_info.symbol_info.symbol_ref.c_str());
			CScriptFunctionObject * info_function = vec_script_function_object_node->at(i);

			if (info_function->object_info.asm_op != NULL) {

				free(info_function->object_info.asm_op);
				info_function->object_info.asm_op=NULL;
			}

			// unloading scope ...
			if (info_function->object_info.info_var_scope != NULL) {
				for (unsigned j = 0; j < info_function->object_info.n_info_var_scope; j++) {
					free(info_function->object_info.info_var_scope[j].var_index);
				}

				free(info_function->object_info.info_var_scope);
				info_function->object_info.info_var_scope=NULL;
			}

			delete info_function;
		}

		vec_script_function_object_node->clear();
		delete vec_script_function_object_node;
		vec_script_function_object_node=NULL;
	}

	void CState::destroyInfoParsedSourceNode(){
		delete vec_info_parsed_source_node;
		vec_info_parsed_source_node=NULL;
	}

	void CState::destroy(){
		clearCompileInformation();
		destroyScriptClassNodes();
		destroyScriptFunctionObjectNodes();
		destroyScopeNodes();
		destroyASTNodes();
		destroyInfoParsedSourceNode();
	}

	CState::~CState(){
		destroy();
	}
}

