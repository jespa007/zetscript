/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
#include "CZetScript.h"

#ifdef  __ZETSCRIPT_VERBOSE_MESSAGE__
#define print_com_cr zs_print_info_cr
#else
#define print_com_cr(s,...)
#endif

namespace zetscript{


	tDefOperator CCompiler::def_operator[MAX_OPERATORS];
	map<string, CCompiler::tInfoConstantValue *> * CCompiler::constant_pool=NULL;

	CCompiler *CCompiler::m_compiler = NULL;


	CCompiler::tInfoConstantValue *CCompiler::getConstant(const string & const_name){

		if((*constant_pool).count(const_name) == 1){
			return (*constant_pool)[const_name];
		}
		return NULL;
	}

	CCompiler::tInfoConstantValue * CCompiler::addConstant(const string & const_name, void *obj, unsigned short properties){

		CCompiler::tInfoConstantValue * info_ptr=NULL;

		if(getConstant(const_name) == NULL){
			info_ptr=new tInfoConstantValue;
			*info_ptr={properties,obj,NULL};
			(*constant_pool)[const_name]=info_ptr;
		}else{
			zs_print_error_cr("internal:constant %s already exist",const_name.c_str());
		}

		return info_ptr;
	}

	int CCompiler::addLocalVarSymbol(const string & var_name,short idxAstNode){

		int idxVar;


		if((idxVar=getIdxLocalVarSymbol(var_name,idxAstNode,false))==ZS_UNDEFINED_IDX){
			tInfoVariableSymbol info_symbol;

			info_symbol.idxAstNode = idxAstNode;
			info_symbol.symbol_name = var_name;



			this->m_currentFunctionInfo->function_info_object->object_info.local_symbols.m_registeredVariable.push_back(info_symbol);

			// init stack variable ...
			if(this->m_currentFunctionInfo->function_info_object->object_info.symbol_info.idxScriptClass == IDX_CLASS_MAIN && this->m_currentFunctionInfo->function_info_object->object_info.idxScriptFunctionObject == 0){ // initialize new global var initialized on MainFuntion ...
				CURRENT_VM->iniStackVar(this->m_currentFunctionInfo->function_info_object->object_info.local_symbols.m_registeredVariable.size(),{STK_PROPERTY_TYPE_UNDEFINED,0,0});
			}


			return this->m_currentFunctionInfo->function_info_object->object_info.local_symbols.m_registeredVariable.size()-1;
		} // else already added so we refer the same var.
		return idxVar;
	}

	bool CCompiler::localVarSymbolExists(const string & name,short  idxAstNode){


		return getIdxLocalVarSymbol(name,idxAstNode, false) != ZS_UNDEFINED_IDX;
	}

	int  CCompiler::getIdxLocalVarSymbol(const string & name,short idxAstNode, bool print_msg){

		string  var_name = AST_NODE(idxAstNode)->symbol_value;

		for(unsigned i = 0; i < this->m_currentFunctionInfo->function_info_object->object_info.local_symbols.m_registeredVariable.size(); i++){
			if(this->m_currentFunctionInfo->function_info_object->object_info.local_symbols.m_registeredVariable[i].symbol_name == var_name ){
				return i;
			}
		}
		return ZS_UNDEFINED_IDX;
	}

	CScriptFunctionObject * CCompiler::addLocalFunctionSymbol(const string & name,short idxAstNode){

		string  function_name = name;

		if(!functionSymbolExists(name,idxAstNode)){

			PASTNode ast_node = AST_NODE(idxAstNode);

			// get n params size in order to find right symbol (rememeber the symbol syntax _pN_symbol)...
			PASTNode ast_args =AST_NODE(ast_node->children[0]);

			if(ast_args->node_type != NODE_TYPE::ARGS_DECL_NODE){
				ZS_WRITE_ERROR_MSG(GET_AST_FILENAME_LINE(idxAstNode),"expected args node");
				return NULL;
			}

			int n_params= ast_args->children.size();

			tScopeVar *irv = SCOPE_INFO_NODE(ast_node->idxScope)->getInfoRegisteredSymbol(function_name,n_params,false);
			if(irv != NULL){

				CScriptFunctionObject *info_symbol = CScriptFunctionObject::newScriptFunctionObject();

				info_symbol->object_info.symbol_info.idxAstNode = irv->idxAstNode;
				//info_symbol.object_info.symbol_info.idxScopeVar = irv->idxScopeVar;
				info_symbol->object_info.symbol_info.symbol_name = name;

				this->m_currentFunctionInfo->function_info_object->object_info.local_symbols.vec_idx_registeredFunction.push_back(info_symbol->object_info.idxScriptFunctionObject);

				return info_symbol;
			}
			else{
				ZS_WRITE_ERROR_MSG(GET_AST_FILENAME_LINE(idxAstNode),"No function symbol \"%s\" with %i args is defined!",function_name.c_str(), n_params);
			}

		}
		else{
			ZS_WRITE_ERROR_MSG(GET_AST_FILENAME_LINE(idxAstNode),"No function symbol \"%s\" is defined!",function_name.c_str());
		}
		return NULL;
	}

	bool CCompiler::functionSymbolExists(const string & name, short idxAstNode){

		unsigned int scope_type;
		return getIdxFunctionObject(name,idxAstNode,scope_type,false) != ZS_UNDEFINED_IDX;
	}

	int  CCompiler::getIdxFunctionObject(const string & name,short idxAstNode, unsigned int & scope_type, bool print_msg){

		PASTNode ast_node=AST_NODE(idxAstNode);
		PASTNode ast_args = AST_NODE(ast_node->children[0]);
		if(ast_args->node_type != NODE_TYPE::ARGS_DECL_NODE){
			ZS_WRITE_ERROR_MSG(GET_AST_FILENAME_LINE(idxAstNode),"expected args node");
			return ZS_UNDEFINED_IDX;
		}

		unsigned n_args = ast_args->children.size();

		tScopeVar *irv=SCOPE_INFO_NODE(AST_NODE(idxAstNode)->idxScope)->getInfoRegisteredSymbol(name,(int)n_args,false);
		scope_type = INS_PROPERTY_LOCAL_SCOPE;
		if(irv != NULL){

			PASTNode ast = AST_NODE(irv->idxAstNode);

			if((ast != NULL) && (AST_NODE(idxAstNode)->idxScope == ast->idxScope)){
				for(unsigned i = 0; i < this->m_currentFunctionInfo->function_info_object->object_info.local_symbols.vec_idx_registeredFunction.size(); i++){
					CScriptFunctionObject *sfo=GET_SCRIPT_FUNCTION_OBJECT(m_currentFunctionInfo->function_info_object->object_info.local_symbols.vec_idx_registeredFunction[i]);
					if(sfo->object_info.symbol_info.symbol_name == name  && sfo->m_arg.size()==n_args){
						return i;
					}
				}
			}
			else{ //global

				scope_type = 0;//INST_PROPERTY_GLOBAL_SCOPE;
				CScriptFunctionObject *main_function = GET_SCRIPT_FUNCTION_OBJECT(GET_MAIN_SCRIPT_FUNCTION_IDX);


				for(unsigned i = 0; i < main_function->object_info.local_symbols.vec_idx_registeredFunction.size(); i++){
					CScriptFunctionObject *sfo=GET_SCRIPT_FUNCTION_OBJECT(main_function->object_info.local_symbols.vec_idx_registeredFunction[i]);
					if(sfo->object_info.symbol_info.symbol_name == name  && sfo->m_arg.size()==n_args ){
						return i;
					}
				}
			}
		}
		else{


		}
		return ZS_UNDEFINED_IDX;
	}

	CCompiler *CCompiler::getInstance(){
		if(m_compiler == NULL){
			constant_pool = new map<string,tInfoConstantValue *>;
			m_compiler = new CCompiler();
		}

		return m_compiler;
	}

	void CCompiler::destroySingletons(){
		if(m_compiler != NULL){


			for(map<string, CCompiler::tInfoConstantValue *>::iterator it=constant_pool->begin();it!=constant_pool->end();it++){
				tInfoConstantValue *icv=it->second;
				switch(GET_INS_PROPERTY_VAR_TYPE(icv->properties)){
				default:
					break;
				case STK_PROPERTY_TYPE_INTEGER:
					//delete (int *)icv->varRef;
					break;
				case STK_PROPERTY_TYPE_BOOLEAN:
					//delete (bool *)icv->stkValue;
					break;
				case STK_PROPERTY_TYPE_NUMBER:
					//delete (float *)icv->stkValue;
					break;
				case STK_PROPERTY_TYPE_STRING:
					delete (CStringScriptVariable *)icv->varRef;
					break;

				}

				delete icv;
			}

			constant_pool->clear();

			delete constant_pool;
			constant_pool = NULL;
			delete m_compiler;
			m_compiler=NULL;
		}
	}
	//------------------------------------------------------------------------------------------------------------------
	//
	// CONSTRUCTOR
	//

	CCompiler::CCompiler(){
		//m_currentListStatements = NULL;
		m_currentFunctionInfo = NULL;
		m_treescope = NULL;

		//		VAR  			|	STR   | ID | NUM OP
		//----------------------+---------+----+-------

		def_operator[EQU]         ={"EQU" ,EQU ,2};  // ==
		def_operator[INSTANCEOF]  ={"INSTANCEOF" ,INSTANCEOF ,2};  // ==
		def_operator[NOT_EQU]     ={"NOT_EQU" ,NOT_EQU ,2};  // !=
		def_operator[LT]          ={"LT"  ,LT ,2};  // <
		def_operator[LTE]         ={"LTE" ,LTE ,2};  // <=
		def_operator[NOT]         ={"NOT" ,NOT,1}; // !
		def_operator[GT]          ={"GT"  ,GT,2};  // >
		def_operator[GTE]         ={"GTE" ,GTE,2}; // >=
		def_operator[NEG]         ={"NEG",NEG,1}; // !
		def_operator[ADD]         ={"ADD" ,ADD,2}; // +
		def_operator[LOGIC_AND]   ={"LOGIC_AND",LOGIC_AND,2}; // &&
		def_operator[LOGIC_OR]    ={"LOGIC_OR",LOGIC_OR,2};  // ||
		def_operator[DIV]         ={"DIV",DIV,2}; // /
		def_operator[MUL]         ={"MUL",MUL,2}; // *
		def_operator[MOD]         ={"MOD",MOD,2};  // %

		def_operator[AND]         ={"AND",AND,2}; // bitwise logic and
		def_operator[OR]          ={"OR",OR,2}; // bitwise logic or
		def_operator[XOR]         ={"XOR",XOR,2}; // logic xor
		def_operator[SHL]         ={"SHL",SHL,2}; // shift left
		def_operator[SHR]         ={"SHR",SHR,2}; // shift right

		def_operator[STORE]         ={"STORE" ,STORE ,2}; // mov expression to var
		def_operator[LOAD]        ={"LOAD",LOAD ,1}; // primitive value like number/string or boolean...

		def_operator[JMP]         ={"JMP",JMP,2}; // Unconditional jump.
		def_operator[JNT]         ={"JNT",JNT,2}; // goto if not true ... goes end to conditional.
		def_operator[JT]          ={"JT",JT,2}; // goto if true ... goes end to conditional.

		def_operator[CALL]={"CALL",CALL,1}; // calling function after all of arguments are processed...
		def_operator[PUSH]={"PUSH",PUSH,1};
		//def_operator[START_ARG]={"START_ARG",START_ARG,0};
		def_operator[VGET]={"VGET",VGET,1}; // vector access after each index is processed...

		def_operator[DECL_VEC]={"DECL_VEC",DECL_VEC,1}; // Vector object (CREATE)

		def_operator[VPUSH]={"VPUSH",VPUSH,1}; // Value push for vector
		def_operator[VPOP]={"VPOP",VPOP,1}; // Value pop for vector
		def_operator[RET]={"RET",RET,1}; // Value pop for vector

		def_operator[NEW]={"NEW",NEW,1}; // New object (CREATE)
		def_operator[DELETE_OP]={"DELETE_OP",DELETE_OP,1};
		def_operator[OBJECT_ACCESS]={"OBJECT_ACCESS",OBJECT_ACCESS,2}; // New object (CREATE)
		//def_operator[SAVE_I]={"SAVE_I",SAVE_I,0}; // New object (CREATE)
		//def_operator[LOAD_I]={"LOAD_I",LOAD_I,0}; // New object (CREATE)
		def_operator[POP_SCOPE]={"POP_SCOPE",POP_SCOPE,0}; // New object (CREATE)
		def_operator[PUSH_SCOPE]={"PUSH_SCOPE",PUSH_SCOPE,1}; // New object (CREATE)
		def_operator[PUSH_ATTR]={"PUSH_ATTR",PUSH_ATTR,2}; // New object (CREATE)
		def_operator[DECL_STRUCT]={"DECL_STRUCT",DECL_STRUCT,0}; // New object (CREATE)
	}

	//------------------------------------------------------------------------------------------------------------------
	//
	// COMPILE COMPILER MANAGEMENT
	//
	int CCompiler::getCurrentInstructionIndex(){
		tInfoStatementOpCompiler *ptr_current_statement_op = &(m_currentFunctionInfo->stament)[m_currentFunctionInfo->stament.size()-1];
		return ptr_current_statement_op->asm_op.size()-1;
	}

	int CCompiler::getCurrentStatmentIndex(){
		return (int)(m_currentFunctionInfo->stament.size()-1);
	}

	CCompiler::tInfoStatementOpCompiler * CCompiler::newStatment(){
		tInfoStatementOpCompiler st;

		m_currentFunctionInfo->stament.push_back(st);

		return  &(m_currentFunctionInfo->stament)[m_currentFunctionInfo->stament.size()-1];
	}

	bool CCompiler::isFunctionNode(short idxAstNode){

		PASTNode node = AST_NODE(idxAstNode);

		if(node==NULL){
			zs_print_error_cr("Node is NULL!");
			return false;
		}

		return node->node_type == FUNCTION_OBJECT_NODE || (node->node_type==FUNCTION_REF_NODE);
	}

	int CCompiler::getIdxArgument(const string & var){


		// search if symbol belongs to arg vector...
		for(unsigned i = 0; i < this->m_currentFunctionInfo->function_info_object->m_arg.size(); i++){

			if(this->m_currentFunctionInfo->function_info_object->m_arg[i].arg_name == var){
				return i;
			}
		}
		return ZS_UNDEFINED_IDX;
	}

	bool checkAccessObjectMember(short idxAstNode){
		PASTNode _node = AST_NODE(idxAstNode);
		bool node_access=false;
		if(_node == NULL) return false;

		PASTNode check_node = AST_NODE(_node->idxAstParent);
		short child_compare=_node->idxAstNode;


		if(check_node != NULL){
			if(check_node->node_type==NODE_TYPE::CALLING_OBJECT_NODE) {// function / array access.
				child_compare=check_node->idxAstNode;
				check_node=AST_NODE(check_node->idxAstParent);
			}

			if(check_node != NULL){
				node_access =check_node->operator_info==PUNCTUATOR_TYPE::FIELD_PUNCTUATOR; // trivial case...
			}

			if(node_access){
				// check first ...
				if(check_node->idxAstParent != ZS_UNDEFINED_IDX){
					if(!checkAccessObjectMember(check_node->idxAstParent)){

						// if c.b is trivial but it has c.b.a must check that parent it has another punctuator.
						node_access = (check_node->children[0] != child_compare)
								   || (AST_NODE(check_node->idxAstParent)->operator_info==PUNCTUATOR_TYPE::FIELD_PUNCTUATOR); // parent not access left is first...
					}
				}
			}
		}
		return node_access;
	}

	void CCompiler::insertStringConstantValueInstruction(short idxAstNode, const string & v){

		tInfoStatementOpCompiler *ptr_current_statement_op = &(this->m_currentFunctionInfo->stament)[this->m_currentFunctionInfo->stament.size()-1];

		unsigned int type=STK_PROPERTY_TYPE_STRING;
		CCompiler::tInfoConstantValue *get_obj;
		void *obj;

		if((get_obj = getConstant(v))!=NULL){
			obj = get_obj;
		}else{

			CStringScriptVariable *s=new CStringScriptVariable(v);
			obj=addConstant(v,NULL,type);

			// set ptrs...
			((tStackElement *)obj)->stkValue=(void *)(s->m_strValue.c_str());
			((tStackElement *)obj)->varRef=s;
		}

		tInfoAsmOpCompiler *asm_op = new tInfoAsmOpCompiler();

		asm_op->var_type=type;
		asm_op->index_op1=LOAD_TYPE_CONSTANT;
		asm_op->index_op2=(intptr_t)obj;
		asm_op->idxAstNode=idxAstNode;
		asm_op->operator_type=ASM_OPERATOR::LOAD;
		ptr_current_statement_op->asm_op.push_back(asm_op);
	}

	bool CCompiler::insertLoadValueInstruction(short idxAstNode, CScope * _lc, tInfoAsmOpCompiler **iao_result){
		PASTNode _node=AST_NODE(idxAstNode);
		PASTNode _parent=AST_NODE(_node->idxAstParent);
		string v = _node->symbol_value;

		// ignore node this ...
		if(_node->symbol_value == "this"){
			ZS_WRITE_ERROR_MSG(GET_AST_FILENAME_LINE(idxAstNode),"\"%s\" cannot be processed here!",_node->symbol_value.c_str());
			return false;
		}


		unsigned int pre_post_operator_type =0;//INS_PROPERTY_UNKNOW_PRE_POST_OPERATOR;
		tInfoStatementOpCompiler *ptr_current_statement_op = &(this->m_currentFunctionInfo->stament)[this->m_currentFunctionInfo->stament.size()-1];
		void *const_obj;
		void *obj;
		CCompiler::tInfoConstantValue *get_obj;
		unsigned short type=STK_PROPERTY_TYPE_SCRIPTVAR;
		LOAD_TYPE load_type=LOAD_TYPE_NOT_DEFINED;
		unsigned int scope_type=0;//INS_PROPERTY_UNKNOWN_SCOPE;
		bool is_constant = true;

		if((_parent->operator_info == INSTANCEOF_PUNCTUATOR) && (_parent->children[1] == idxAstNode)){ // we search for class idx

			intptr_t classidx=-1;


			if(v=="bool"){
				classidx=IDX_CLASS_BOOL_PTR_C;
			}else if(v== "int"){
				classidx=IDX_CLASS_INT_PTR_C;
			}else if(v=="number"){
					classidx=IDX_CLASS_FLOAT_PTR_C;
			}else if(v=="string"){
				classidx=IDX_CLASS_STRING_PTR_C;
			}else if(v=="vector"){
				classidx=IDX_CLASS_VECTOR;
			}else if(v=="struct"){
				classidx=IDX_CLASS_STRUCT;
			}else if(v=="function"){
				classidx=IDX_CLASS_FUNCTOR;
			}else{ // search idx...
				if((classidx=CScriptClass::getIdxScriptClass(v,false))==-1){
					ZS_WRITE_ERROR_MSG(GET_AST_FILENAME_LINE(idxAstNode)," in instenceof operator  class \"%s\" is not registered",v.c_str());
					return false;
				}
			}

			type=STK_PROPERTY_TYPE_INTEGER;
			load_type=LOAD_TYPE_CONSTANT;
			print_com_cr("%s detected as idx class (%i)\n",v.c_str(),classidx);
			if((get_obj = getConstant(v))!=NULL){ // if not exist ... insert!
				obj = get_obj;
			}else{
				obj=addConstant(v,(void *)classidx,type);
			}

		}else{

			pre_post_operator_type=post_operator2instruction_property(_node->pre_post_operator_info);

			// try parse value...
			if(v=="null"){
				type=STK_PROPERTY_TYPE_NULL;
				load_type=LOAD_TYPE_NULL;
				obj=NULL;//CScriptVariable::NullSymbol;
				print_com_cr("%s detected as null\n",v.c_str());
			}else if(v=="undefined"){
					type=STK_PROPERTY_TYPE_UNDEFINED;
					load_type=LOAD_TYPE_UNDEFINED;
					obj=NULL;// CScriptVariable::UndefinedSymbol;
					print_com_cr("%s detected as undefined\n",v.c_str());

			}else if((const_obj=CStringUtils::ParseInteger(v))!=NULL){
				intptr_t value = *((int *)const_obj);

				delete (int *)const_obj;

				type=STK_PROPERTY_TYPE_INTEGER;
				load_type=LOAD_TYPE_CONSTANT;
				print_com_cr("%s detected as int\n",v.c_str());
				if((get_obj = getConstant(v))!=NULL){
					obj = get_obj;
				}else{
					obj=addConstant(v,(void *)value,type);
				}
			}
			else if((const_obj=CStringUtils::ParseFloat(v))!=NULL){
				float value = *((float *)const_obj);

				delete (float *)const_obj;
				void *value_ptr;

				memcpy(&value_ptr,&value,sizeof(float));


				type=STK_PROPERTY_TYPE_NUMBER;
				load_type=LOAD_TYPE_CONSTANT;
				print_com_cr("%s detected as float\n",v.c_str());

				if((get_obj = getConstant(v))!=NULL){
					obj = get_obj;
				}else{
					obj=addConstant(v,value_ptr,type);
				}
			}
			else if((v[0]=='\"' && v[v.size()-1]=='\"')){
				type=STK_PROPERTY_TYPE_STRING;
				load_type=LOAD_TYPE_CONSTANT;
				print_com_cr("%s detected as string\n",v.c_str());

				if((get_obj = getConstant(v))!=NULL){
					obj = get_obj;
				}else{

					CStringScriptVariable *s=new CStringScriptVariable(v.substr(1,v.size()-2));
					obj=addConstant(v,NULL,type);
					((tStackElement *)obj)->stkValue=((void *)(s->m_strValue.c_str()));
					((tStackElement *)obj)->varRef=s;

				}
			}
			else if((const_obj=CStringUtils::ParseBoolean(v))!=NULL){

				bool value = *((bool *)const_obj);
				delete (bool *)const_obj;

				type=STK_PROPERTY_TYPE_BOOLEAN;
				load_type=LOAD_TYPE_CONSTANT;
				print_com_cr("%s detected as boolean\n",v.c_str());

				if((get_obj = getConstant(v))!=NULL){
					obj = get_obj;
				}else{
					obj=addConstant(v,(void *)value,type);
				}
			}else{

				is_constant = false;
				string symbol_name = _node->symbol_value;


				intptr_t idx_local_var=ZS_UNDEFINED_IDX;
				bool access;

				if(    (access=checkAccessObjectMember(_node->idxAstNode))
					|| _node->node_type==NODE_TYPE::NEW_OBJECT_NODE) // we wants to call the constructor
				{
					scope_type = INS_PROPERTY_ACCESS_SCOPE;
					PASTNode parent = AST_NODE(_node->idxAstParent);
					PASTNode old_parent=_node;
					PASTNode left=NULL;

					/* we have three types of access..

						   .               .              .
						  / \             / \            / \
						 /   \           /   \          /   \
						this  var      this   ·       this  calling objec
											 / \
											/   \
										  var1  var2
					*/
					if(access) {
						while(parent->node_type!=NODE_TYPE::PUNCTUATOR_NODE) {// function / array access.
								old_parent = parent;
								parent=AST_NODE(parent->idxAstParent);
						}
					}

					if(parent!=NULL && parent->node_type == NODE_TYPE::PUNCTUATOR_NODE){ //|| // single access ?

						 left=AST_NODE(parent->children[0]);

						 if(left->symbol_value == "this"){ // 1st check
							 scope_type=INS_PROPERTY_THIS_SCOPE;
						 }else{ // 2nd check (must be at the left)

							if(old_parent->idxAstNode==parent->children[0]){// must be in the left...

								 parent = AST_NODE(parent->idxAstParent);
								left=AST_NODE(parent->children[0]);

								 if(parent != NULL && parent->node_type == NODE_TYPE::PUNCTUATOR_NODE){

									 left=AST_NODE(parent->children[0]);

									 if(left->symbol_value == "this"){
										 scope_type=INS_PROPERTY_THIS_SCOPE;
									 }
								 }
							 }
						 }
					}
				}
				else if(
					(_node->symbol_value == "super")
					){
						scope_type=INS_PROPERTY_SUPER_SCOPE;
					}else{

						// if not function then is var or arg node ?
						// first we find the list of argments
						if((idx_local_var = getIdxArgument(_node->symbol_value))!=ZS_UNDEFINED_IDX){
							load_type=LOAD_TYPE_ARGUMENT;
						}
						else{ // ... if not argument finally, we deduce that the value is a local/global symbol... check whether it exist in the current scope...

								if(_node->node_type == SYMBOL_NODE){
									if(_lc!=NULL){
										if(!_lc->existRegisteredSymbol(symbol_name)){ // check local
											if(!SCOPE_INFO_NODE(0)->existRegisteredSymbol(symbol_name)){ // check global
												ZS_WRITE_ERROR_MSG(GET_AST_FILENAME_LINE(_node->idxAstNode)," variable \"%s\" not defined",symbol_name.c_str());
												return false;
											}
										}
									}
									else{
										zs_print_error_cr("scope null");
										return false;
									}
								}
						}
					}
					obj = (CScriptVariable *)idx_local_var;
			}

			if((pre_post_operator_type !=0 && GET_INS_PROPERTY_PRE_POST_OP(pre_post_operator_type) !=INS_PROPERTY_PRE_NEG) &&
				is_constant){
				ZS_WRITE_ERROR_MSG(GET_AST_FILENAME_LINE(_node->idxAstNode),"operation \"%s\" not allowed for constants ",CASTNode::defined_operator_punctuator[_node->pre_post_operator_info].str);
				return false;
			}
		}

		tInfoAsmOpCompiler *asm_op = new tInfoAsmOpCompiler();

		//asm_op->variable_type=type;
		asm_op->index_op1=load_type;
		asm_op->index_op2=(intptr_t)obj;
		//asm_op->scope_type = scope_type;
		asm_op->idxAstNode=_node->idxAstNode;
		asm_op->var_type=type;
		asm_op->pre_post_op_type=pre_post_operator_type;
		asm_op->scope_type=scope_type;

		asm_op->operator_type=ASM_OPERATOR::LOAD;
		ptr_current_statement_op->asm_op.push_back(asm_op);

		if(iao_result != NULL){
			*iao_result=asm_op;
		}

		return true;
	}

	bool CCompiler::insertMovVarInstruction(short idxAstNode,int left_index, int right_index){

		tInfoStatementOpCompiler *ptr_current_statement_op = &(this->m_currentFunctionInfo->stament)[this->m_currentFunctionInfo->stament.size()-1];
		tInfoAsmOpCompiler * left_asm_op = ptr_current_statement_op->asm_op[left_index];

		// check whether left operant is object...
		if(left_asm_op->var_type != STK_PROPERTY_TYPE_SCRIPTVAR){
			//int line = -1;
			//if(left_asm_op->idxAstNode!=ZS_UNDEFINED_IDX)
			//	line=AST_LINE(left_asm_op->idxAstNode);
			ZS_WRITE_ERROR_MSG(GET_AST_FILENAME_LINE(left_asm_op->idxAstNode)," left operand must be l-value for '=' operator");
			return false;
		}

		tInfoAsmOpCompiler *asm_op = new tInfoAsmOpCompiler();
		asm_op->index_op1 = ZS_UNDEFINED_IDX;//&(this->m_currentFunctionInfo->stament[dest_statment]);
		asm_op->index_op2 =  ZS_UNDEFINED_IDX;
		asm_op->idxAstNode = idxAstNode;
		//asm_op->symbol_name="";
		asm_op->operator_type=ASM_OPERATOR::STORE;

		ptr_current_statement_op->asm_op.push_back(asm_op);
		return true;
	}

	void CCompiler::insertNot(short idxAstNode){
		tInfoStatementOpCompiler *ptr_current_statement_op = &this->m_currentFunctionInfo->stament[this->m_currentFunctionInfo->stament.size()-1];
		tInfoAsmOpCompiler *asm_op = new tInfoAsmOpCompiler();
		asm_op->idxAstNode = idxAstNode;
		asm_op->operator_type=ASM_OPERATOR::NOT;
		ptr_current_statement_op->asm_op.push_back(asm_op);
	}

	void CCompiler::insertNeg(short idxAstNode){
		tInfoStatementOpCompiler *ptr_current_statement_op = &this->m_currentFunctionInfo->stament[this->m_currentFunctionInfo->stament.size()-1];
		tInfoAsmOpCompiler *asm_op = new tInfoAsmOpCompiler();
		asm_op->idxAstNode = idxAstNode;
		asm_op->operator_type=ASM_OPERATOR::NEG;
		ptr_current_statement_op->asm_op.push_back(asm_op);
	}

	CCompiler::tInfoAsmOpCompiler * CCompiler::insert_JMP_Instruction(short idxAstNode,int jmp_statement, int instruction_index){

		tInfoStatementOpCompiler *ptr_current_statement_op = &this->m_currentFunctionInfo->stament[this->m_currentFunctionInfo->stament.size()-1];
		tInfoAsmOpCompiler *asm_op = new tInfoAsmOpCompiler();
		asm_op->index_op1 = instruction_index;
		asm_op->index_op2 = jmp_statement;//&(this->m_currentFunctionInfo->stament[dest_statment]);
		asm_op->operator_type=ASM_OPERATOR::JMP;
		asm_op->idxAstNode = idxAstNode;
		ptr_current_statement_op->asm_op.push_back(asm_op);
		return asm_op;
	}

	CCompiler::tInfoAsmOpCompiler * CCompiler::insert_JNT_Instruction(short idxAstNode,int jmp_statement, int instruction_index){

		tInfoStatementOpCompiler *ptr_current_statement_op = &this->m_currentFunctionInfo->stament[this->m_currentFunctionInfo->stament.size()-1];
		tInfoAsmOpCompiler *asm_op = new tInfoAsmOpCompiler();
		asm_op->index_op1 = instruction_index;
		asm_op->index_op2 = jmp_statement;//&(this->m_currentFunctionInfo->stament[dest_statment]);
		asm_op->operator_type=ASM_OPERATOR::JNT;
		asm_op->idxAstNode = idxAstNode;
		ptr_current_statement_op->asm_op.push_back(asm_op);

		return asm_op;
	}

	CCompiler::tInfoAsmOpCompiler * CCompiler::insert_JT_Instruction(short idxAstNode,int jmp_statement, int instruction_index){

		tInfoStatementOpCompiler *ptr_current_statement_op = &this->m_currentFunctionInfo->stament[this->m_currentFunctionInfo->stament.size()-1];
		tInfoAsmOpCompiler *asm_op = new tInfoAsmOpCompiler();
		asm_op->operator_type=ASM_OPERATOR::JT;
		asm_op->index_op1 = instruction_index;
		asm_op->index_op2 = jmp_statement;
		asm_op->idxAstNode = idxAstNode;
		ptr_current_statement_op->asm_op.push_back(asm_op);
		return asm_op;
	}

	void CCompiler::insert_CreateArrayObject_Instruction(short idxAstNode){
		tInfoStatementOpCompiler *ptr_current_statement_op = &this->m_currentFunctionInfo->stament[this->m_currentFunctionInfo->stament.size()-1];
		tInfoAsmOpCompiler *asm_op = new tInfoAsmOpCompiler();

		asm_op->operator_type=ASM_OPERATOR::DECL_VEC;
		asm_op->var_type=STK_PROPERTY_TYPE_SCRIPTVAR;
		asm_op->idxAstNode = idxAstNode;
		ptr_current_statement_op->asm_op.push_back(asm_op);
	}

	void CCompiler::insert_ArrayAccess_Instruction(int vec_object, int index_instrucction, short idxAstNode){
		tInfoStatementOpCompiler *ptr_current_statement_op = &this->m_currentFunctionInfo->stament[this->m_currentFunctionInfo->stament.size()-1];
		tInfoAsmOpCompiler *asm_op = new tInfoAsmOpCompiler();
		asm_op->index_op1 = vec_object;//&(this->m_currentFunctionInfo->stament[dest_statment]);
		asm_op->index_op2 = index_instrucction;//&(this->m_currentFunctionInfo->stament[dest_statment]);
		asm_op->operator_type=ASM_OPERATOR::VGET;
		asm_op->idxAstNode = idxAstNode;
		asm_op->var_type = STK_PROPERTY_TYPE_SCRIPTVAR;
		ptr_current_statement_op->asm_op.push_back(asm_op);
	}

	void CCompiler::insert_CallFunction_Instruction(short idxAstNode,int  index_call,int  index_object){
		tInfoStatementOpCompiler *ptr_current_statement_op = &this->m_currentFunctionInfo->stament[this->m_currentFunctionInfo->stament.size()-1];
		tInfoAsmOpCompiler *asm_op = new tInfoAsmOpCompiler();

		asm_op->index_op1 = index_call;//&(this->m_currentFunctionInfo->stament[dest_statment]);
		asm_op->index_op2 = index_object;//&(this->m_currentFunctionInfo->stament[dest_statment]);

		asm_op->idxAstNode = idxAstNode;
		asm_op->operator_type=ASM_OPERATOR::CALL;

		ptr_current_statement_op->asm_op.push_back(asm_op);

	}

	void CCompiler::insertRet(short idxAstNode,int index){

		tInfoStatementOpCompiler *ptr_current_statement_op = &this->m_currentFunctionInfo->stament[this->m_currentFunctionInfo->stament.size()-1];
		tInfoAsmOpCompiler *asm_op = new tInfoAsmOpCompiler();

		// if type return is object type, then return first index...
		if(ptr_current_statement_op->asm_op[0]->operator_type == ASM_OPERATOR::DECL_VEC ||
		   ptr_current_statement_op->asm_op[0]->operator_type == ASM_OPERATOR::NEW ||
		   ptr_current_statement_op->asm_op[0]->operator_type == ASM_OPERATOR::DECL_STRUCT
		   ){
			asm_op->index_op1 = 0; // we need the object
		}
		else{ // else return expression result index
			asm_op->index_op1 = index;//&(this->m_currentFunctionInfo->stament[dest_statment]);

			if(ptr_current_statement_op->asm_op.size() > 1){
				tInfoAsmOpCompiler *last_asm = ptr_current_statement_op->asm_op[ptr_current_statement_op->asm_op.size()-1]; // get last operator

				if((last_asm->operator_type == ASM_OPERATOR::CALL) && ((unsigned)index == ptr_current_statement_op->asm_op.size()-1)){
					last_asm->runtime_prop |=INS_PROPERTY_DIRECT_CALL_RETURN;
				}
			}
		}

		asm_op->operator_type=ASM_OPERATOR::RET;
		asm_op->idxAstNode = idxAstNode;

		ptr_current_statement_op->asm_op.push_back(asm_op);
	}

	void CCompiler::insert_ArrayObject_PushValueInstruction(short idxAstNode,int ref_vec_object_index,int index_instruciont_to_push){
		tInfoStatementOpCompiler *ptr_current_statement_op = &this->m_currentFunctionInfo->stament[this->m_currentFunctionInfo->stament.size()-1];
		tInfoAsmOpCompiler *asm_op = new tInfoAsmOpCompiler();
		asm_op->index_op1=ref_vec_object_index;
		asm_op->idxAstNode = idxAstNode;
		asm_op->index_op2=index_instruciont_to_push;
		if(index_instruciont_to_push == ZS_UNDEFINED_IDX){
			asm_op->index_op2=CCompiler::getCurrentInstructionIndex();
		}
		asm_op->operator_type=ASM_OPERATOR::VPUSH;
		ptr_current_statement_op->asm_op.push_back(asm_op);
	}

	//--------------------------------------------------------------------------------------------------------------------------------------------------------------------

	bool CCompiler::insert_NewObject_Instruction(short idxAstNode, const string & class_name){
		tInfoStatementOpCompiler *ptr_current_statement_op = &this->m_currentFunctionInfo->stament[this->m_currentFunctionInfo->stament.size()-1];
		tInfoAsmOpCompiler *asm_op = new tInfoAsmOpCompiler();
		if((asm_op->index_op1 = CScriptClass::getIdxScriptClass(class_name))==ZS_UNDEFINED_IDX){//&(this->m_currentFunctionInfo->stament[dest_statment]);
			ZS_WRITE_ERROR_MSG(GET_AST_FILENAME_LINE(idxAstNode),"class \"%s\" is not registered", class_name.c_str());
			return false;
		}
		asm_op->operator_type=ASM_OPERATOR::NEW;
		asm_op->idxAstNode = idxAstNode;

		ptr_current_statement_op->asm_op.push_back(asm_op);
		return true;
	}

	bool CCompiler::insert_DeleteObject_Instruction(short idxAstNode){
		tInfoStatementOpCompiler *ptr_current_statement_op = &this->m_currentFunctionInfo->stament[this->m_currentFunctionInfo->stament.size()-1];
		tInfoAsmOpCompiler *asm_op = new tInfoAsmOpCompiler();

		asm_op->operator_type=ASM_OPERATOR::DELETE_OP;
		asm_op->idxAstNode = idxAstNode;
		ptr_current_statement_op->asm_op.push_back(asm_op);

		return true;
	}

	bool CCompiler::insertObjectMemberAccessFrom(short idxAstNode, int ref_node_index){
		tInfoStatementOpCompiler *ptr_current_statement_op = &this->m_currentFunctionInfo->stament[this->m_currentFunctionInfo->stament.size()-1];
		tInfoAsmOpCompiler *asm_op = new tInfoAsmOpCompiler();
		asm_op->index_op1 = ref_node_index;
		asm_op->index_op2 = ZS_UNDEFINED_IDX; // index from object cached node ?
		asm_op->operator_type=ASM_OPERATOR::OBJECT_ACCESS;
		asm_op->idxAstNode = idxAstNode;

		ptr_current_statement_op->asm_op.push_back(asm_op);
		return true;
	}

	bool CCompiler::insertPushScopeInstruction(short idxAstNode,int scope_idx){
		if(scope_idx==ZS_UNDEFINED_IDX){
			zs_print_error_cr("Internal error undefined scope!");
			return false;
		}

		tInfoStatementOpCompiler *ptr_current_statement_op = &this->m_currentFunctionInfo->stament[this->m_currentFunctionInfo->stament.size()-1];
		tInfoAsmOpCompiler *asm_op = new tInfoAsmOpCompiler();

		asm_op->index_op1 = ZS_UNDEFINED_IDX;
		asm_op->index_op2 = scope_idx; // index from object cached node ?
		asm_op->operator_type=ASM_OPERATOR::PUSH_SCOPE;
		asm_op->idxAstNode = idxAstNode;

		ptr_current_statement_op->asm_op.push_back(asm_op);

		return true;
	}

	void CCompiler::insertPopScopeInstruction(short idxAstNode){
		tInfoStatementOpCompiler *ptr_current_statement_op = &this->m_currentFunctionInfo->stament[this->m_currentFunctionInfo->stament.size()-1];
		tInfoAsmOpCompiler *asm_op = new tInfoAsmOpCompiler();
		asm_op->index_op1 = ZS_UNDEFINED_IDX;
		asm_op->index_op2 = ZS_UNDEFINED_IDX; // index from object cached node ?
		asm_op->operator_type=ASM_OPERATOR::POP_SCOPE;
		asm_op->idxAstNode = idxAstNode;

		ptr_current_statement_op->asm_op.push_back(asm_op);
	}

	void CCompiler::insert_DeclStruct_Instruction(short idxAstNode){
		tInfoStatementOpCompiler *ptr_current_statement_op = &this->m_currentFunctionInfo->stament[this->m_currentFunctionInfo->stament.size()-1];
		tInfoAsmOpCompiler *asm_op = new tInfoAsmOpCompiler();
		asm_op->index_op1 = ZS_UNDEFINED_IDX;
		asm_op->index_op2 = ZS_UNDEFINED_IDX; // index from object cached node ?
		asm_op->operator_type=ASM_OPERATOR::DECL_STRUCT;
		asm_op->idxAstNode = idxAstNode;

		ptr_current_statement_op->asm_op.push_back(asm_op);
	}

	void CCompiler::insert_PushAttribute_Instruction(short idxAstNode,int ref_object,int ref_result_expression){
		tInfoStatementOpCompiler *ptr_current_statement_op = &this->m_currentFunctionInfo->stament[this->m_currentFunctionInfo->stament.size()-1];
		tInfoAsmOpCompiler *asm_op = new tInfoAsmOpCompiler();
		asm_op->index_op1 = ref_object; // struct ref
		asm_op->index_op2 = ref_result_expression; // ref result expression
		asm_op->operator_type=ASM_OPERATOR::PUSH_ATTR;
		asm_op->idxAstNode = idxAstNode;
		//asm_op->aux_name = attr_name;

		ptr_current_statement_op->asm_op.push_back(asm_op);
	}

	//--------------------------------------------------------------------------------------------------------------------------------------------------------------------
	unsigned int CCompiler::post_operator2instruction_property(PUNCTUATOR_TYPE op){

		switch(op){
		default:

			break;
		case PUNCTUATOR_TYPE::POST_INC_PUNCTUATOR:
			return INS_PROPERTY_POST_INC;
		case PUNCTUATOR_TYPE::POST_DEC_PUNCTUATOR:
			return INS_PROPERTY_POST_DEC;
		}
		return 0;//INS_PROPERTY_UNKNOW_PRE_POST_OPERATOR;
	}

	ASM_OPERATOR CCompiler::puntuator2instruction(PUNCTUATOR_TYPE op){

		switch(op){
		default:
			zs_print_error_cr("%s Not implemented", CASTNode::defined_operator_punctuator[op].str);
			break;
		case SUB_PUNCTUATOR:
			return ASM_OPERATOR::NEG;
		case ADD_PUNCTUATOR:
			return ASM_OPERATOR::ADD;
		case MUL_PUNCTUATOR:
			return ASM_OPERATOR::MUL;
		case DIV_PUNCTUATOR:
			return ASM_OPERATOR::DIV;
		case MOD_PUNCTUATOR:
			return ASM_OPERATOR::MOD;
		case ASSIGN_PUNCTUATOR:
			return ASM_OPERATOR::STORE;
		case BINARY_XOR_PUNCTUATOR:
			return ASM_OPERATOR::XOR;
		case BINARY_AND_PUNCTUATOR:
			return ASM_OPERATOR::AND;
		case BINARY_OR_PUNCTUATOR:
			return ASM_OPERATOR::OR;
		case SHIFT_LEFT_PUNCTUATOR:
			return ASM_OPERATOR::SHL;
		case SHIFT_RIGHT_PUNCTUATOR:
			return ASM_OPERATOR::SHR;
		case LOGIC_AND_PUNCTUATOR:
			return ASM_OPERATOR::LOGIC_AND;
		case LOGIC_OR_PUNCTUATOR:
			return ASM_OPERATOR::LOGIC_OR;
		case LOGIC_EQUAL_PUNCTUATOR:
			return ASM_OPERATOR::EQU;
		case LOGIC_NOT_EQUAL_PUNCTUATOR:
			return ASM_OPERATOR::NOT_EQU;
		case LOGIC_GT_PUNCTUATOR:
			return ASM_OPERATOR::GT;
		case LOGIC_LT_PUNCTUATOR:
			return ASM_OPERATOR::LT;
		case LOGIC_GTE_PUNCTUATOR:
			return ASM_OPERATOR::GTE;
		case LOGIC_LTE_PUNCTUATOR:
			return ASM_OPERATOR::LTE;
		case LOGIC_NOT_PUNCTUATOR:
			return ASM_OPERATOR::NOT;
		case FIELD_PUNCTUATOR:
			return ASM_OPERATOR::OBJECT_ACCESS;
		case INSTANCEOF_PUNCTUATOR:
			return ASM_OPERATOR::INSTANCEOF;


		}
		return INVALID_OP;
	}


	CCompiler::tInfoAsmOpCompiler * CCompiler::getLastInsertedInfoAsmOpCompiler(){
		tInfoStatementOpCompiler *ptr_current_statement_op = &this->m_currentFunctionInfo->stament[this->m_currentFunctionInfo->stament.size()-1];
		return ptr_current_statement_op->asm_op[ptr_current_statement_op->asm_op.size()-1];
	}


	bool CCompiler::insertOperatorInstruction(PUNCTUATOR_TYPE op,short idxAstNode,  string & error_str, int op_index_left, int op_index_right){
		PASTNode _node=AST_NODE( idxAstNode);

		tInfoStatementOpCompiler *ptr_current_statement_op = &this->m_currentFunctionInfo->stament[this->m_currentFunctionInfo->stament.size()-1];
		tInfoAsmOpCompiler * left_asm_op = ptr_current_statement_op->asm_op[op_index_left];
		tInfoAsmOpCompiler *iao=NULL;

		// special cases ...
		switch(op){

			case ADD_ASSIGN_PUNCTUATOR: // a+b

				iao = new tInfoAsmOpCompiler();
				iao->operator_type = ASM_OPERATOR::ADD;
				iao->runtime_prop |= STK_PROPERTY_READ_TWO_POP_ONE;
				iao->idxAstNode=_node->idxAstNode;
				ptr_current_statement_op->asm_op.push_back(iao);

				op_index_right+=1;
				op = ASSIGN_PUNCTUATOR;
				break;
			case SUB_ASSIGN_PUNCTUATOR: // a + (-b)

				iao = new tInfoAsmOpCompiler();
				iao->operator_type = ASM_OPERATOR::NEG;

				iao->idxAstNode=_node->idxAstNode;
				ptr_current_statement_op->asm_op.push_back(iao);

				iao = new tInfoAsmOpCompiler();
				iao->operator_type = ASM_OPERATOR::ADD;
				iao->runtime_prop |= STK_PROPERTY_READ_TWO_POP_ONE;
				iao->idxAstNode=_node->idxAstNode;
				ptr_current_statement_op->asm_op.push_back(iao);

				op_index_right+=2;
				op = ASSIGN_PUNCTUATOR;
				break;
			case MUL_ASSIGN_PUNCTUATOR: // a*b

				iao = new tInfoAsmOpCompiler();
				iao->operator_type = ASM_OPERATOR::MUL;
				iao->runtime_prop |= STK_PROPERTY_READ_TWO_POP_ONE;

				iao->idxAstNode=_node->idxAstNode;
				ptr_current_statement_op->asm_op.push_back(iao);

				op_index_right+=1;
				op = ASSIGN_PUNCTUATOR;
				break;
			case DIV_ASSIGN_PUNCTUATOR: // a/b

				iao = new tInfoAsmOpCompiler();
				iao->operator_type = ASM_OPERATOR::DIV;
				iao->runtime_prop |= STK_PROPERTY_READ_TWO_POP_ONE;

				iao->idxAstNode=_node->idxAstNode;
				ptr_current_statement_op->asm_op.push_back(iao);

				op_index_right+=1;
				op = ASSIGN_PUNCTUATOR;
				break;
			case MOD_ASSIGN_PUNCTUATOR: // a % b

				iao = new tInfoAsmOpCompiler();
				iao->operator_type = ASM_OPERATOR::MOD;
				iao->runtime_prop |= STK_PROPERTY_READ_TWO_POP_ONE;

				iao->idxAstNode=_node->idxAstNode;
				ptr_current_statement_op->asm_op.push_back(iao);

				op_index_right+=1;
				op = ASSIGN_PUNCTUATOR;
				break;
			default:
				break;

		}

		if(iao!=NULL){
			iao->index_op1 = ZS_UNDEFINED_IDX;
			iao->index_op2 = ZS_UNDEFINED_IDX;
		}

		if((op == ASSIGN_PUNCTUATOR) && (left_asm_op->var_type != STK_PROPERTY_TYPE_SCRIPTVAR)){

				error_str = "left operand must be l-value for '=' operator";
				return false;
		}

		if(op == PUNCTUATOR_TYPE::FIELD_PUNCTUATOR){ // trivial access...
			return true;
		}
		ASM_OPERATOR asm_op;
		if((asm_op= puntuator2instruction(op))!=INVALID_OP){
			iao = new tInfoAsmOpCompiler();
			iao->operator_type = asm_op;
			iao->index_op1 = ZS_UNDEFINED_IDX;
			iao->index_op2 = ZS_UNDEFINED_IDX;

			if(_node!=NULL){
				iao->idxAstNode=_node->idxAstNode;
			}
			ptr_current_statement_op->asm_op.push_back(iao);
			return true;
		}
		return false;
	}

	//------------------------------------------------------------------------------------------------------------------
	//
	// COMPILE EXPRESSIONS AND GENERATE ITS ASM
	//

	int CCompiler::gacExpression_FunctionOrArrayAccess(short idxAstNode, CScope *_lc){

		PASTNode _node_access=AST_NODE(idxAstNode);
		bool  function_access = false;
		bool array_access = false;

		PASTNode eval_node_sp = _node_access;
		if(_node_access->node_type == CALLING_OBJECT_NODE){
			if(_node_access->children.size() > 0){
				eval_node_sp = AST_NODE(_node_access->children[0]);

				function_access = eval_node_sp->node_type == FUNCTION_OBJECT_NODE || eval_node_sp->node_type == FUNCTION_REF_NODE;
				array_access = eval_node_sp->node_type == ARRAY_OBJECT_NODE || eval_node_sp->node_type == ARRAY_REF_NODE;


			}else {
				zs_print_error_cr("Calling object should have at least 1 children");
				return ZS_UNDEFINED_IDX;
			}
		}

		if(array_access){
			return gacExpression_ArrayAccess(_node_access->idxAstNode, _lc);
		}else if(function_access){
			return gacExpression_FunctionAccess(_node_access->idxAstNode, _lc);
		}

		zs_print_error_cr("Expected function or array access");
		return ZS_UNDEFINED_IDX;
	}

	int CCompiler::gacExpression_ArrayAccess(short idxAstNode, CScope *_lc){

		PASTNode _node = AST_NODE(idxAstNode);

		if(_node == NULL) {zs_print_error_cr("NULL node");return ZS_UNDEFINED_IDX;}
		if(_node->node_type != CALLING_OBJECT_NODE ){zs_print_error_cr("node is not CALLING_OBJECT_NODE type or null");return ZS_UNDEFINED_IDX;}
		if(!(_node->children.size()==2 || _node->children.size()==3)) {zs_print_error_cr("Array access should have 2 children");return ZS_UNDEFINED_IDX;}


		PASTNode node_0=AST_NODE(_node->children[0]),
				 node_1=AST_NODE(_node->children[1]),
				 node_2=NULL;

		if(_node->children.size()==3){
			node_2 = AST_NODE(_node->children[2]);
		}


		if(node_0->node_type != ARRAY_REF_NODE && node_0->node_type != ARRAY_OBJECT_NODE ){zs_print_error_cr("Node is not ARRAY_OBJECT type"); return ZS_UNDEFINED_IDX;}
		if(node_1->node_type != ARRAY_ACCESS_NODE || node_1->children.size() == 0){zs_print_error_cr("Array has no index nodes "); return ZS_UNDEFINED_IDX;}

		int vec=0;

		if(node_0->node_type == ARRAY_OBJECT_NODE){ // must first create the object ...
			if(!gacExpression_ArrayObject(node_0->idxAstNode,_lc)){
				return ZS_UNDEFINED_IDX;
			}
		}else{
			if(node_0->symbol_value != "--"){ // starts with symbol ...
				if(!insertLoadValueInstruction(node_0->idxAstNode,  _lc)){
					return ZS_UNDEFINED_IDX;
				}
			}
			vec=CCompiler::getCurrentInstructionIndex();
		}

		PASTNode array_acces = node_1;

		// get all indexes ...
		for(unsigned k = 0; k < array_acces->children.size(); k++){
			if(AST_NODE(array_acces->children [k])->node_type == ARRAY_INDEX_NODE){
				if(AST_NODE(array_acces->children [k])->children.size() == 1){
					// check whether is expression node...
					if(!(gacExpression(AST_NODE(array_acces->children [k])->children[0], _lc,getCurrentInstructionIndex()+1))){
						return ZS_UNDEFINED_IDX;
					}
					// insert vector access instruction ...
					insert_ArrayAccess_Instruction(vec,CCompiler::getCurrentInstructionIndex(), array_acces->children [k]);
					vec = getCurrentInstructionIndex();

				}else{
					zs_print_error_cr("Expected 1 children");
					return ZS_UNDEFINED_IDX;
				}
			}else{
				zs_print_error_cr("Node not ARRAY_INDEX_NODE");
				return ZS_UNDEFINED_IDX;
			}
		}

		if(_node->pre_post_operator_info != PUNCTUATOR_TYPE::UNKNOWN_PUNCTUATOR){ // there's pre/post increment...
			// get post/inc
			tInfoStatementOpCompiler *ptr_current_statement_op = &this->m_currentFunctionInfo->stament[this->m_currentFunctionInfo->stament.size()-1];
			tInfoAsmOpCompiler *asm_op = ptr_current_statement_op->asm_op[ptr_current_statement_op->asm_op.size()-1];
			asm_op->pre_post_op_type=post_operator2instruction_property(_node->pre_post_operator_info);

		}

		if(_node->children.size()==3){ // array or function access ...

			return gacExpression_FunctionOrArrayAccess(node_2->idxAstNode, _lc);
		}
		// return last instruction where was modified
		return getCurrentInstructionIndex();
	}

	int CCompiler::gacExpression_ArrayObject_Recursive(short idxAstNode, CScope *_lc){

		return 0;
	}

	bool CCompiler::gacExpression_ArrayObject(short idxAstNode, CScope *_lc){

		PASTNode _node=AST_NODE(idxAstNode);

		if(_node == NULL) {zs_print_error_cr("NULL node");return false;}
		if(_node->node_type != ARRAY_OBJECT_NODE ){zs_print_error_cr("node is not ARRAY_OBJECT_NODE type or null");return false;}
		//int r=0;

		// 1. create object ...
		insert_CreateArrayObject_Instruction(_node->idxAstNode);
		int index_created_vec = CCompiler::getCurrentInstructionIndex();

		// 2. evaluate expressions if any
		for(unsigned j=0; j < _node->children.size(); j++){

			int ini_instruction = ZS_UNDEFINED_IDX;

			if(AST_NODE(_node->children[j])->node_type == EXPRESSION_NODE){
				/*children_is_vector_object = _node->children[j]->children[0]->node_type == ARRAY_OBJECT_NODE;*/
				ini_instruction = getCurrentInstructionIndex()+1;
			}
			// check whether is expression node...
			if(!gacExpression(_node->children[j], _lc,ini_instruction)){
				return false;
			}
			insert_ArrayObject_PushValueInstruction(_node->idxAstNode,index_created_vec,ini_instruction);
		}
		return true;//index_created_vec;//CCompiler::getCurrentInstructionIndex();
	}

	bool CCompiler::gacExpression_FunctionObject(short idxAstNode, CScope *_lc){

		PASTNode _node=AST_NODE(idxAstNode);

		if(_node == NULL) {zs_print_error_cr("NULL node");return false;}
		if(_node->node_type != FUNCTION_OBJECT_NODE ){zs_print_error_cr("node is not FUNCTION_OBJECT_NODE type or null");return false;}
		if(_node->children.size()!=2) {zs_print_error_cr("Array access should have 2 children");return false;}

		CScriptFunctionObject * script_function=NULL;

		// 1. insert load reference created object ...
		if(functionSymbolExists(_node->symbol_value, _node->idxAstNode)){
				ZS_WRITE_ERROR_MSG(GET_AST_FILENAME_LINE(idxAstNode),"Function \"%s\" already defined !",_node->symbol_value.c_str());
				return false;
		}

		if((script_function=addLocalFunctionSymbol(_node->symbol_value,_node->idxAstNode)) == NULL){
			return false;
		}

		// compiles anonymous function ...
		if(!gacFunctionOrOperator(_node->idxAstNode,_lc, script_function)){
			return false;
		}

		return insertLoadValueInstruction(_node->idxAstNode, _lc);
	}

	int CCompiler::gacExpression_FunctionAccess(short idxAstNode, CScope *_lc){

		PASTNode _node=AST_NODE(idxAstNode);

		if(_node == NULL) {zs_print_error_cr("NULL node");return ZS_UNDEFINED_IDX;}
		if(_node->node_type != CALLING_OBJECT_NODE ){zs_print_error_cr("node is not CALLING_OBJECT_NODE type or null");return ZS_UNDEFINED_IDX;}
		if(!(_node->children.size()==2 || _node->children.size()==3)) {zs_print_error_cr("Array access should have 2 or 3 children");return ZS_UNDEFINED_IDX;}

		PASTNode node_0=AST_NODE(_node->children[0]),
				 node_1=AST_NODE(_node->children[1]),
				 node_2=NULL;

		if(_node->children.size()==3){
			node_2 = AST_NODE(_node->children[2]);
		}


		if(node_0->node_type != FUNCTION_REF_NODE && node_0->node_type != FUNCTION_OBJECT_NODE){zs_print_error_cr("Node is not FUNCTION_OBJECT_NODE type"); return ZS_UNDEFINED_IDX;}
		if(node_1->node_type != ARGS_PASS_NODE){zs_print_error_cr("Function has no index nodes "); return ZS_UNDEFINED_IDX;}

		// load function ...
		if(node_0->symbol_value != "--"){ // starts with symbol ...
			if(!insertLoadValueInstruction(node_0->idxAstNode,_lc)) {
				return ZS_UNDEFINED_IDX;
			}
		}

		int call_index = getCurrentInstructionIndex();

		// 1. insert push to pass values to all args ...
		PASTNode function_args =node_1;
		//insert_StartArgumentStack_Instruction(_node->idxAstNode);
		if(function_args->children.size() > 0){
			for(unsigned k = 0; k < function_args->children.size(); k++){

				// check whether is expression node...
				if(!gacExpression(function_args->children[k], _lc,getCurrentInstructionIndex()+1)){
					return ZS_UNDEFINED_IDX;
				}
			}
		}

		// 2. insert call instruction itself.
		insert_CallFunction_Instruction(_node->idxAstNode,call_index);

		if(_node->children.size()==3){ // array or function access ...

			return gacExpression_FunctionOrArrayAccess(node_2->idxAstNode, _lc);
		}

		return CCompiler::getCurrentInstructionIndex();
	}

	int CCompiler::gacExpression_StructAttribute(short idxAstNode, CScope *_lc, int index_ref_object){

		PASTNode _node = AST_NODE(idxAstNode);

		if(_node == NULL) {zs_print_error_cr("NULL node");return ZS_UNDEFINED_IDX;}
		if(_node->node_type != EXPRESSION_NODE ){zs_print_error_cr("node is not EXPRESSION_NODE type or null");return ZS_UNDEFINED_IDX;}


		tInfoStatementOpCompiler *ptr_current_statement_op = &this->m_currentFunctionInfo->stament[this->m_currentFunctionInfo->stament.size()-1];
		int index_attr = getCurrentInstructionIndex()+1;

		// 1st evalualte expression...
		if(!gacExpression(_node->idxAstNode, _lc,getCurrentInstructionIndex()+1)){
			return ZS_UNDEFINED_IDX;
		}

		// 2nd insert strign constant ...
		insertStringConstantValueInstruction(_node->idxAstNode,_node->symbol_value);

		if(!(
			 ptr_current_statement_op->asm_op[index_attr]->operator_type == ASM_OPERATOR::DECL_VEC
		  || ptr_current_statement_op->asm_op[index_attr]->operator_type == ASM_OPERATOR::NEW
		  || ptr_current_statement_op->asm_op[index_attr]->operator_type == ASM_OPERATOR::DECL_STRUCT
		)){
			index_attr = getCurrentInstructionIndex()-1;
		}

		// 3rd insert push attribute...
		insert_PushAttribute_Instruction(_node->idxAstNode,index_ref_object,index_attr);

		return CCompiler::getCurrentInstructionIndex();
	}

	int CCompiler::gacExpression_Struct(short idxAstNode, CScope *_lc){

		PASTNode _node=AST_NODE(idxAstNode);

		if(_node == NULL) {zs_print_error_cr("NULL node");return ZS_UNDEFINED_IDX;}
		if(_node->node_type != STRUCT_NODE ){zs_print_error_cr("node is not STRUCT_NODE type or null");return ZS_UNDEFINED_IDX;}

		insert_DeclStruct_Instruction(_node->idxAstNode);
		int ref_obj = getCurrentInstructionIndex();

		for(unsigned i = 0; i < _node->children.size(); i++){
			// evaluate attribute
			if(gacExpression_StructAttribute(_node->children[i], _lc,ref_obj) == ZS_UNDEFINED_IDX){
				return ZS_UNDEFINED_IDX;
			}
		}
		return ref_obj;
	}

	bool CCompiler::isThisScope(short idxAstNode){

		PASTNode _node = AST_NODE(idxAstNode);

		if(_node == NULL){
			return false;
		}

		return ((_node->node_type == PUNCTUATOR_NODE) &&
				   //(_node->parent != NULL && _node->parent->node_type != PUNCTUATOR_NODE) &&
				   (_node->children.size()==2 && AST_NODE(_node->children[0])->symbol_value=="this")
				   );
	}

	bool CCompiler::isSuperScope(short idxAstNode){

		PASTNode _node=AST_NODE(idxAstNode);

		if(_node == NULL){
			return false;
		}

		return ((_node->node_type == PUNCTUATOR_NODE) &&
				   //(_node->parent != NULL && _node->parent->node_type != PUNCTUATOR_NODE) &&
				   (_node->children.size()==2 && AST_NODE(_node->children[0])->symbol_value=="super")
				   );
	}

	int CCompiler::gacExpression_Recursive(short idxAstNode, CScope *_lc, int & index_instruction){

		PASTNode _node = AST_NODE(idxAstNode);

		int r=index_instruction;
		string error_str;
		bool access_node = false;
		PUNCTUATOR_TYPE pre_operator = PUNCTUATOR_TYPE::UNKNOWN_PUNCTUATOR;
		switch(_node->pre_post_operator_info){
		case PUNCTUATOR_TYPE::POST_DEC_PUNCTUATOR:
		case PUNCTUATOR_TYPE::POST_INC_PUNCTUATOR:
			break;
		default:
			pre_operator=_node->pre_post_operator_info;
			break;
		}

		if(_node==NULL){
			return ZS_UNDEFINED_IDX;
		}

		if( isThisScope(_node->idxAstNode)
		 || isSuperScope(_node->idxAstNode)){ // only take care left children...
			_node = AST_NODE(_node->children[1]);
		}

		bool special_node =	 _node->node_type == ARRAY_OBJECT_NODE || // =[]
							_node->node_type == FUNCTION_OBJECT_NODE || // =function()
							_node->node_type == CALLING_OBJECT_NODE ||  // pool[] or pool()
							_node->node_type == NEW_OBJECT_NODE ||  // new
							//_node->node_type == DELETE_OBJECT_NODE ||  // new
							_node->node_type == STRUCT_NODE;

		// TERMINAL SYMBOLS OR SPECIAL NODE
		if(_node->children.size()==0 ||
				special_node
			)
		{
			bool function_access = false;
			bool array_access = false;

			PASTNode eval_node_sp = _node;
			if(_node->node_type == CALLING_OBJECT_NODE){
				if(_node->children.size() > 0){
					eval_node_sp = AST_NODE(_node->children[0]);

					function_access = eval_node_sp->node_type == FUNCTION_OBJECT_NODE || eval_node_sp->node_type == FUNCTION_REF_NODE;
					array_access = eval_node_sp->node_type == ARRAY_OBJECT_NODE || eval_node_sp->node_type == ARRAY_REF_NODE;


				}else {
					zs_print_error_cr("Calling object should have at least 1 children");
					return ZS_UNDEFINED_IDX;
				}
			}

			if(special_node ){
					if(array_access){
						if((r=gacExpression_ArrayAccess(_node->idxAstNode, _lc)) == ZS_UNDEFINED_IDX){
													return ZS_UNDEFINED_IDX;
												}
					}else if(function_access){
						if((r=gacExpression_FunctionAccess(_node->idxAstNode, _lc)) == ZS_UNDEFINED_IDX){
							return ZS_UNDEFINED_IDX;
						}
					}
					else{
						switch(eval_node_sp->node_type){
						case ARRAY_OBJECT_NODE: // should have 1 children
							if(!gacExpression_ArrayObject(_node->idxAstNode, _lc)){
								return ZS_UNDEFINED_IDX;
							}

							index_instruction = r;
							return r;
							break;

						case FUNCTION_OBJECT_NODE: // should have 1 children
							if(!gacExpression_FunctionObject(_node->idxAstNode, _lc)){
								return ZS_UNDEFINED_IDX;
							}
							r=CCompiler::getCurrentInstructionIndex();
							break;

						case NEW_OBJECT_NODE:
							if((gacNew(_node->idxAstNode, _lc))==ZS_UNDEFINED_IDX){
								return ZS_UNDEFINED_IDX;
							}

							r=CCompiler::getCurrentInstructionIndex();
							break;
						/*case DELETE_OBJECT_NODE:

							if((gacDelete(_node->idxAstNode, _lc))==ZS_UNDEFINED_IDX){
								return ZS_UNDEFINED_IDX;
							}

							r=CCompiler::getCurrentInstructionIndex();
							break;*/
						case STRUCT_NODE:
							if((r=gacExpression_Struct(_node->idxAstNode, _lc))==ZS_UNDEFINED_IDX){
								return ZS_UNDEFINED_IDX;
							}
							//r=CCompiler::getCurrentInstructionIndex();
							break;
						default:
							zs_print_error_cr("Unexpected node type %i",eval_node_sp->node_type);
							return ZS_UNDEFINED_IDX;
							break;
						}
					}
			}
			else{ // TERMINAL NODE

				if(!insertLoadValueInstruction(_node->idxAstNode, _lc)){
					return ZS_UNDEFINED_IDX;
				}
			}
			index_instruction=r;
		}else{

			if(_node->operator_info == PUNCTUATOR_TYPE::UNKNOWN_PUNCTUATOR){
				ZS_WRITE_ERROR_MSG(GET_AST_FILENAME_LINE(_node->idxAstNode),"Malformed expression at line %i");
				return ZS_UNDEFINED_IDX;
			}

			if(_node->operator_info == TERNARY_IF_PUNCTUATOR){
				if(AST_NODE(_node->children[1])->operator_info == TERNARY_ELSE_PUNCTUATOR){
					// node children[0]: conditional.
					// node children[1]: body-if
					// node children[2]: body-else
					//inline_if_else = true;
					//return ZS_UNDEFINED_IDX;

					int t1= gacInlineIf(_node->idxAstNode,_lc,index_instruction);

					return t1;

				}else{
					ZS_WRITE_ERROR_MSG(GET_AST_FILENAME_LINE(_node->idxAstNode)," Put parenthesis on the inner ternary conditional");
					return ZS_UNDEFINED_IDX;
				}
			}else{
				// only inserts terminal symbols...
				if(_node!=NULL ){
					access_node = _node->operator_info == PUNCTUATOR_TYPE::FIELD_PUNCTUATOR;
				}
				// check if there's inline-if-else
				int right=ZS_UNDEFINED_IDX, left=ZS_UNDEFINED_IDX;
				if((left=gacExpression_Recursive(_node->children[LEFT_NODE], _lc,index_instruction)) == ZS_UNDEFINED_IDX){
					return ZS_UNDEFINED_IDX;
				}

				if(_node->children.size()==2){
					if((right=gacExpression_Recursive(_node->children[RIGHT_NODE],_lc,index_instruction)) == ZS_UNDEFINED_IDX){
						return ZS_UNDEFINED_IDX;
					}
				}
				else {
					right = ZS_UNDEFINED_IDX;
				}

				r=index_instruction;

				// get last load symbol ..
				if(access_node){
					r=index_instruction-1;
				}

				if(left !=ZS_UNDEFINED_IDX && right!=ZS_UNDEFINED_IDX){ // 2 ops

					// Ignore punctuator node. Only take cares about terminal symbols...
					if(!access_node){

						// particular case if operator is =
						if(!insertOperatorInstruction(_node->operator_info,_node->idxAstNode,error_str,left,right)){
							ZS_WRITE_ERROR_MSG(GET_AST_FILENAME_LINE(_node->idxAstNode),"%s",error_str.c_str());
							return ZS_UNDEFINED_IDX;
						}
					}

				}else if(right!=ZS_UNDEFINED_IDX){ // one op..
					if(!insertOperatorInstruction(_node->operator_info,_node->idxAstNode,  error_str,right)){
						ZS_WRITE_ERROR_MSG(GET_AST_FILENAME_LINE(_node->idxAstNode),"%s",error_str.c_str());
						return ZS_UNDEFINED_IDX;
					}

				}else if(left!=ZS_UNDEFINED_IDX){ // one op..
					if(!insertOperatorInstruction(_node->operator_info,_node->idxAstNode,error_str,left)){
						ZS_WRITE_ERROR_MSG(GET_AST_FILENAME_LINE(_node->idxAstNode),"%s",error_str.c_str());
						return ZS_UNDEFINED_IDX;
					}
				}else{ // ERROR
					zs_print_error_cr("ERROR both ops ==0!");
					return ZS_UNDEFINED_IDX;
				}
			}
		}

		// we ignore field node instruction ...
		if(!access_node){
			index_instruction++;
		}

		switch(pre_operator){
		case PUNCTUATOR_TYPE::LOGIC_NOT_PUNCTUATOR:
			insertNot(_node->idxAstNode);
			break;
		case PUNCTUATOR_TYPE::SUB_PUNCTUATOR:
			insertNeg(_node->idxAstNode);
			break;
		case PUNCTUATOR_TYPE::PRE_DEC_PUNCTUATOR:
		case PUNCTUATOR_TYPE::PRE_INC_PUNCTUATOR:
			{
				tInfoStatementOpCompiler *ptr_current_statement_op = &this->m_currentFunctionInfo->stament[this->m_currentFunctionInfo->stament.size()-1];
				tInfoAsmOpCompiler *asm_op = ptr_current_statement_op->asm_op[ptr_current_statement_op->asm_op.size()-1];
				asm_op->pre_post_op_type=pre_operator==PRE_DEC_PUNCTUATOR?INS_PROPERTY_PRE_DEC:INS_PROPERTY_PRE_INC;
				/*if(asm_op->index_op1==LOAD_TYPE::LOAD_TYPE_VARIABLE || asm_op->operator_type==VGET){
					asm_op->pre_post_op_type=pre_operator;
				}*/
				//ptr_current_statement_op->asm_op.push_back(asm_op);
			}
			break;
		default:
			break;
		}

		return r;
	}

	int findConstructorIdxNode(short idxAstNode){

		PASTNode _node=AST_NODE(idxAstNode);

		if(_node->node_type!=NODE_TYPE::ARGS_PASS_NODE) {zs_print_error_cr("children[0] is not args_pass_node");return ZS_UNDEFINED_IDX;}
		for(unsigned i = 0; i < _node->children.size(); i++){
			PASTNode child_node = AST_NODE(_node->children[i]);
			if(child_node->node_type == NODE_TYPE::KEYWORD_NODE){

				if(child_node->keyword_info==KEYWORD_TYPE::FUNCTION_KEYWORD){
					if(child_node->symbol_value == _node->symbol_value){
						return i;
					}
				}
			}
		}
		return ZS_UNDEFINED_IDX;
	}

	PASTNode itHasReturnSymbol(PASTNode _node){

		PASTNode _ret;
		if(_node == NULL) return NULL;
		if(_node->keyword_info == RETURN_KEYWORD) return _node;

		for(unsigned i = 0; i < _node->children.size(); i++){
			if((_ret = itHasReturnSymbol(AST_NODE(_node->children[i]))) != NULL){
				return _ret;
			}
		}
		return NULL;//itHasReturnSymbol(PASTNode _node);
	}

	bool CCompiler::doRegisterVariableSymbolsClass(const string & class_name, CScriptClass *current_class){

		PASTNode _node_ret=NULL;
		string symbol_value;

		if(current_class == NULL){
			return true;
		}

		if(current_class->idxBaseClass.size() > 0){
			for(int i = 0; i < (int)current_class->idxBaseClass.size() ; i++){
				if(!CCompiler::doRegisterVariableSymbolsClass(class_name,CScriptClass::getScriptClassByIdx(current_class->idxBaseClass.at(i)))){
					return false;
				}
			}
		}

		PASTNode node_class = AST_NODE(current_class->metadata_info.object_info.symbol_info.idxAstNode);
		string current_class_name = current_class->metadata_info.object_info.symbol_info.symbol_name;
		PASTNode symbol_node = AST_NODE(node_class->children[0]);
		// register all vars...

		for(unsigned i = 0; i < symbol_node->children.size(); i++){ // foreach declared var.

			PASTNode child_node = AST_NODE(symbol_node->children[i]);

			for(unsigned j = 0; j < child_node->children.size(); j++){ // foreach element declared within ','

				tInfoVariableSymbol *irs_dest=NULL;
				symbol_value = AST_NODE(child_node->children[j])->symbol_value;

				if((irs_dest=CScriptClass::registerVariableSymbol(
						class_name,
						AST_NODE(child_node->children[j])->symbol_value,
						child_node->children[j]
					)) == NULL){
					return false;
				}

				if(current_class->is_c_class()){

					tInfoVariableSymbol *irs_src=CScriptClass::getRegisteredVariableSymbol(current_class_name, symbol_value);

					if(irs_src){

						// copy c refs ...
						irs_dest->ref_ptr=irs_src->ref_ptr;
						irs_dest->c_type = irs_src->c_type;
						irs_dest->idxAstNode=-1;
						irs_dest->properties = irs_src->properties;
					}else{
						return false;
					}
				}
			}
		}

		zs_print_debug_cr("registering base class %s",current_class_name.c_str());
		for(unsigned i = 0; i < AST_NODE(node_class->children[1])->children.size(); i++){

			PASTNode node_fun = AST_NODE(AST_NODE(node_class->children[1])->children[i]);
			string symbol_value = node_fun->symbol_value;

			zs_print_debug_cr("* %s::%s",current_class_name.c_str(), symbol_value.c_str());

		}

		// register all functions ...
		for(unsigned i = 0; i < AST_NODE(node_class->children[1])->children.size(); i++){
			CScriptFunctionObject *irfs;
			PASTNode node_fun = AST_NODE(AST_NODE(node_class->children[1])->children[i]);
			string symbol_value = node_fun->symbol_value;

			if(current_class_name == symbol_value){ // constructor symbol...
				symbol_value = class_name; // rename to be base constructor later ...

				if((_node_ret=itHasReturnSymbol(node_fun))!=NULL){
					ZS_WRITE_ERROR_MSG(GET_AST_FILENAME_LINE(_node_ret->idxAstNode),"return keyword is not allowed in constructor");
					return false;
				}
			}

			if((irfs=CScriptClass::registerFunctionSymbol(
					class_name,
					symbol_value,
					node_fun->idxAstNode
			)) == NULL){
				return false;
			}

			if(current_class->is_c_class()){ // set c refs ...

				CScriptFunctionObject *irs_src=CScriptClass::getScriptFunctionObjectByClassFunctionName(
						current_class_name,
						node_fun->symbol_value);

				if(irs_src){

					// copy c refs ...
					irfs->object_info.symbol_info.symbol_name= symbol_value;//irs_src->object_info.symbol_info.symbol_name;
					irfs->object_info.symbol_info.properties = irs_src->object_info.symbol_info.properties;

					irfs->object_info.symbol_info.ref_ptr = irs_src->object_info.symbol_info.ref_ptr;
					irfs->m_arg = irs_src->m_arg;
					irfs->idx_return_type = irs_src->idx_return_type;

				}else{
					return false;
				}

			}else{ // compile function ...
				// compile function (within scope class)...
				if(!gacFunctionOrOperator(node_fun->idxAstNode, SCOPE_INFO_NODE(node_class->idxScope),irfs)){
					return false;
				}


			}
		}

		return true;
	}

	bool CCompiler::gacClass(short idxAstNode, CScope * _lc){

		PASTNode _node = AST_NODE(idxAstNode);

		if(_node == NULL) {zs_print_error_cr("NULL node");return false;}
		if(_node->node_type != KEYWORD_NODE){zs_print_error_cr("node is not keyword type or null");return false;}
		if(_node->keyword_info != KEYWORD_TYPE::CLASS_KEYWORD){zs_print_error_cr("node is not CLASS keyword type");return false;}
		if(_node->children.size()==3 && AST_NODE(_node->children[2])->node_type != BASE_CLASS_NODE){zs_print_error_cr("expected BASE CLASS keyword type");return false;}

		string base_class= "";
		if(_node->children.size()==3){
			base_class=	AST_NODE(_node->children[2])->symbol_value;
		}

		CScriptClass *irc;

		// children[0]==var_collection && children[1]=function_collection
		if(_node->children.size()!=2 && _node->children.size()!=3) {zs_print_error_cr("node CLASS has not valid number of nodes");return false;}

		if(_node->children.size() == 3){
			AST_NODE(_node->children[2])->symbol_value = base_class;
		}

		// verify class is not already registered...
		if((irc=CScriptClass::registerClass(_node->symbol_value,base_class,_node)) == NULL){
			return false;
		}
		// we push class symbol ref as function, then all registered symbols will take account with this scope...
		pushFunction(_node->idxAstNode,&irc->metadata_info);

		// register all symbols ...
		if(!doRegisterVariableSymbolsClass(_node->symbol_value,irc)){
			return false;
		}
		// pop class ref so we go back to main scope (don't save its statment op)
		popFunction(false);
		return true;
	}

	int CCompiler::gacNew(short idxAstNode, CScope * _lc){
		PASTNode _node = AST_NODE(idxAstNode);

		if(_node == NULL) {zs_print_error_cr("NULL node");return ZS_UNDEFINED_IDX;}
		if(_node->node_type != NEW_OBJECT_NODE ){zs_print_error_cr("node is not NEW OBJECT NODE type");return ZS_UNDEFINED_IDX;}
		if(_node->children.size()!=1) {zs_print_error_cr("node NEW has not valid number of nodes");return ZS_UNDEFINED_IDX;}
		if(AST_NODE(_node->children[0])->node_type!=NODE_TYPE::ARGS_PASS_NODE) {zs_print_error_cr("children[0] is not args_pass_node");return ZS_UNDEFINED_IDX;}

		tInfoAsmOpCompiler *iao=NULL;

		// load function ...
		// 1. create object instruction ...
		if(!insert_NewObject_Instruction(_node->idxAstNode,_node->symbol_value)) // goto end  ...
		{
			return ZS_UNDEFINED_IDX;
		}

		// 2. load function ...

		if(!insertLoadValueInstruction(_node->idxAstNode,_lc,&iao)) {
			return ZS_UNDEFINED_IDX;
		}

		// set load as function constructor ...
		(*iao).runtime_prop = INS_PROPERTY_CONSTRUCT_CALL;


		int call_index = getCurrentInstructionIndex();

		// 3. pass parameters ...
		PASTNode constructor_args = AST_NODE(_node->children[0]);

		if(constructor_args->children.size() > 0){

			//insert_StartArgumentStack_Instruction(_node->idxAstNode);

			for(unsigned k = 0; k < constructor_args->children.size(); k++){

				// check whether is expression node...
				if(!gacExpression(constructor_args->children[k], _lc,getCurrentInstructionIndex()+1)){
					return ZS_UNDEFINED_IDX;
				}

			}
		}else{
			// clear the stack only ..
			//insert_ClearArgumentStack_Instruction(_node->idxAstNode);
		}

		// 4. call function...
		insert_CallFunction_Instruction(_node->idxAstNode,call_index);

		return CCompiler::getCurrentInstructionIndex();
	}

	int CCompiler::gacDelete(short idxAstNode, CScope * _lc){


		PASTNode _node = AST_NODE(idxAstNode);

		if(_node == NULL) {zs_print_error_cr("NULL node");return ZS_UNDEFINED_IDX;}
		if(_node->node_type != DELETE_OBJECT_NODE ){zs_print_error_cr("gacDelete: node is not NEW OBJECT NODE type");return ZS_UNDEFINED_IDX;}
		if(_node->children.size() != 1 ){zs_print_error_cr("gacDelete: expected 1 children");return ZS_UNDEFINED_IDX;}

		// load function ...
		// push value  ...
		/*if(!insertLoadValueInstruction(_node->idxAstNode, _lc)){
			return ZS_UNDEFINED_IDX;
		}*/
		// parse expression ...
		if(!ast2asm_Recursive(_node->children[0],SCOPE_INFO_NODE(_node->idxScope))){ return ZS_UNDEFINED_IDX;}

		// delete object instruction ...
		if(!insert_DeleteObject_Instruction(_node->idxAstNode)) // goto end  ...
		{
			return ZS_UNDEFINED_IDX;
		}
		return CCompiler::getCurrentInstructionIndex();
	}

	bool CCompiler::gacFor(short idxAstNode, CScope * _lc){

		PASTNode _node=AST_NODE(idxAstNode);

		if(_node == NULL) {zs_print_error_cr("NULL node");return false;}
		if(_node->node_type != KEYWORD_NODE ){zs_print_error_cr("node is not keyword type or null");return false;}
		if(_node->keyword_info != KEYWORD_TYPE::FOR_KEYWORD){zs_print_error_cr("node is not FOR keyword type");return false;}
		if(_node->children.size()!=4) {zs_print_error_cr("node FOR has not valid number of nodes");return false;}
		if(!(AST_NODE(_node->children[0])->node_type==PRE_FOR_NODE && AST_NODE(_node->children[1])->node_type==CONDITIONAL_NODE &&
		AST_NODE(_node->children[2])->node_type==POST_FOR_NODE && AST_NODE(_node->children[3])->node_type==BODY_NODE)) {zs_print_error_cr("node FOR has not valid TYPE nodes");return false;}
		tInfoAsmOpCompiler *asm_op;

		// 0. insert push statment
		newStatment();
		if(!insertPushScopeInstruction(_node->idxAstNode,_node->idxScope)){
			return false;
		}

		// 1. compile var init ...

		if(AST_NODE(_node->children[0])->children.size()>0){
			if(!ast2asm_Recursive(_node->children[0],SCOPE_INFO_NODE(_node->idxScope))){ return false;}
		}

		// 2. compile conditional
		if(!ast2asm_Recursive(_node->children[1],SCOPE_INFO_NODE(_node->idxScope))){ return false;}
		// get current index statment in order to jmp from end body for.
		int index_statment_conditional_for_= getCurrentStatmentIndex();

		// insert conditional jmp (if not true go to the end)
		asm_op = insert_JNT_Instruction(_node->children[1]);

		// 3. compile body

		if(!gacBody(_node->children[3],SCOPE_INFO_NODE(AST_NODE(_node->children[3])->idxScope))){ return false;}

		// 4. compile post oper
		if(AST_NODE(_node->children[2])->children.size()>0 && AST_NODE(_node->children[2])->children[0] != ZS_UNDEFINED_IDX){
			if(!ast2asm_Recursive(_node->children[2],SCOPE_INFO_NODE(_node->idxScope))){ return false;}
		}

		// 5. jmp to the conditional index ...
		insert_JMP_Instruction(_node->children[1],index_statment_conditional_for_);

		// 6. insert pop scope...
		newStatment();
		insertPopScopeInstruction(_node->idxAstNode);
		//}

		// save jmp value...
		asm_op->index_op2=getCurrentStatmentIndex();
		return true;
	}

	bool CCompiler::gacWhile(short idxAstNode, CScope * _lc){

		PASTNode _node = AST_NODE(idxAstNode);

		if(_node == NULL) {zs_print_error_cr("NULL node");return false;}
		if(_node->node_type != KEYWORD_NODE){zs_print_error_cr("node is not keyword type or null");return false;}
		if(_node->keyword_info != KEYWORD_TYPE::WHILE_KEYWORD){zs_print_error_cr("node is not WHILE keyword type");return false;}
		if(_node->children.size()!=2) {zs_print_error_cr("node WHILE has not valid number of nodes");return false;}
		if(!(AST_NODE(_node->children[0])->node_type==CONDITIONAL_NODE && AST_NODE(_node->children[1])->node_type==BODY_NODE )) {zs_print_error_cr("node WHILE has not valid TYPE nodes");return false;}
		tInfoAsmOpCompiler *asm_op_jmp_end;
		int index_ini_while;

		// compile conditional expression...
		if(!ast2asm_Recursive(_node->children[0],_lc)){ return false;}
		index_ini_while = getCurrentStatmentIndex();
		asm_op_jmp_end = insert_JNT_Instruction(_node->children[0]); // goto end  ...

		// compile if-body ...
		if(!gacBody(_node->children[1],SCOPE_INFO_NODE(AST_NODE(_node->children[1])->idxScope))){ return false;}
		insert_JMP_Instruction(_node->children[0],index_ini_while); // goto end  ...

		// save jmp value ...
		asm_op_jmp_end->index_op2= getCurrentStatmentIndex()+1;
		return true;
	}

	bool CCompiler::gacDoWhile(short idxAstNode, CScope * _lc){

		PASTNode _node = AST_NODE(idxAstNode);

		if(_node == NULL) {zs_print_error_cr("NULL node");return false;}
		if(_node->node_type != KEYWORD_NODE){zs_print_error_cr("node is not keyword type or null");return false;}
		if(_node->keyword_info != KEYWORD_TYPE::DO_WHILE_KEYWORD){zs_print_error_cr("node is not DO_WHILE keyword type");return false;}
		if(_node->children.size()!=2) {zs_print_error_cr("node DO-WHILE has not valid number of nodes");return false;}
		if(!(AST_NODE(_node->children[0])->node_type==CONDITIONAL_NODE && AST_NODE(_node->children[1])->node_type==BODY_NODE )) {zs_print_error_cr("node WHILE has not valid TYPE nodes");return false;}
		int index_start_do_while;

		// compile conditional expression...
		index_start_do_while =  getCurrentStatmentIndex()+1;

		// compile do-body ...
		if(!gacBody(_node->children[1],SCOPE_INFO_NODE(AST_NODE(_node->children[1])->idxScope))){ return false;}

		// compile while condition...
		if(!ast2asm_Recursive(_node->children[0],_lc)){ return false;}

		insert_JT_Instruction(_node->children[0],index_start_do_while); // goto end  ...

		// save jmp value ...
		//asm_op_jmp_end->index_op2= index_start_do_while;
		return true;
	}

	bool CCompiler::gacReturn(short idxAstNode, CScope * _lc){

		PASTNode _node = AST_NODE(idxAstNode);

		if(_node == NULL) {zs_print_error_cr("NULL node");return false;}
		if(_node->node_type != KEYWORD_NODE){zs_print_error_cr("node is not keyword type or null");return false;}
		if(_node->keyword_info != KEYWORD_TYPE::RETURN_KEYWORD){zs_print_error_cr("node is not RETURN keyword type");return false;}
		if(_node->children.size() != 1){zs_print_error_cr("node RETURN has not 1 child");return false;}

		if(gacExpression(_node->children[0], _lc)){
			// finally we put mov to Value ...
			insertRet(_node->idxAstNode,getCurrentInstructionIndex());
		}
		else {
			return false;
		}
		return true;
	}

	bool CCompiler::gacFunctionOrOperator(short idxAstNode, CScope * _lc, CScriptFunctionObject *irfs){

		PASTNode _node = AST_NODE(idxAstNode);

		if(_node == NULL) {zs_print_error_cr("NULL node");return false;}

		if(
			! ( _node->keyword_info == KEYWORD_TYPE::FUNCTION_KEYWORD
			||  _node->node_type    == FUNCTION_OBJECT_NODE
			)) {
			ZS_WRITE_ERROR_MSG(GET_AST_FILENAME_LINE(_node->idxAstNode),"Expected FUNCTION or OPERATOR or FUNCTION_OBJECT_NODE keyword type");
			return false;
		}

		if(_node->children.size() != 2){zs_print_error_cr("node FUNCTION has not 2 child");return false;}
		if(AST_NODE(_node->children[0])->node_type != NODE_TYPE::ARGS_DECL_NODE){zs_print_error_cr("node FUNCTION has not ARGS node");return false;}
		if(AST_NODE(_node->children[1])->node_type != NODE_TYPE::BODY_NODE){zs_print_error_cr("node FUNCTION has not BODY node");return false;}

		// 1. Processing args ...
		for(unsigned i = 0; i < AST_NODE(_node->children[0])->children.size(); i++){
			string ss = AST_NODE(AST_NODE(_node->children[0])->children[i])->symbol_value;
			irfs->m_arg.push_back(
					{ZS_UNDEFINED_IDX,ss}
			);
		}
		// 2. Compiles the function ...
		return compile(_node->children[1], irfs);
	}

	bool CCompiler::gacIf(short idxAstNode, CScope * _lc){

		PASTNode _node = AST_NODE(idxAstNode);

		if(_node == NULL) {zs_print_error_cr("NULL node");return false;}
		if(_node->node_type != KEYWORD_NODE ){zs_print_error_cr("node is not keyword type or null");return false;}
		if(_node->keyword_info != KEYWORD_TYPE::IF_KEYWORD){zs_print_error_cr("node is not IF keyword type");return false;}
		if(_node->children.size()<1) {zs_print_error_cr("node IF has not valid number of nodes");return false;}

		CASTNode *if_group_nodes =  AST_NODE(_node->children[0]);

		if(if_group_nodes->node_type!=GROUP_IF_NODES) {zs_print_error_cr("node IF has not GROUP IF NODES");return false;}
		if((if_group_nodes->children.size()<1)) {zs_print_error_cr("GROUP IF NODES has to have at least one node.");return false;}

		tInfoAsmOpCompiler *asm_op_jmp_else_if;

		vector<tInfoAsmOpCompiler *> asm_op_jmp_end;



		for(unsigned int i=0; i < if_group_nodes->children.size(); i++){

			CASTNode * if_node =  AST_NODE(if_group_nodes->children[i]);
			if(if_node->children.size() != 2) {zs_print_error_cr(" IF NODE HAS TO HAVE 2 NODES");return false;}

			// compile conditional expression...
			if(!ast2asm_Recursive(if_node->children[0],_lc)){ return false;}
			asm_op_jmp_else_if = insert_JNT_Instruction(if_node->children[0]); // goto else body ...

			// compile if-body ...
			if(!gacBody(if_node->children[1],SCOPE_INFO_NODE(AST_NODE(if_node->children[1])->idxScope))){ return false;}

			asm_op_jmp_end.push_back(insert_JMP_Instruction(_node->idxAstNode));
			asm_op_jmp_else_if->index_op2 = getCurrentStatmentIndex()+1;

		}

		// if there's else body, compile-it
		if(_node->children.size()==2){
			//asm_op_jmp_end = insert_JMP_Instruction(_node->idxAstNode); // goto end
			asm_op_jmp_else_if->index_op2 = getCurrentStatmentIndex()+1;
			if(!gacBody(_node->children[1],_lc)){ return false;}

			//asm_op_jmp_end->index_op2 = getCurrentStatmentIndex()+1;
		}

		// assign all if jmp after block finishes (except else)
		for(unsigned j = 0; j < asm_op_jmp_end.size();j++){
			asm_op_jmp_end[j]->index_op2 = getCurrentStatmentIndex()+1;
		}

		return true;
	}

	int CCompiler::gacInlineIf(short idxAstNode, CScope * _lc, int & instruction){

		PASTNode _node = AST_NODE(idxAstNode);

		if(_node == NULL) {zs_print_error_cr("NULL node");return ZS_UNDEFINED_IDX;}
		if(_node->node_type != PUNCTUATOR_NODE ){zs_print_error_cr("node is not punctuator type or null");return ZS_UNDEFINED_IDX;}
		if(_node->operator_info != TERNARY_IF_PUNCTUATOR){zs_print_error_cr("node is not INLINE-IF PUNCTUATOR type");return ZS_UNDEFINED_IDX;}
		if(_node->children.size()!=2) {zs_print_error_cr("node INLINE-IF has not 2 nodes");return ZS_UNDEFINED_IDX;}
		if(!(AST_NODE(_node->children[1])->node_type==PUNCTUATOR_NODE && AST_NODE(_node->children[1])->operator_info==TERNARY_ELSE_PUNCTUATOR )) {zs_print_error_cr("node INLINE-ELSE has not found");return ZS_UNDEFINED_IDX;}
		if(AST_NODE(_node->children[1])->children.size() != 2) {zs_print_error_cr("node INLINE-ELSE has not 2 nodes");return ZS_UNDEFINED_IDX;}
		tInfoAsmOpCompiler *asm_op_jmp_else_if,*asm_op_jmp_end;

		int r=instruction;

		// compile conditional expression...
		if((r=gacExpression_Recursive(_node->children[0],_lc,r))==ZS_UNDEFINED_IDX){ return ZS_UNDEFINED_IDX;}
		asm_op_jmp_else_if = insert_JNT_Instruction(_node->children[0]); // goto else body ...

		// compile if-body ...
		r+=2; // in order to execute recursive expression we have to advance r pointer jnt (+2)
		if((r=gacExpression_Recursive(AST_NODE(_node->children[1])->children[0],_lc,r))==ZS_UNDEFINED_IDX){ return ZS_UNDEFINED_IDX;}

		//insert_Save_CurrentInstruction();

		// compile else-body ...
		asm_op_jmp_end = insert_JMP_Instruction(_node->idxAstNode); // goto end+

		asm_op_jmp_else_if->index_op1 = getCurrentInstructionIndex()+1;
		asm_op_jmp_else_if->index_op2 = getCurrentStatmentIndex();


		r+=3;

		if((r=gacExpression_Recursive(AST_NODE(_node->children[1])->children[1],_lc,r))==ZS_UNDEFINED_IDX){ return ZS_UNDEFINED_IDX;}

		asm_op_jmp_end->index_op1 = getCurrentInstructionIndex()+1;
		asm_op_jmp_end->index_op2 = getCurrentStatmentIndex();

		instruction = r+1;

		return r;
	}

	bool CCompiler::gacSwitch(short idxAstNode, CScope * _lc){

		PASTNode _node = AST_NODE(idxAstNode);

		if(_node == NULL) {zs_print_error_cr("NULL node");return false;}
		if(_node->node_type != KEYWORD_NODE ){zs_print_error_cr("node is not keyword type or null");return false;}
		if(_node->keyword_info != SWITCH_KEYWORD){zs_print_error_cr("node is not SWITCH keyword type");return false;}
		bool has_default = false;
		PASTNode switch_node;
		PASTNode group_cases;
		PASTNode case_value;
		PASTNode case_body;

		tInfoAsmOpCompiler * asm_op=NULL;

		string error_str;
		string detected_type_str;
		//int idxScope=AST_SCOPE_INFO_IDX(this->m_currentFunctionInfo->function_info_object->object_info.symbol_info.idxAstNode);
		 //CScope *_scope = SCOPE_INFO_NODE(idxScope)->getCurrentScopePointer();
		 vector<tInfoAsmOpCompiler *> jmp_instruction;//=NULL;
		 vector<vector<tInfoAsmOpCompiler *>> jt_instruction;

		// create new statment ...
		CCompiler::getInstance()->newStatment();

		if(ast2asm_Recursive(_node->children[0],_lc)){ // insert condition value ...

			// get current instruction value to take as ref for compare within value cases...
			int switch_value_index  = getCurrentInstructionIndex();

			// the stratege is first evaluate all cases and then their bodies...
			for(unsigned s=0; s < 2; s++){
				// start +1 because 0 is switch value ...
				for(unsigned i = 1; i < _node->children.size(); i++){ // expect node type group cases ...
					switch_node = AST_NODE(_node->children[i]);

					if(switch_node != NULL){ // the rules are the following: children[0]:group_cases and children[1]:body_case

						if(switch_node->children.size() == 2){
							group_cases = AST_NODE(switch_node->children[0]);
							case_body = AST_NODE(switch_node->children[1]);
							switch(s){

							// case group
							case 0: // GENERATE ASM FOR CONDITIONAL CASES ...

								jt_instruction.push_back(vector<tInfoAsmOpCompiler *>{});

								if(group_cases->node_type == GROUP_CASES_NODE){

									//int condition_index  = getCurrentInstructionIndex();

									for(unsigned j = 0; j < group_cases->children.size(); j++){ // generate condition case ...
										case_value = AST_NODE(group_cases->children[j]);
										//jmp_instruction=NULL;

										if(case_value->node_type == KEYWORD_NODE && case_value->keyword_info != KEYWORD_TYPE::UNKNOWN_KEYWORD){

											switch(case_value->keyword_info){
											default:
												zs_print_error_cr("Unexpected %s keyword node in SWITCH node",CASTNode::defined_keyword[case_value->keyword_info].str);
												break;
											case DEFAULT_KEYWORD:

												if(!has_default){
													has_default = true;
													// insert jmp instruction and save its information to store where to jmp when we know the total code size of cases + body...
													jt_instruction[i-1].push_back(insert_JMP_Instruction(case_value->idxAstNode));
												}else{
													ZS_WRITE_ERROR_MSG(GET_AST_FILENAME_LINE(case_value->idxAstNode),"case already defined!");
													return false;
												}
												break;
											case CASE_KEYWORD:

												// load case X:
												insertLoadValueInstruction(case_value->idxAstNode,_lc);

												// is equal ? ==
												if(!insertOperatorInstruction(LOGIC_EQUAL_PUNCTUATOR,0, error_str, switch_value_index ,getCurrentInstructionIndex())){
														ZS_WRITE_ERROR_MSG(GET_AST_FILENAME_LINE(case_value->idxAstNode),"%s",error_str.c_str());
														return false;
												}

												// set property as READ_TWO_AND_ONE_POP...
												asm_op=getLastInsertedInfoAsmOpCompiler();
												asm_op->runtime_prop|=STK_PROPERTY_READ_TWO_POP_ONE;

												// insert jmp instruction and save its information to store where to jmp when we know the total code size of cases...
												jt_instruction[i-1].push_back(insert_JT_Instruction(case_value->idxAstNode));

												break;

											}

										}else{
											zs_print_error_cr("Not SWITCH case or NULL keyword info");
											return false;
										}
									}
								}else{
									zs_print_error_cr("Expected group cases type node in SWITCH node");
									return false;
								}
								break;

							// Body!
							case 1: // GENERATE ASM FOR BODY AND WRITE initial JMP

								for(unsigned j = 0; j < group_cases->children.size(); j++){
										case_value = AST_NODE(group_cases->children[j]);
										if(case_value->keyword_info == CASE_KEYWORD){
											jt_instruction[i-1][j]->index_op2= getCurrentStatmentIndex()+1;
										}else{ // default...
											jt_instruction[i-1][j]->index_op2= getCurrentStatmentIndex()+1;
										}
										//case_value = AST_NODE(group_cases->children[i]);
										//asm_op = jmp_instruction; // load jt instruction and set current instruction before write asm code.
										//asm_op->index_op2 = getCurrentStatmentIndex();

									}


								if(gacBody(case_body->idxAstNode,_lc)){

									if(i < (_node->children.size()-1))
										jmp_instruction.push_back(insert_JMP_Instruction(_node->idxAstNode));
								}else{
									return false;
								}
								break;

							/*case 2: // FINALLY, WRITE JMP's to end statment

								if((asm_op = (tInfoAsmOpCompiler *)jmp_instruction) != NULL){
									asm_op->index_op2 = getCurrentStatmentIndex()+1;
								}
								break;*/
							}
						}
						else{
							zs_print_error_cr("SWITCH node has not 2 nodes");
							return false;
						}
					}
					else{
						zs_print_error_cr("SWITCH node NULL");
						return false;
					}
				}
			}

			// update all jmp instructions ...
			for(unsigned j = 0; j < jmp_instruction.size(); j++){
				jmp_instruction[j]->index_op2=getCurrentStatmentIndex()+1;
			}
		}else{
			return false;
		}
		return true;
	}

	bool CCompiler::gacVar(short idxAstNode, CScope * _lc){

		PASTNode _node = AST_NODE(idxAstNode);

		if(_node == NULL) {zs_print_error_cr("NULL node");return false;}
		if(_node->node_type != KEYWORD_NODE ){zs_print_error_cr("node is not keyword type or null");return false;}
		if(_node->keyword_info != VAR_KEYWORD){zs_print_error_cr("node is not VAR keyword type");return false;}

		int local_variable_idx;

		for(unsigned i = 0; i < _node->children.size(); i++){ // for all vars ...

			if(localVarSymbolExists(AST_NODE(_node->children[i])->symbol_value, _node->idxAstNode)){
				ZS_WRITE_ERROR_MSG(GET_AST_FILENAME_LINE(_node->idxAstNode),"Variable \"%s\" already defined !",_node->symbol_value.c_str());
				return false;
			}

			if((local_variable_idx=addLocalVarSymbol(AST_NODE(_node->children[i])->symbol_value,_node->children[i])) == ZS_UNDEFINED_IDX){
				return false;
			}

			//for(unsigned i = 0 ; i < _node->children.size(); i++){ // init all requested vars...
			if(AST_NODE(_node->children[i])->children.size()==1){
				if(!gacExpression(AST_NODE(_node->children[i])->children[0], _lc)){
					return false;
				}
			}
		}
		return true;
	}

	bool CCompiler::gacKeyword(short idxAstNode, CScope * _lc){

		PASTNode _node = AST_NODE(idxAstNode);

		CScriptFunctionObject *function_object=NULL;
		if(_node == NULL) {zs_print_error_cr("NULL node");return false;}
		if(_node->node_type != KEYWORD_NODE){zs_print_error_cr("node is not keyword type or null");return false;}

		switch(_node->keyword_info){
		default:
			zs_print_error_cr("Keyword [ %s ] not implemented yet!",CASTNode::defined_keyword[_node->keyword_info].str);
			break;
		case KEYWORD_TYPE::CLASS_KEYWORD:
			return gacClass(_node->idxAstNode, _lc);
			break;
		case KEYWORD_TYPE::SWITCH_KEYWORD:
			return gacSwitch(_node->idxAstNode, _lc);
			break;
		case KEYWORD_TYPE::FOR_KEYWORD:
			return gacFor(_node->idxAstNode, _lc);
			break;
		case KEYWORD_TYPE::WHILE_KEYWORD:
			return gacWhile(_node->idxAstNode, _lc);
			break;
		case KEYWORD_TYPE::DO_WHILE_KEYWORD:
			return gacDoWhile(_node->idxAstNode, _lc);
			break;
		case KEYWORD_TYPE::IF_KEYWORD:
			return gacIf(_node->idxAstNode, _lc);
			break;
		case KEYWORD_TYPE::VAR_KEYWORD:
			return gacVar(_node->idxAstNode, _lc);
			break;
		case KEYWORD_TYPE::FUNCTION_KEYWORD: // don't compile function. It will compiled later, after main body


			if(functionSymbolExists(_node->symbol_value, _node->idxAstNode)){
				ZS_WRITE_ERROR_MSG(GET_AST_FILENAME_LINE(_node->idxAstNode),"Function \"%s\" already defined !",_node->symbol_value.c_str());
					return false;
			}

			if((function_object=addLocalFunctionSymbol(_node->symbol_value,_node->idxAstNode)) == NULL){
				return false;
			}

			return gacFunctionOrOperator(_node->idxAstNode, _lc,function_object);
			break;
		case KEYWORD_TYPE::RETURN_KEYWORD:
			return gacReturn(_node->idxAstNode, _lc);
			break;
		}

		return false;
	}

	bool CCompiler::gacBody(short idxAstNode, CScope * _lc){

		PASTNode _node = AST_NODE(idxAstNode);

		if(_node == NULL) {zs_print_error_cr("NULL node");return false;}
		if(_node->node_type != BODY_NODE ){zs_print_error_cr("node is not BODY type or null");return false;}


		// 0. insert push statment
		newStatment();
		if(!insertPushScopeInstruction(_node->idxAstNode,_node->idxScope)){
			return false;
		}

		if(_node->children.size() > 0){ // body has code ...

			for(unsigned i = 0; i < _node->children.size(); i++){
				if(!ast2asm_Recursive(_node->children[i], _lc))
					return false;
			}

		}else{ // no block. Insert one statment at least ..
			newStatment();
		}

		newStatment();
		insertPopScopeInstruction(_node->idxAstNode);//,index);

		return true;
	}

	bool CCompiler::gacExpression(short idxAstNode, CScope *_lc,int index_instruction){

		PASTNode _node = AST_NODE(idxAstNode);

		if(index_instruction == ZS_UNDEFINED_IDX){ // create new statment
			//int index_instruction=0;
			tInfoStatementOpCompiler i_stat;
			m_currentFunctionInfo->stament.push_back(i_stat);
			index_instruction = 0; // set as 0
		}

		if(_node == NULL) {zs_print_error_cr("NULL node");return false;}
		if(_node->node_type != EXPRESSION_NODE){zs_print_error_cr("node is not Expression");return false;}

		int r = gacExpression_Recursive(_node->children[0], _lc,index_instruction);

		return r!= ZS_UNDEFINED_IDX;
	}

	bool CCompiler::ast2asm_Recursive(short idxAstNode, CScope *_lc){

		PASTNode _node = AST_NODE(idxAstNode);

		if(_node != NULL){
			switch(_node->node_type){
				default:
				case UNKNOWN_NODE:
					zs_print_error_cr("UNKNOWN_NODE (%i)",_node->node_type);
					return false;
					break;
				case DELETE_OBJECT_NODE:
					zs_print_debug_cr("DELETE NODE");
					return gacDelete(_node->idxAstNode, _lc);
					break;
				case EXPRESSION_NODE: // in fact is EXPRESSION NODE
					zs_print_debug_cr("EXPRESSION_NODE");
					return gacExpression(_node->idxAstNode, _lc);
					break;
				case GROUP_CASES_NODE:
					zs_print_debug_cr("GROUP_CASES_NODE");
					break;
				case KEYWORD_NODE:
					zs_print_debug_cr("KEYWORD_NODE %s",CASTNode::defined_keyword[_node->keyword_info].str);
					return gacKeyword(_node->idxAstNode, _lc);
					break;
				case BODY_NODE:
					zs_print_debug_cr("BODY_NODE");
					return gacBody(_node->idxAstNode, SCOPE_INFO_NODE(_node->idxScope)); // we pass scope node
					break;
				case POST_FOR_NODE:
				case CONDITIONAL_NODE:
					zs_print_debug_cr("%s",_node->node_type == CONDITIONAL_NODE ? "CONDITIONAL_NODE":"POST_FOR_NODE");
					if(_node->children.size() == 1){
						return gacExpression(_node->children[0], _lc);
					}else{
						zs_print_error_cr("Expected nodes for %i",_node->node_type);
					}

					break;
				case PRE_FOR_NODE:
					if(_node->children.size() == 1){
						return ast2asm_Recursive(_node->children[0], _lc);
					}else{
						zs_print_error_cr("Expected nodes for %i",_node->node_type);
					}
					break;
				case FUNCTION_OBJECT_NODE:
					zs_print_debug_cr("FUNCTION_OBJECT");break;
					break;
				case CLASS_VAR_COLLECTION_NODE:zs_print_debug_cr("CLASS_VAR_COLLECTION_NODE");break;
				case CLASS_FUNCTION_COLLECTION_NODE:zs_print_debug_cr("CLASS_FUNCTION_COLLECTION_NODE");break;
				case BASE_CLASS_NODE:zs_print_debug_cr("BASE_CLASS_NODE");break;
				case CALLING_OBJECT_NODE:zs_print_debug_cr("CALLING_OBJECT_NODE");break;
			}
		}else{
			zs_print_error_cr("Node is null!");
		}
		return false;
	}

	void CCompiler::pushFunction(short idxAstNode,CScriptFunctionObject *sf){

		PASTNode _node =AST_NODE(idxAstNode);
		stk_scriptFunction.push_back(m_currentFunctionInfo=new tInfoFunctionCompile(sf));
		this->m_treescope = SCOPE_INFO_NODE(_node->idxScope);
	}

	void CCompiler::popFunction(bool save_statment_op, bool partial_save){

		m_currentFunctionInfo->function_info_object->object_info.statment_op=NULL;

		if (save_statment_op) {
			// reserve memory for statment struct...
			vector<tInfoStatementOpCompiler> *vec_comp_statment;
			vec_comp_statment = &m_currentFunctionInfo->stament;

			// get total size op + 1 ends with NULL
			unsigned size = (vec_comp_statment->size() + 1) * sizeof(PtrStatment);
			m_currentFunctionInfo->function_info_object->object_info.statment_op = (PtrStatment)malloc(size);
			memset(m_currentFunctionInfo->function_info_object->object_info.statment_op, 0, size);
			//m_currentFunctionInfo->function_info_object->object_info.n_statment_op = vec_comp_statment->size();

			m_currentFunctionInfo->function_info_object->object_info.n_statments=vec_comp_statment->size();

			for (unsigned i = 0; i < vec_comp_statment->size(); i++) { // foreach statment...
				// reserve memory for n ops + 1 NULL end of instruction...
				size = (vec_comp_statment->at(i).asm_op.size() + 1) * sizeof(tInfoAsmOp);

				m_currentFunctionInfo->function_info_object->object_info.statment_op[i] = (tInfoAsmOp *)malloc(size);

				memset(m_currentFunctionInfo->function_info_object->object_info.statment_op[i], 0, size);
				//m_currentFunctionInfo->function_info_object->object_info.statment_op[i].n_asm_op=vec_comp_statment->at(i).asm_op.size();
				//m_currentFunctionInfo->object_info.statment_op[i] = m_currentListStatements->at(i);
				for (unsigned j = 0; j < vec_comp_statment->at(i).asm_op.size(); j++) {

					tInfoAsmOpCompiler *asm_op = vec_comp_statment->at(i).asm_op[j];

					m_currentFunctionInfo->function_info_object->object_info.statment_op[i][j].idxAstNode = asm_op->idxAstNode;
					m_currentFunctionInfo->function_info_object->object_info.statment_op[i][j].operator_type = asm_op->operator_type;
					m_currentFunctionInfo->function_info_object->object_info.statment_op[i][j].index_op1 = asm_op->index_op1;
					m_currentFunctionInfo->function_info_object->object_info.statment_op[i][j].index_op2 = asm_op->index_op2;
					m_currentFunctionInfo->function_info_object->object_info.statment_op[i][j].instruction_properties = asm_op->pre_post_op_type | asm_op->scope_type | asm_op->runtime_prop;
				}
			}
		}

		// delete and popback function information...
		delete(m_currentFunctionInfo);
		stk_scriptFunction.pop_back();

		if(stk_scriptFunction.size() > 0){
			m_currentFunctionInfo = stk_scriptFunction[stk_scriptFunction.size()-1];
			//this->m_currentListStatements = &m_currentFunctionInfo->object_info.statment_op;
			this->m_treescope =AST_SCOPE_INFO(m_currentFunctionInfo->function_info_object->object_info.symbol_info.idxAstNode);
		}
	}

	void CCompiler::clear(){ // clears all compiled

	}

	bool CCompiler::compile(short idxAstNode ,CScriptFunctionObject *sf){

		PASTNode _node =AST_NODE(idxAstNode);

		if(_node == NULL){
			zs_print_error_cr("NULL node!");
			return false;
		}

		//CScope *_scope =AST_SCOPE_INFO(idxAstNode);

		if(_node->node_type == NODE_TYPE::BODY_NODE ){
			pushFunction(_node->idxAstNode,sf);
			// reset current pointer ...
			{ // main node ?
				for(unsigned i = GET_CURSOR_COMPILE; i < _node->children.size(); i++){

					if(!ast2asm_Recursive(_node->children[i], m_treescope)){
						return false;
					}
				}
			}
			popFunction();
			return true;
		}
		else{
			zs_print_error_cr("Body node expected");
		}

		return false;
	}

	CCompiler::~CCompiler(){

	}
}
