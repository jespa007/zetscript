/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
#include "CZetScript.h"

namespace zetscript{

	



	void  writeErrorMsg(const char *filename, int line, const  char  *string_text, ...);

/*	vector<CScriptClass *> 				* CScriptClass::vec_script_class_node=NULL;


	string  * CScriptClass::VOID_TYPE_STR=NULL;// 		typeid(void).name()
	string  * CScriptClass::INT_PTR_TYPE_STR=NULL;//	typeid(int *).name()
	string  * CScriptClass::FLOAT_PTR_TYPE_STR=NULL;//	typeid(float *).name()
	string  * CScriptClass::STRING_PTR_TYPE_STR=NULL;//	typeid(string *).name()
	string  * CScriptClass::CONST_CHAR_PTR_TYPE_STR=NULL;//	typeid(string *).name()
	string  * CScriptClass::BOOL_PTR_TYPE_STR=NULL;//	typeid(bool *).name()

	string  * CScriptClass::INT_TYPE_STR=NULL;//	typeid(int *).name()
	string  * CScriptClass::UNSIGNED_INT_TYPE_STR=NULL;//	typeid(int *).name()
	string  * CScriptClass::INTPTR_T_TYPE_STR=NULL;//	typeid(int *).name()
	string  * CScriptClass::FLOAT_TYPE_STR=NULL;//	typeid(int *).name()
	string  * CScriptClass::BOOL_TYPE_STR=NULL;//	typeid(bool *).name()
	string  * CScriptClass::STACK_ELEMENT_PTR=NULL;//	typeid(bool *).name()


	// register metamethods str ...
	 //--obj , type convert, ---
	 map<int,map<int,fntConversionType>>  * CScriptClass::mapTypeConversion=NULL;
*/




	/*unsigned char CScriptClass::getIdxScriptClass(const string & v){
		return getIdxScriptClass_Internal(v);
	}*/



	bool CScriptClass::is_c_class(){

		 return ((symbol_info.properties & SYMBOL_INFO_PROPERTY::PROPERTY_C_OBJECT_REF) != 0);
	}
	//------------------------------------------------------------



	/* bool CScriptClass::initStaticVars(){

			vec_script_class_node = new vector<CScriptClass *>();


			if(vec_script_class_node->size() != 0){ // must be 0 in order to register basic types...
				THROW_RUNTIME_ERROR("vector class factory not 0");
				return false;
			}

			CScriptClass *main_class=NULL;
			mapTypeConversion = new map<int, map<int, fntConversionType>>();

			VOID_TYPE_STR = new string(typeid(void).name());
			INT_PTR_TYPE_STR = new string(typeid(int *).name());
			FLOAT_PTR_TYPE_STR = new string(typeid(float *).name());
			CONST_CHAR_PTR_TYPE_STR = new string(typeid(const char *).name());
			STRING_PTR_TYPE_STR = new string(typeid(string *).name());
			BOOL_PTR_TYPE_STR = new string(typeid(bool *).name());

			// particular case for return ... for not to force user returning pointer (this is the only vars allowed, sorry!)
			BOOL_TYPE_STR = new string(typeid(bool).name());
			INT_TYPE_STR = new string(typeid(int).name());
			UNSIGNED_INT_TYPE_STR = new string(typeid(unsigned int).name());
			INTPTR_T_TYPE_STR = new string(typeid(intptr_t).name());
			FLOAT_TYPE_STR = new string(typeid(float).name());
			STACK_ELEMENT_PTR= new string(typeid(tStackElement *).name());

			//------------------------------------------------------------------------------------------------------------------------------------------------------------------

			// REGISTER BUILT IN SCRIPT TYPES
			// MAIN CLASS (0)
			if((main_class=registerClass(MAIN_SCRIPT_CLASS_NAME,"")) == NULL) return false; // 0
			if(main_class->idxScope!=IDX_GLOBAL_SCOPE){
				THROW_RUNTIME_ERROR("Error initializing global scope");
				return false;
			}

			// REGISTER BUILT IN C TYPES
			REGISTER_BUILT_IN_TYPE(void,IDX_TYPE_VOID_C);
			REGISTER_BUILT_IN_TYPE(int *,IDX_TYPE_INT_PTR_C);
			REGISTER_BUILT_IN_TYPE(float *,IDX_TYPE_FLOAT_PTR_C);
			REGISTER_BUILT_IN_TYPE(const char *,IDX_TYPE_CONST_CHAR_PTR_C);
			REGISTER_BUILT_IN_TYPE(string *,IDX_TYPE_STRING_PTR_C);
			REGISTER_BUILT_IN_TYPE(bool *,IDX_TYPE_BOOL_PTR_C);
			REGISTER_BUILT_IN_TYPE(int,IDX_TYPE_INT_C);
			REGISTER_BUILT_IN_TYPE(unsigned int,IDX_TYPE_UNSIGNED_INT_C);
			REGISTER_BUILT_IN_TYPE(intptr_t,IDX_TYPE_INTPTR_T_C);
			REGISTER_BUILT_IN_TYPE(float,IDX_TYPE_FLOAT_C);
			REGISTER_BUILT_IN_TYPE(bool,IDX_TYPE_BOOL_C);

			// REGISTER BUILT IN CLASS TYPES
			REGISTER_BUILT_IN_CLASS(tStackElement,IDX_STACK_ELEMENT);
			REGISTER_BUILT_IN_CLASS(CScriptVariable,IDX_CLASS_SCRIPT_VAR);
			REGISTER_BUILT_IN_CLASS(CStringScriptVariable,IDX_CLASS_STRING);
			REGISTER_BUILT_IN_CLASS(CVectorScriptVariable,IDX_CLASS_VECTOR);
			REGISTER_BUILT_IN_CLASS(CFunctorScriptVariable,IDX_CLASS_FUNCTOR);
			REGISTER_BUILT_IN_CLASS(CStructScriptVariable,IDX_CLASS_STRUCT);


			//------------------------------------------------------------------------------------------------------------------------------------------------------------------

			// rgister basic classes (Warning!!! must match idx !!! and the order is important!!!)

			//----------------------------------------------------------------------
			// From here you defined all basic, start define hierarchy

			// register custom functions ...
			if(!class_C_baseof<CVectorScriptVariable,CScriptVariable>()) return false;
			if(!class_C_baseof<CFunctorScriptVariable,CScriptVariable>()) return false;
			if(!class_C_baseof<CStructScriptVariable,CScriptVariable>()) return false;


			//------------------------------------------------------------------------------------------------------------
			// Let's register functions,...
			// register c function's

			// MAIN FUNCTION (0)...
			if((main_class->registerFunctionMember(MAIN_SCRIPT_FUNCTION_OBJECT_NAME)) == NULL) return false;


			if(!register_C_Function("print",print)) return false;


			if(!register_C_Function("error",internal_print_error)) return false;


			if(!register_C_FunctionMember<CVectorScriptVariable>("size",&CVectorScriptVariable::size)) return false;
			if(!register_C_FunctionMember<CVectorScriptVariable>("push",static_cast<void (CVectorScriptVariable:: *)(tStackElement *)>(&CVectorScriptVariable::push))) return false;
			if(!register_C_FunctionMember<CVectorScriptVariable>("pop",&CVectorScriptVariable::pop)) return false;


			if(!register_C_FunctionMember<CStructScriptVariable>("add",&CStructScriptVariable::add_attr)) return false;
			if(!register_C_FunctionMember<CStructScriptVariable>("remove",&CStructScriptVariable::remove_attr)) return false;
			if(!register_C_FunctionMember<CStructScriptVariable>("size",&CStructScriptVariable::size)) return false;
			return true;
	 }*/

	/* void CScriptClass::destroyStaticVars(){


		// we have to destroy all allocated constructor/destructor ...
		for (unsigned i = 0; i < vec_script_class_node->size(); i++) {

			delete vec_script_class_node->at(i);
		}

		vec_script_class_node->clear();
		delete vec_script_class_node;
		vec_script_class_node=NULL;


		 if(VOID_TYPE_STR != NULL){
			delete VOID_TYPE_STR;
		 }

		 if(INT_PTR_TYPE_STR != NULL){
			 delete INT_PTR_TYPE_STR;
		 }

		 if(FLOAT_PTR_TYPE_STR != NULL){
			 delete FLOAT_PTR_TYPE_STR;
		 }

		 if(CONST_CHAR_PTR_TYPE_STR != NULL){
			 delete CONST_CHAR_PTR_TYPE_STR;
		 }

		 if(STRING_PTR_TYPE_STR != NULL){
			 delete STRING_PTR_TYPE_STR;
		 }

		 if(BOOL_PTR_TYPE_STR != NULL){
			 delete BOOL_PTR_TYPE_STR;
		 }

		 if (BOOL_TYPE_STR != NULL) {
			 delete BOOL_TYPE_STR;
		 }
		 
		 if (INT_TYPE_STR != NULL) {
			 delete INT_TYPE_STR;
		 }
		 
		 if (UNSIGNED_INT_TYPE_STR != NULL) {
			 delete UNSIGNED_INT_TYPE_STR;
		 }

		 if (INTPTR_T_TYPE_STR != NULL) {
			 delete INTPTR_T_TYPE_STR;
		 }

		 if (FLOAT_TYPE_STR != NULL) {
			 delete FLOAT_TYPE_STR;
		 }
		 
		 if (STACK_ELEMENT_PTR != NULL) {
			 delete STACK_ELEMENT_PTR;
		 }

		 if (mapTypeConversion != NULL) {
			 delete mapTypeConversion;
		 }

		 VOID_TYPE_STR=NULL;
		 INT_PTR_TYPE_STR=NULL;
		 FLOAT_PTR_TYPE_STR=NULL;
		 CONST_CHAR_PTR_TYPE_STR=NULL;
		 STRING_PTR_TYPE_STR=NULL;
		 BOOL_PTR_TYPE_STR=NULL;
		 BOOL_TYPE_STR = NULL;
		 INT_TYPE_STR = NULL;
		 UNSIGNED_INT_TYPE_STR = NULL;
		 INTPTR_T_TYPE_STR=NULL;
		 FLOAT_TYPE_STR = NULL;
		 STACK_ELEMENT_PTR = NULL;
		 mapTypeConversion = NULL;
	 }*/

	 CScriptClass::CScriptClass(unsigned char _idxClass, short _idxScope):CCommonClassFunctionData(_idxClass,_idxScope){

			classPtrType="";
			c_destructor = NULL;
			c_constructor=NULL;
			idx_function_member_constructor =ZS_UNDEFINED_IDX;
	}

	/*int getNumberArgsfromFunctionRefNode(PASTNode ast_node){
		if(ast_node->node_type == NODE_TYPE::FUNCTION_REF_NODE){ // function

			ast_node = AST_NODE(ast_node->idxAstParent);

			if(ast_node == NULL){
				THROW_RUNTIME_ERROR("internal error, parent null");
				return false;
			}

			if(ast_node->node_type != NODE_TYPE::CALLING_OBJECT_NODE){
				THROW_RUNTIME_ERROR("internal error, expected calling object node");
				return false;
			}

			if(ast_node->children.size()!=2){
				THROW_RUNTIME_ERROR("internal error, expected calling function 2 children");
				return false;
			}

			ast_node = AST_NODE(ast_node->children[1]);

			if(ast_node == NULL){
				THROW_RUNTIME_ERROR("internal error, expected args node");
				return false;
			}

			if(ast_node->node_type != NODE_TYPE::ARGS_PASS_NODE){
				THROW_RUNTIME_ERROR("internal error, espected ARGS PASS NODE");
				return false;
			}

			return ast_node->children.size();
		}else{
			THROW_RUNTIME_ERROR("internal error, espected FUNCTION_REF_NODE");
		}
		return -1;
	}

	int getNumberArgsfromFunctionObjectNode(PASTNode ast_node){
		if(ast_node->node_type == NODE_TYPE::FUNCTION_OBJECT_NODE){ // function

			if(ast_node->children.size()!=2){
				THROW_RUNTIME_ERROR("internal error, expected calling function 1 children");
				return false;
			}

			ast_node = AST_NODE(ast_node->children[0]);

			if(ast_node == NULL){
				THROW_RUNTIME_ERROR("internal error, expected args node");
				return false;
			}

			if(ast_node->node_type != NODE_TYPE::ARGS_DECL_NODE){
				THROW_RUNTIME_ERROR("internal error, espected ARGS PASS NODE");
				return false;
			}

			return ast_node->children.size();
		}else{
			THROW_RUNTIME_ERROR("internal error, espected FUNCTION_OBJECT_NODE");
		}

		return -1;
	}*/
#if 0
	bool CScriptClass::searchVarFunctionSymbol(CCommonClassFunctionData * scope_info, tInstruction *iao, int current_function, bool & symbol_not_found, unsigned int param_scope_type){

		int idx=0;
		symbol_not_found = true;
		char n_args_to_find =-1;
		PASTNode ast_node = AST_NODE(iao->idxAstNode);
		string symbol_to_find =ast_node->symbol_value;//CCompiler::makeSymbolRef(ast_node->symbol_value, ast_node->idxScope);

		if(ast_node->node_type == NODE_TYPE::FUNCTION_REF_NODE){ // function

			n_args_to_find = getNumberArgsfromFunctionRefNode(ast_node);
		}

		unsigned int iao_scope = GET_INS_PROPERTY_SCOPE_TYPE(iao->instruction_properties);

		if(iao_scope & INS_PROPERTY_SUPER_SCOPE){ // try deduce local/global

			if(current_function > 0){ // minimum have to have a 1

				CScriptFunction *sfo =  GET_SCRIPT_FUNCTION(scope_info->local_symbols.function[current_function]);//.object_info;
				symbol_to_find = sfo->symbol_info.symbol_ref;
				short idxScope=CCompiler::getIdxScopeFromSymbolRef(symbol_to_find);

				if(idxScope == -1){
					return false;
				}

				int idx_super=-1;
				bool is_c=false;

				for(int i = current_function-1; i >= 0 && idx_super==-1; i--){
					CScriptFunction * sfo = GET_SCRIPT_FUNCTION(scope_info->local_symbols.function[i]);
					string symbol_ref=CCompiler::getSymbolNameFromSymbolRef(sfo->symbol_info.symbol_ref);
					string symbol_ref_to_find=CCompiler::getSymbolNameFromSymbolRef(symbol_to_find);

					if((symbol_ref == symbol_ref_to_find) && ((int)sfo->m_arg.size() == n_args_to_find)){ // match name and args ...
						is_c = (sfo->symbol_info.properties & PROPERTY_C_OBJECT_REF) != 0;
						idx_super=i;
					}
				}

				if(idx_super!= -1){
					 REMOVE_SCOPES(iao->instruction_properties);
					 iao->instruction_properties |= param_scope_type;
					 iao->index_op1 = LOAD_TYPE_FUNCTION;
					 iao->index_op2 = idx_super;
					 iao->instruction_properties |= (is_c ? INS_PROPERTY_DEDUCE_C_CALL : 0);
					 //iao->idxFunction = idxFunction;
					 return true;

				}
				else{ // symbol not found ...
					return false;
				}
			}
			else{
				return false;
			}
		}
		else{

			int idx_scope=ast_node->idxScope;
			bool partial_c_class= false;

			if(info_function->symbol_info.idxClass != ZS_INVALID_CLASS){
				CScriptClass *sc=CScriptClass::getScriptClassByIdx(info_function->symbol_info.idxClass);

				if(symbol_to_find == "this" && (iao_scope & INS_PROPERTY_THIS_SCOPE)){ // trivial is the first symbol we find...
					 REMOVE_SCOPES(iao->instruction_properties);
					 iao->instruction_properties |= INS_PROPERTY_THIS_SCOPE;
					 iao->index_op1 = LOAD_TYPE_VARIABLE;
					 iao->index_op2 = ZS_THIS_IDX;
					 return true;
				}

				 partial_c_class=(info_function->symbol_info.properties&SYMBOL_INFO_PROPERTY::PROPERTY_C_OBJECT_REF) !=0;
				while( sc->idxBaseClass.size()>0 && !partial_c_class){

					sc=CScriptClass::getScriptClassByIdx(sc->idxBaseClass[0]); // get base class...
					partial_c_class|=(sc->metadata_info.object_info.symbol_info.properties&SYMBOL_INFO_PROPERTY::PROPERTY_C_OBJECT_REF) !=0;
				}

				if(iao_scope & INS_PROPERTY_THIS_SCOPE){ // start from class scope to find its variable/function member...
					sc=CScriptClass::getScriptClassByIdx(info_function->symbol_info.idxClass);
					idx_scope=AST_NODE(sc->metadata_info.object_info.symbol_info.idxAstNode)->idxScope;
				}
			}


			bool variable_in_main_class=SCOPE_IN_MAIN_CLASS(ast_node->idxScope);

			 bool end=false;


			 while(!end){ // we try all scopes until match symbols at right scope...

				 symbol_to_find=CCompiler::makeSymbolRef(ast_node->symbol_value,idx_scope);

				 if((idx=CScriptFunction::getIdxFunctionObject(info_function,symbol_to_find,n_args_to_find,false))!=-1){
					 REMOVE_SCOPES(iao->instruction_properties);
					 iao->instruction_properties |= param_scope_type;
					 iao->index_op1 = LOAD_TYPE_FUNCTION;
					 iao->index_op2 = idx;
					 if((GET_FUNCTION_INFO(info_function->local_symbols.function[idx])->symbol_info.properties & SYMBOL_INFO_PROPERTY::PROPERTY_C_OBJECT_REF) == SYMBOL_INFO_PROPERTY::PROPERTY_C_OBJECT_REF){ // set as -1 to search the right signature ...
						 iao->index_op2 = -1;
					 }
					 return true;
				 }else {

					 if((idx=CScriptFunction::getIdxVariableSymbol(info_function,symbol_to_find, false))!=-1){
						 REMOVE_SCOPES(iao->instruction_properties);
						 iao->instruction_properties |= param_scope_type;
						 iao->index_op1 = LOAD_TYPE_VARIABLE;
						 iao->index_op2 = idx;
						 return true;
					 }
				 }

			 	if(idx_scope >= 0){ // var local or class member ?
					 CScope *sc=SCOPE_NODE(idx_scope);
				 	 idx_scope=sc->getIdxParent();
			 	}

			 	if(idx_scope<0){

			 		idx_scope=ZS_UNDEFINED_IDX;

			 		if(partial_c_class){ // var in c scope ?

			 			idx_scope = IDX_C_CLASS_SCOPE;
			 			partial_c_class=false; // <-- set it false to tell that is already test

			 		}else{ // finally try global...

						 if(!variable_in_main_class){ // try global scope...
							 variable_in_main_class=true;
							 CScriptFunction *main=MAIN_SCRIPT_FUNCTION_OBJECT;
							 info_function=&main->object_info; // set main function as other...
							 idx_scope=0; // set scope global and try last...
							 param_scope_type=0; // override to global scope...
						 }
			 		}
			 	 }

			 	 end = idx_scope == ZS_UNDEFINED_IDX;


			 }
		}
		symbol_not_found = false;
		 return false;
	}

	void CScriptClass::buildScopeVariablesBlock(CScriptFunction *root_class_irfs ){
		/// PRE: base_class_irfs must be info of root class.
		 bool is_main_function = MAIN_SCRIPT_FUNCTION_OBJECT == root_class_irfs;
		 PASTNode ast = AST_NODE(root_class_irfs->object_info.symbol_info.idxAstNode);

		 if(ast!=NULL){

			 int idxScope =ast->idxScope;
			 if(!is_main_function) {// is not main function

				 if(ast->node_type == NODE_TYPE::KEYWORD_NODE){
					 if(ast->keyword_info == KEYWORD_TYPE::FUNCTION_KEYWORD ){
						 idxScope = AST_NODE(ast->children[1])->idxScope; // pass scope block ...
					 }
				 }
			 }

			 if(idxScope != -1){

				 struct tInfoVarScopeBlockRegister{
					 int idxScope;
					 vector<int> var_index;
				 };

				 vector<CScope *> *list = CScope::getVectorScopeNode();
				 vector<tVariableSymbolInfo> *vs = &root_class_irfs->object_info.local_symbols.variable;
				 vector<tInfoVarScopeBlockRegister> vec_ivsb;
				 for(unsigned i = 0;i < list->size(); i++){ // register index var per scope ...
					 tInfoVarScopeBlockRegister ivsb;

					 ivsb.idxScope = list->at(i)->idxScope;

					 for(unsigned v = 0;v < vs->size(); v++){ // register index var per scope ...

						 PASTNode ast_var = AST_NODE(vs->at(v).idxAstNode);

						 if(ast_var !=NULL){

							if(ast_var->idxScope == ivsb.idxScope){
								ivsb.var_index.push_back(v);
							}
						 }
					 }
					 vec_ivsb.push_back(ivsb);
				 }

				 root_class_irfs->scope_info.lut_scope_symbol = (tInfoVarScopeBlock*)malloc(vec_ivsb.size()*sizeof(tInfoVarScopeBlock));
				 root_class_irfs->object_info.n_lut_scope_symbols = vec_ivsb.size();

				 for(unsigned i = 0; i < vec_ivsb.size(); i++){
					 root_class_irfs->scope_info.lut_scope_symbol[i].idxScope = vec_ivsb[i].idxScope;
					 root_class_irfs->scope_info.lut_scope_symbol[i].n_var_index = (char)vec_ivsb[i].var_index.size();
					 root_class_irfs->scope_info.lut_scope_symbol[i].var_index = (int *)malloc(sizeof(int)*vec_ivsb[i].var_index.size());
					 for(unsigned j = 0; j < vec_ivsb[i].var_index.size(); j++){
						 root_class_irfs->scope_info.lut_scope_symbol[i].var_index[j] = vec_ivsb[i].var_index[j];
					 }
				 }
			 }
		 }
	}



	bool CScriptClass::updateReferenceSymbols(){


		 int idx_main_function = ((*vec_script_class_node)[IDX_START_SCRIPTVAR]->scope_info.local_symbols.function[0]);
		 CScriptFunction  *main_function = GET_SCRIPT_FUNCTION((*vec_script_class_node)[IDX_START_SCRIPTVAR]->scope_info.local_symbols.function[0]);
		 zs_print_debug_cr("DEFINED CLASSES");
		 vector<int>  mrf; // aux vector for collect function obj info.
		 bool symbol_found = false;

		 // For each class (after primitives)...
		 for(unsigned i = IDX_START_SCRIPTVAR; i < (*vec_script_class_node).size(); i++){

			 mrf.clear();
			 if(i==IDX_START_SCRIPTVAR){ // First entry (MAIN_CLASS), load global functions....
				 mrf.push_back(idx_main_function);//->object_info.local_symbols.function;
				 for(unsigned h=0; h<  main_function->scope_info.local_symbols.function.size(); h++){
					 mrf.push_back(main_function->scope_info.local_symbols.function[h]);
				 }
			 }else{ // any other class
				 for(unsigned h=0; h<  (*vec_script_class_node)[i]->scope_info.local_symbols.function.size(); h++){
					 mrf.push_back((*vec_script_class_node)[i]->scope_info.local_symbols.function[h]);
				 }
			 }

			 // For each function on the class ...
			 for(unsigned k=0; k < mrf.size();k++){


				 CScriptFunction * info_function = GET_SCRIPT_FUNCTION(mrf[k]);
				 bool is_main_class = i == IDX_CLASS_MAIN;
				 bool is_main_function = is_main_class && k==0;
				 CScriptClass * _belonging_class = (*vec_script_class_node)[i];

				 buildScopeVariablesBlock(info_function);

				 if(is_main_class){
					 if(is_main_function){
						 zs_print_debug_cr("Main Function");
					 }
					 else{
						 zs_print_debug_cr("Global function %s...",info_function->symbol_info.symbol_ref.c_str());
					 }
				 }else{
					 zs_print_debug_cr("Function %s::%s...",_belonging_class->metadata_info.symbol_info.symbol_ref.c_str(),info_function->symbol_info.symbol_ref.c_str());
				 }

				 if(info_function->object_info.asm_op != NULL){

					 int idx_op = 0;
					 for(tInstruction *iao=info_function->asm_op; iao->operator_type!=END_FUNCTION; iao++,idx_op++){

						 if(iao->operator_type==OP_CODE::LOAD){

							 string base_class = _belonging_class->symbol_info.symbol_ref;

							 CCommonClassFunctionData *sfi=NULL;
							 unsigned int scope_type = GET_INS_PROPERTY_SCOPE_TYPE(iao->instruction_properties);

							 if(scope_type & INS_PROPERTY_ACCESS_SCOPE ){
								 iao->index_op1 = LOAD_TYPE_VARIABLE;
								 iao->index_op2 = -1; // keep unknow reference for first time ..
							 }
							 else {

								 if((scope_type & INS_PROPERTY_THIS_SCOPE) | (scope_type & INS_PROPERTY_SUPER_SCOPE)){

									 sfi = &_belonging_class->scope_info;
								 }

								 if(iao->index_op1 == LOAD_TYPE_NOT_DEFINED){
									 PASTNode iao_node = AST_NODE(iao->idxAstNode);

									 string symbol_to_find =iao_node->symbol_value;

									 if(scope_type & INS_PROPERTY_ACCESS_SCOPE){

										 writeErrorMsg(GET_INSTRUCTION_FILE_LINE(iao->idxAstNode),"Symbol \"%s\" will solved at run-time", AST_SYMBOL_VALUE_CONST_CHAR(iao->idxAstNode));

									 }else{

										// this/super required ...
										 if(sfi != NULL){
											 // make ref var from class scope ...
											 // search global...
											 if(scope_type & INS_PROPERTY_SUPER_SCOPE){
												 symbol_to_find = GET_SCRIPT_FUNCTION(sfi->local_symbols.function[k])->symbol_info.symbol_ref;
											 }

											 if(!searchVarFunctionSymbol(sfi,iao,k,symbol_found,scope_type)){
												 if(scope_type & INS_PROPERTY_SUPER_SCOPE){
													 PASTNode ast_node = AST_NODE(iao->idxAstNode);
													 int n_args_to_find = getNumberArgsfromFunctionRefNode(ast_node);
													 string arg_str="";
													 for(int i = 0; i < n_args_to_find; i++){
														 if(i>0){
															 arg_str+=",";
														 }
														 arg_str+="arg"+CZetScriptUtils::intToString(i);
													 }
													 writeErrorMsg(GET_INSTRUCTION_FILE_LINE(iao->idxAstNode),"Cannot find \"super\" function for \"%s(%s)\". Has the class an inheritance or parent has the function ?", symbol_to_find.c_str(),arg_str.c_str());
												 }
												 else{
													 writeErrorMsg(GET_INSTRUCTION_FILE_LINE(iao->idxAstNode),"Symbol \"%s::%s\" not found", base_class.c_str(),symbol_to_find.c_str());
												 }
												 return false;
											 }
										 }
										 else{ //normal symbol...
											 string symbol_to_find =CCompiler::makeSymbolRef(iao_node->symbol_value,iao_node->idxScope);
											 // search local...
											 if(!searchVarFunctionSymbol(&info_function->scope_info,iao,k,symbol_found,INS_PROPERTY_LOCAL_SCOPE)){

												 // search global...
												 CScriptFunction * mainFunctionInfo = MAIN_SCRIPT_FUNCTION_OBJECT;
												 if(!searchVarFunctionSymbol(&mainFunctionInfo->scope_info,iao,k,symbol_found,0)){
														PASTNode ast_node = AST_NODE(iao->idxAstNode);

														if(ast_node->node_type == NODE_TYPE::FUNCTION_REF_NODE){ // function

															if(!symbol_found){
																writeErrorMsg(GET_INSTRUCTION_FILE_LINE(iao->idxAstNode),"function \"%s\" not registered", iao_node->symbol_value.c_str() );
															}
															else{
																writeErrorMsg(GET_INSTRUCTION_FILE_LINE(iao->idxAstNode),"Cannot match function \"%s\" with %i args",iao_node->symbol_value.c_str(),getNumberArgsfromFunctionRefNode(ast_node) );
															}

														}
														else{
															writeErrorMsg(GET_INSTRUCTION_FILE_LINE(iao->idxAstNode),"Symbol \"%s\" not found",iao_node->symbol_value.c_str());
														}
													 return false;
												 }
											 }
										}
									}
								 }
							 }

						 }else  if(iao->operator_type==OP_CODE::CALL){ // overrides variable type as function ...
							 // check whether access scope ...
							 if(info_function->asm_op[iao->index_op2].operator_type ==OP_CODE::LOAD  && // e
								(GET_INS_PROPERTY_SCOPE_TYPE(info_function->asm_op[iao->index_op2].instruction_properties) & INS_PROPERTY_ACCESS_SCOPE)){

								 info_function->asm_op[iao->index_op2].index_op1 = LOAD_TYPE_FUNCTION;
								 info_function->asm_op[iao->index_op2].index_op2 = -1;
							 }
						 }
					 }

				 }

				 if(!is_main_function){ // process all function symbols ...
					 for(unsigned m=0; m < info_function->scope_info.local_symbols.function.size(); m++){
						 if(!updateFunctionSymbols(
								 info_function->scope_info.local_symbols.function[m],
								 info_function->symbol_info.symbol_ref,m)){
							 return false;
						 }
					 }
				 }
			 }

			 // check whether class has aritmethic metamethod but it hasn't a set metamethod....
			 bool it_has_any_metamethod=false;

			 for(int m = 0; m < MAX_METAMETHOD_OPERATORS && !it_has_any_metamethod; m++){
				 if((*vec_script_class_node)[i]->metamethod_operator[m].size()>0){
					 it_has_any_metamethod=true;
				 }
			 }

			 if(it_has_any_metamethod && ((*vec_script_class_node)[i]->metamethod_operator[SET_METAMETHOD].size()==0)){
				 fprintf(stderr,"class \"%s\" it has some metamethods but it doesn't have _set metamethod (aka '='). The variable will be override on assigment.\n"
						 ,(*vec_script_class_node)[i]->symbol_info.symbol_ref.c_str()
				);
			 }
		 }

		 // update global variables, only C refs...
		 for(unsigned i = 0;i < main_function->scope_info.local_symbols.variable.size();i++){
			 if(main_function->scope_info.local_symbols.variable[i].properties & PROPERTY_C_OBJECT_REF){
				 CURRENT_VM->iniStackVar(
						 i,

						 CScriptClass::C_REF_InfoVariable_2_StackElement(
								 	&main_function->scope_info.local_symbols.variable[i],
									(void *)main_function->scope_info.local_symbols.variable[i].ref_ptr
				 ));
			 }
		 }
		 // finally update constant symbols ...
		 return true;
	}
#endif



	/*tVariableSymbolInfo * CScriptClass::registerVariableSymbol(const string & class_name,const string & var_name){


		CScriptClass *rc = GET_SCRIPT_CLASS(class_name);// getIdxRegisteredClass(class_name);

		if(rc != NULL){//idxClass != -1){

			string symbol_ref = CEval::makeSymbolRef(var_name,rc->idxScope);

			if(rc->getVariable(symbol_ref) != NULL){
				writeErrorMsg(GET_INSTRUCTION_FILE_LINE(idxAstNode),"symbol variable \"%s::%s\" already exist",class_name.c_str(),var_name.c_str());
				return NULL;
			}


			CCommonClassFunctionData *scope_info=&rc->scope_info;

			tVariableSymbolInfo info_var;
			info_var.idxClass = rc->symbol_info.idxClass;
			info_var.idxAstNode = idxAstNode;
			info_var.symbol_ref =var_name;
			info_var.idxSymbol = (short)scope_info->local_symbols.variable.size();
			scope_info->local_symbols.variable.push_back(info_var);

			return &scope_info->local_symbols.variable[scope_info->local_symbols.variable.size()-1];

			return rc->registerVariable(var_name,)
		}else{
			writeErrorMsg(GET_INSTRUCTION_FILE_LINE(idxAstNode),"class \"%s\" not exist",class_name.c_str());
			return NULL;
		}

		return NULL;
	}*/


	CScriptFunction * CScriptClass::registerFunctionMember(const string & function_name, vector<tArgumentInfo> args, int idx_return_type,intptr_t ref_ptr, unsigned short properties){

		CScriptFunction * sf = registerFunction( function_name,  args,  idx_return_type, ref_ptr,  properties);
		string class_name=symbol_info.symbol_ref;

		if(function_name == class_name){
			idx_function_member_constructor = (char)m_function.size();
		}


		// check if metamethod...
		for(int i = 0; i < MAX_METAMETHOD_OPERATORS; i++){
			if(STRCMP(zetscript::CScriptClassFactory::getInstance()->getMetamethod((METAMETHOD_OPERATOR)i),==,function_name.c_str())){

				metamethod_operator[i].push_back(sf);

				zs_print_debug_cr("Registered metamethod %s::%s",class_name.c_str(), function_name.c_str());
				break;
			}
		}
		return sf;

	}

	/*bool  CScriptClass::existFunctionMember(const string & function_name, int n_params){

		return CScriptFunction::getFunction(function_name, n_params) != ZS_UNDEFINED_IDX;
	}*/

	//-----



	CScriptClass::~CScriptClass(){

		if ((symbol_info.properties & PROPERTY_C_OBJECT_REF) == PROPERTY_C_OBJECT_REF) {

			if (c_constructor !=NULL) {
				delete c_constructor;
				c_constructor=NULL;
			}

			if (c_destructor != NULL) {
				delete c_destructor;
				c_destructor=NULL;
			}

		}
	}
}



