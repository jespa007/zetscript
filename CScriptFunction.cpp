/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
#include "CZetScript.h"

namespace zetscript{

	//vector<CScriptFunction *> 	* CScriptFunction::vec_script_function_node=NULL;



/*	void CScriptFunction::initStaticVars() {
		vec_script_function_node = new vector<CScriptFunction *> ();
	}

	void CScriptFunction::destroyStaticVars() {


	}*/


	void CScriptFunction::buildLutScopeSymbols(){

			if(idxScope < 0){ // it could be undefined or C
				return;
			}

			if(m_variable.size() == 0){ // no elements...
				return;
			}

			/// PRE: base_class_irfs must be info of root class.
			 bool is_main_function = idxScope == IDX_GLOBAL_SCOPE;


			 struct tInfoVarScopeBlockRegister{
				 int idxScope;
				 vector<int> var_index;
			 };

			 vector<CScope *> *list = CScopeFactory::getInstance()->getVectorScopeNode();
			 vector<tInfoVarScopeBlockRegister> vec_ivsb;
			 std::map<short,tInfoVarScopeBlockRegister> map_scope_register;
			 //for(unsigned i = 0;i < list->size(); i++){ // register index var per scope ...
				 //tInfoVarScopeBlockRegister ivsb;

				 //ivsb.idxScope = list->at(i)->idxScope;

				 for(unsigned v = 0;v < m_variable.size(); v++){ // register index var per scope ...


					map_scope_register[m_variable[v].idxScope].idxScope=m_variable[v].idxScope;
					map_scope_register[m_variable[v].idxScope].var_index.push_back(v);



					 //PASTNode ast_var = AST_NODE(vs->at(v).idxAstNode);

					 //if(ast_var !=NULL){

						//if(ast_var->idxScope == ivsb.idxScope){
						//	ivsb.var_index.push_back(v);
						//}
					 //}
				 }
				 //vec_ivsb.push_back(ivsb);
			 //}

			 lut_scope_symbol = (tInfoVarScopeBlock*)malloc(map_scope_register.size()*sizeof(tInfoVarScopeBlock));
			 n_lut_scope_symbols =map_scope_register.size();

			 int i=0;
			 for(std::map<short,tInfoVarScopeBlockRegister>::iterator e = map_scope_register.begin(); e != map_scope_register.end(); e++){

				 tInfoVarScopeBlockRegister ivs = map_scope_register[e->first];

				 lut_scope_symbol[i].idxScope = ivs.idxScope;
				 lut_scope_symbol[i].n_var_index = (char)ivs.var_index.size();
				 lut_scope_symbol[i].var_index = (int *)malloc(sizeof(int)*ivs.var_index.size());
				 for(unsigned j = 0; j < ivs.var_index.size(); j++){
					 lut_scope_symbol[i].var_index[j] = ivs.var_index[j];
				 }
				 i++;
			 }


		}


	CScriptFunction::CScriptFunction(unsigned char _idxClass,short _idxScope):CCommonClassFunctionData(_idxClass,_idxScope){
		idx_return_type = ZS_UNDEFINED_IDX;
		idxScriptFunction = ZS_UNDEFINED_IDX;
		instruction=NULL;
		lut_scope_symbol=NULL;
		n_lut_scope_symbols=0;

	}


	int 		 CScriptFunction::getLine(tInstruction * ins){
		return -1;
	}

	const char * CScriptFunction::getSymbol(tInstruction * ins){
		return "unknown";
	}

	const char * CScriptFunction::getFile(){
		return "unknown";
	}





	/*bool CScriptFunction::updateFunctionSymbols(const string & parent_symbol, int n_function){


		//tFunctionInfo * info_function = &sfo->object_info;
		bool symbol_found=false;

		zs_print_info_cr("processing function %s -> %s",parent_symbol.c_str(),info_function->symbol_info.symbol_ref.c_str());

		buildLutScopeSymbols();

		//int ;
		int idx_op=0;
		 for(PtrInstruction iao = instruction; iao->op_code != OP_CODE::END_FUNCTION;iao++,idx_op++){

			 unsigned int scope_type = GET_INS_PROPERTY_SCOPE_TYPE(iao->properties);

			 if(iao->op_code==OP_CODE::LOAD){

				 if(scope_type & INS_PROPERTY_ACCESS_SCOPE ){
					 iao->index_op1 = LOAD_TYPE_VARIABLE;
					 iao->index_op2 = idx_op-1;
				 }
				 else {

					 if(iao->index_op1 == LOAD_TYPE_NOT_DEFINED){

								 string symbol_to_find =AST_SYMBOL_VALUE(iao->idxAstNode);

								 if(scope_type & INS_PROPERTY_ACCESS_SCOPE){

									 writeErrorMsg(GET_INSTRUCTION_FILE_LINE(iao->idxAstNode),"Symbol defined \"%s\" will solved at run-time", symbol_to_find.c_str());
								 }
								 else{
									 // search symbol...
									 if(!searchVarFunctionSymbol(info_function,iao,n_function,symbol_found,INS_PROPERTY_LOCAL_SCOPE)){

										 // search global...
										 //if(!searchVarFunctionSymbol(&MAIN_SCRIPT_FUNCTION_OBJECT->object_info,iao,n_function,symbol_found,0)){
												PASTNode ast_node = AST_NODE(iao->idxAstNode);

												if(ast_node->node_type == NODE_TYPE::FUNCTION_REF_NODE){ // function

													if(!symbol_found){
														writeErrorMsg(GET_INSTRUCTION_FILE_LINE(iao->idxAstNode),"function \"%s\" not registered", CCompiler::getSymbolNameFromSymbolRef(symbol_to_find).c_str() );
													}
													else{
														writeErrorMsg(GET_INSTRUCTION_FILE_LINE(iao->idxAstNode),"Cannot match function \"%s\" with %i args",CCompiler::getSymbolNameFromSymbolRef(symbol_to_find).c_str(),getNumberArgsfromFunctionRefNode(ast_node) );
													}

												}
												else{
													writeErrorMsg(GET_INSTRUCTION_FILE_LINE(iao->idxAstNode),"Symbol defined \"%s\"not found", CCompiler::getSymbolNameFromSymbolRef(symbol_to_find).c_str());
												}
											 return false;
										 //}

									 }
							 }
					 }
				 }

			 }else  if(iao->operator_type==OP_CODE::CALL){ // overrides variable type as function ...
				 // check whether access scope ...
				 if(info_function->asm_op[iao->index_op2].operator_type ==OP_CODE::LOAD  && // e
						 (GET_INS_PROPERTY_SCOPE_TYPE(info_function->asm_op[iao->index_op2].properties) & INS_PROPERTY_ACCESS_SCOPE)){

					 info_function->asm_op[iao->index_op2].index_op1 = LOAD_TYPE_FUNCTION;
					 info_function->asm_op[iao->index_op2].index_op2 = iao->index_op2-1;
					// iao->properties |= INS_PROPERTY_CALLING_OBJECT;
				 }

			 }
		 }


		 vector<int> *mrf = &info_function->local_symbols.function;

		 for(unsigned k=0; k < mrf->size();k++){

			 if(!updateFunctionSymbols(mrf->at(k), info_function->symbol_info.symbol_ref, k)){
				 return false;
			 }
		 }

		return true;
	}*/

	tVariableSymbolInfo *	CScriptFunction::registerVariable(const string & variable_name, const string & c_type, intptr_t ref_ptr, unsigned short properties)
	{
		tVariableSymbolInfo *vsi=CCommonClassFunctionData::registerVariable(this->idxScope,  variable_name,  c_type,  ref_ptr,   properties);

		tStackElement se = {0,0,STK_PROPERTY_TYPE_UNDEFINED};

		if(properties &  PROPERTY_C_OBJECT_REF) // convert c ref var into stack element. This should be consistent in the whole execution.
			se=CScriptClassFactory::C_REF_InfoVariable_2_StackElement(
										 	vsi,
											(void *)vsi->ref_ptr);

		CURRENT_VM->addGlobalVar(se);

		return vsi;

	}




	CScriptFunction::~CScriptFunction(){

	}




}
