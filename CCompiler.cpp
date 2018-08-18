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

	void  writeErrorMsg(const char *filename, int line, const  char  *string_text, ...);

	tDefOperator CCompiler::def_operator[MAX_OPERATORS];
	map<string, CCompiler::tInfoConstantValue *> * CCompiler::constant_pool=NULL;

	CCompiler *CCompiler::m_compiler = NULL;

	string 		CCompiler::getSymbolNameFromSymbolRef(const string & ref_symbol){
		string symbol_var="";

		char * start_ptr=strchr((char *)ref_symbol.c_str(),'_');

		if(start_ptr==NULL){
			THROW_RUNTIME_ERROR("cannot get '_'");
			return symbol_var;
		}

		start_ptr++;

		while(*start_ptr !=0){
			symbol_var+=*start_ptr++;
		}

		return symbol_var;
	}

	string CCompiler::makeSymbolRef(const string & symbol_var, int idxScope){
		return string("@lnk"+CZetScriptUtils::intToString(idxScope)+"_"+symbol_var);
	}

	int	CCompiler::getIdxScopeFromSymbolRef(const string & symbol_ref){

		short idxScope=-1;
		char buffer[30]={0};

		if((strncmp("@lnk",symbol_ref.c_str(),4))!=0){
			THROW_RUNTIME_ERROR("symbol doesn't start with @lnk");
			return -1;
		}

		char *start=(char *)symbol_ref.c_str()+4;
		char *end=strchr(start,'_');

		strncpy(buffer,start,end-start);

		int *i=CZetScriptUtils::ParseInteger(buffer);

		if(i==NULL){
			THROW_RUNTIME_ERROR("%s is not valid integer",buffer);
			return -1;
		}

		idxScope=*i;
		delete i;

		return idxScope;
	}

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
			THROW_RUNTIME_ERROR("internal:constant %s already exist",const_name.c_str());
		}

		return info_ptr;
	}

	int CCompiler::addLocalVarSymbol(const string & var_name,short idxAstNode){

		int idxVar;


		if((idxVar=getIdxLocalVarSymbol(var_name,idxAstNode,false))==ZS_UNDEFINED_IDX){
			tInfoVariableSymbol info_symbol;
			PASTNode ast = AST_NODE(idxAstNode);

			info_symbol.idxAstNode = idxAstNode;
			info_symbol.symbol_ref = makeSymbolRef(var_name,ast->idxScope);
			int n_element=this->m_currentFunctionInfo->function_info_object->object_info.local_symbols.m_registeredVariable.size();

			this->m_currentFunctionInfo->function_info_object->object_info.local_symbols.m_registeredVariable.push_back(info_symbol);

			// init stack variable ...
			if(this->m_currentFunctionInfo->function_info_object->object_info.symbol_info.idxScriptClass == IDX_CLASS_MAIN && this->m_currentFunctionInfo->function_info_object->object_info.idxScriptFunctionObject == 0){ // initialize new global var initialized on MainFuntion ...
				CURRENT_VM->iniStackVar(n_element,{STK_PROPERTY_TYPE_UNDEFINED,0,0});
			}


			return n_element;//this->m_currentFunctionInfo->function_info_object->object_info.local_symbols.m_registeredVariable.size()-1;
		} // else already added so we refer the same var.

		writeErrorMsg(GET_AST_FILENAME_LINE(idxAstNode),"symbol \"%s\" already defined. (link issue)",var_name.c_str());
		return ZS_UNDEFINED_IDX;
	}

	bool CCompiler::localVarSymbolExists(const string & name,short  idxAstNode){

		return getIdxLocalVarSymbol(name,idxAstNode, false) != ZS_UNDEFINED_IDX;
	}

	int  CCompiler::getIdxLocalVarSymbol(const string & name,short idxAstNode, bool print_msg){
		// name comes from same astnode...

		string  symbol_ref = makeSymbolRef(name,AST_NODE(idxAstNode)->idxScope);

		for(unsigned i = 0; i < this->m_currentFunctionInfo->function_info_object->object_info.local_symbols.m_registeredVariable.size(); i++){
			if(this->m_currentFunctionInfo->function_info_object->object_info.local_symbols.m_registeredVariable[i].symbol_ref == symbol_ref ){
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
				writeErrorMsg(GET_AST_FILENAME_LINE(idxAstNode),"expected args node");
				return NULL;
			}

			int n_params= ast_args->children.size();

			short idxScope=ast_node->idxScope;



			tScopeVar *irv = SCOPE_NODE(idxScope)->getInfoRegisteredSymbol(function_name,n_params,false);
			if(irv != NULL){

				CScriptFunctionObject *info_symbol = NEW_SCRIPT_FUNCTION_OBJECT;

				CScope *scope=SCOPE_NODE(ast_node->idxScope);
				PASTNode ast_node_root=AST_NODE(scope->getIdxBaseAstNode());

				info_symbol->object_info.symbol_info.idxAstNode = irv->idxAstNode;
				info_symbol->object_info.symbol_info.idxScriptClass = ast_node_root==NULL?IDX_CLASS_MAIN:CScriptClass::getIdxScriptClass(ast_node_root->symbol_value);
				//info_symbol.object_info.symbol_info.idxScopeVar = irv->idxScopeVar;
				info_symbol->object_info.symbol_info.symbol_ref = makeSymbolRef(name,ast_node->idxScope);

				this->m_currentFunctionInfo->function_info_object->object_info.local_symbols.vec_idx_registeredFunction.push_back(info_symbol->object_info.idxScriptFunctionObject);

				return info_symbol;
			}
			else{
				writeErrorMsg(GET_AST_FILENAME_LINE(idxAstNode),"No function symbol \"%s\" with %i args is defined",function_name.c_str(), n_params);
			}

		}
		else{
			writeErrorMsg(GET_AST_FILENAME_LINE(idxAstNode),"No function symbol \"%s\" is defined",function_name.c_str());
		}
		return NULL;
	}

	bool CCompiler::functionSymbolExists(const string & name, short idxAstNode){

		unsigned int scope_type;
		return getIdxFunctionObject(name,idxAstNode,scope_type,false) != ZS_UNDEFINED_IDX;
	}

	int  CCompiler::getIdxFunctionObject(const string & name,short idxAstNode, unsigned int & scope_type, bool print_msg){

		// expects symbol name from astnode...

		PASTNode ast_node=AST_NODE(idxAstNode);
		PASTNode ast_args = AST_NODE(ast_node->children[0]);
		if(ast_args->node_type != NODE_TYPE::ARGS_DECL_NODE){
			writeErrorMsg(GET_AST_FILENAME_LINE(idxAstNode),"expected args node");
			return ZS_UNDEFINED_IDX;
		}

		unsigned n_args = ast_args->children.size();


		// get name_ref
		string symbol_ref = makeSymbolRef(name,ast_node->idxScope);

		tScopeVar *irv=SCOPE_NODE(AST_NODE(idxAstNode)->idxScope)->getInfoRegisteredSymbol(name,(int)n_args,false);


		scope_type = INS_PROPERTY_LOCAL_SCOPE;
		if(irv != NULL){

			PASTNode ast = AST_NODE(irv->idxAstNode);

			if((ast != NULL) && (AST_NODE(idxAstNode)->idxScope == ast->idxScope)){
				for(unsigned i = 0; i < this->m_currentFunctionInfo->function_info_object->object_info.local_symbols.vec_idx_registeredFunction.size(); i++){
					CScriptFunctionObject *sfo=GET_SCRIPT_FUNCTION_OBJECT(m_currentFunctionInfo->function_info_object->object_info.local_symbols.vec_idx_registeredFunction[i]);
					if(sfo->object_info.symbol_info.symbol_ref == symbol_ref  && sfo->m_arg.size()==n_args){
						return i;
					}
				}
			}
			else{ //global

				scope_type = 0;//INST_PROPERTY_GLOBAL_SCOPE;
				CScriptFunctionObject *main_function = GET_SCRIPT_FUNCTION_OBJECT(MAIN_SCRIPT_FUNCTION_IDX);


				for(unsigned i = 0; i < main_function->object_info.local_symbols.vec_idx_registeredFunction.size(); i++){
					CScriptFunctionObject *sfo=GET_SCRIPT_FUNCTION_OBJECT(main_function->object_info.local_symbols.vec_idx_registeredFunction[i]);
					if(sfo->object_info.symbol_info.symbol_ref == symbol_ref  && sfo->m_arg.size()==n_args ){
						return i;
					}
				}
			}
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


		def_operator[VGET]={"VGET",VGET,1}; // vector access after each index is processed...

		def_operator[DECL_VEC]={"DECL_VEC",DECL_VEC,1}; // Vector object (CREATE)

		def_operator[VPUSH]={"VPUSH",VPUSH,1}; // Value push for vector
		def_operator[RET]={"RET",RET,1}; // Value pop for vector

		def_operator[NEW]={"NEW",NEW,1}; // New object (CREATE)
		def_operator[DELETE_OP]={"DELETE_OP",DELETE_OP,1};

		def_operator[POP_SCOPE]={"POP_SCOPE",POP_SCOPE,0}; // New object (CREATE)
		def_operator[PUSH_SCOPE]={"PUSH_SCOPE",PUSH_SCOPE,1}; // New object (CREATE)
		def_operator[PUSH_ATTR]={"PUSH_ATTR",PUSH_ATTR,2}; // New object (CREATE)
		def_operator[DECL_STRUCT]={"DECL_STRUCT",DECL_STRUCT,0}; // New object (CREATE)

		ptr_continueInstructionsScope = NULL;
		ptr_breakInstructionsScope = NULL;
	}

	//------------------------------------------------------------------------------------------------------------------
	//
	// COMPILE COMPILER MANAGEMENT
	//
	int CCompiler::getCurrentInstructionIndex(){
		//tInfoStatementOpCompiler *ptr_current_statement_op = &(m_currentFunctionInfo->stament)[m_currentFunctionInfo->stament.size()-1];
		return m_currentFunctionInfo->asm_op.size()-1;
	}

	bool CCompiler::isFunctionNode(short idxAstNode){

		PASTNode node = AST_NODE(idxAstNode);

		if(node==NULL){
			THROW_RUNTIME_ERROR("Node is NULL");
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

		//tInfoStatementOpCompiler *ptr_current_statement_op = &(this->m_currentFunctionInfo->stament)[this->m_currentFunctionInfo->stament.size()-1];

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
		m_currentFunctionInfo->asm_op.push_back(asm_op);
	}

	bool CCompiler::insertLoadValueInstruction(PASTNode _node, CScope * _lc, tInfoAsmOpCompiler **iao_result){

		PASTNode _parent=AST_NODE(_node->idxAstParent);
		string v = _node->symbol_value;

		unsigned int pre_post_operator_type =0;//INS_PROPERTY_UNKNOW_PRE_POST_OPERATOR;
		//tInfoStatementOpCompiler *ptr_current_statement_op = &(this->m_currentFunctionInfo->stament)[this->m_currentFunctionInfo->stament.size()-1];
		void *const_obj;
		void *obj;
		CCompiler::tInfoConstantValue *get_obj;
		unsigned short type=STK_PROPERTY_TYPE_SCRIPTVAR;
		LOAD_TYPE load_type=LOAD_TYPE_NOT_DEFINED;
		unsigned int scope_type=0;//INS_PROPERTY_UNKNOWN_SCOPE;
		bool is_constant = true;

		if((_parent->operator_info == INSTANCEOF_PUNCTUATOR) && (_parent->children[1] == _node->idxAstNode)){ // we search for class idx

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
					writeErrorMsg(GET_AST_FILENAME_LINE(_node->idxAstNode)," in instenceof operator  class \"%s\" is not registered",v.c_str());
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

			}else if((const_obj=CZetScriptUtils::ParseInteger(v))!=NULL){
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
			else if((const_obj=CZetScriptUtils::ParseFloat(v))!=NULL){
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
			else if((const_obj=CZetScriptUtils::ParseBoolean(v))!=NULL){

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
				else if(_node->symbol_value == "super"){
						scope_type=INS_PROPERTY_SUPER_SCOPE;
				}else if(_node->symbol_value == "this"){
						scope_type=INS_PROPERTY_THIS_SCOPE;
				}else{

						// if not function then is var or arg node ?
						// first we find the list of argments
						if((idx_local_var = getIdxArgument(_node->symbol_value))!=ZS_UNDEFINED_IDX){
							load_type=LOAD_TYPE_ARGUMENT;
						}
						else{ // ... if not argument finally, we deduce that the value is a local/global symbol... check whether it exist in the current scope...

								if(_node->node_type == SYMBOL_NODE){
									if(_lc!=NULL){
										if(!_lc->existRegisteredSymbol(symbol_name,NO_PARAMS_SYMBOL_ONLY)){ // check local
											if(!SCOPE_NODE(IDX_GLOBAL_SCOPE)->existRegisteredSymbol(symbol_name,NO_PARAMS_SYMBOL_ONLY)){ // check global
												writeErrorMsg(GET_AST_FILENAME_LINE(_node->idxAstNode),"Variable \"%s\" not defined",symbol_name.c_str());
												return false;
											}
										}
									}
									else{
										THROW_RUNTIME_ERROR("scope null");
										return false;
									}
								}
						}
					}
					obj = (CScriptVariable *)idx_local_var;
			}

			if((pre_post_operator_type !=0 && GET_INS_PROPERTY_PRE_POST_OP(pre_post_operator_type) !=INS_PROPERTY_PRE_NEG) &&
				is_constant){
				writeErrorMsg(GET_AST_FILENAME_LINE(_node->idxAstNode),"operation \"%s\" not allowed for constants ",CASTNode::defined_operator_punctuator[_node->pre_post_operator_info].str);
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
		m_currentFunctionInfo->asm_op.push_back(asm_op);

		if(iao_result != NULL){
			*iao_result=asm_op;
		}
		return true;
	}

	bool CCompiler::insertMovVarInstruction(short idxAstNode,int left_index, int right_index){

		//tInfoStatementOpCompiler *ptr_current_statement_op = &(this->m_currentFunctionInfo->stament)[this->m_currentFunctionInfo->stament.size()-1];
		tInfoAsmOpCompiler * left_asm_op = m_currentFunctionInfo->asm_op[left_index];

		// check whether left operant is object...
		if(left_asm_op->var_type != STK_PROPERTY_TYPE_SCRIPTVAR){
			writeErrorMsg(GET_AST_FILENAME_LINE(left_asm_op->idxAstNode)," left operand must be l-value for '=' operator");
			return false;
		}

		tInfoAsmOpCompiler *asm_op = new tInfoAsmOpCompiler();
		asm_op->index_op1 = ZS_UNDEFINED_IDX;//&(this->m_currentFunctionInfo->stament[dest_statment]);
		asm_op->index_op2 =  ZS_UNDEFINED_IDX;
		asm_op->idxAstNode = idxAstNode;
		//asm_op->symbol_name="";
		asm_op->operator_type=ASM_OPERATOR::STORE;

		m_currentFunctionInfo->asm_op.push_back(asm_op);
		return true;
	}

	void CCompiler::insertNot(short idxAstNode){
		//tInfoStatementOpCompiler *ptr_current_statement_op = &this->m_currentFunctionInfo->stament[this->m_currentFunctionInfo->stament.size()-1];
		tInfoAsmOpCompiler *asm_op = new tInfoAsmOpCompiler();
		asm_op->idxAstNode = idxAstNode;
		asm_op->operator_type=ASM_OPERATOR::NOT;
		m_currentFunctionInfo->asm_op.push_back(asm_op);
	}

	void CCompiler::insertNeg(short idxAstNode){
	//	tInfoStatementOpCompiler *ptr_current_statement_op = &this->m_currentFunctionInfo->stament[this->m_currentFunctionInfo->stament.size()-1];
		tInfoAsmOpCompiler *asm_op = new tInfoAsmOpCompiler();
		asm_op->idxAstNode = idxAstNode;
		asm_op->operator_type=ASM_OPERATOR::NEG;
		m_currentFunctionInfo->asm_op.push_back(asm_op);
	}

	CCompiler::tInfoAsmOpCompiler * CCompiler::insert_JMP_Instruction(short idxAstNode,int jmp_statement, char instruction_index){

		//tInfoStatementOpCompiler *ptr_current_statement_op = &this->m_currentFunctionInfo->stament[this->m_currentFunctionInfo->stament.size()-1];
		tInfoAsmOpCompiler *asm_op = new tInfoAsmOpCompiler();
		asm_op->index_op1 = instruction_index;
		asm_op->index_op2 = jmp_statement;//&(this->m_currentFunctionInfo->stament[dest_statment]);
		asm_op->operator_type=ASM_OPERATOR::JMP;
		asm_op->idxAstNode = idxAstNode;
		m_currentFunctionInfo->asm_op.push_back(asm_op);
		return asm_op;
	}

	CCompiler::tInfoAsmOpCompiler * CCompiler::insert_JNT_Instruction(short idxAstNode,int jmp_statement, char instruction_index){

		//tInfoStatementOpCompiler *ptr_current_statement_op = &this->m_currentFunctionInfo->stament[this->m_currentFunctionInfo->stament.size()-1];
		tInfoAsmOpCompiler *asm_op = new tInfoAsmOpCompiler();
		asm_op->index_op1 = instruction_index;
		asm_op->index_op2 = jmp_statement;//&(this->m_currentFunctionInfo->stament[dest_statment]);
		asm_op->operator_type=ASM_OPERATOR::JNT;
		asm_op->idxAstNode = idxAstNode;
		m_currentFunctionInfo->asm_op.push_back(asm_op);

		return asm_op;
	}

	CCompiler::tInfoAsmOpCompiler * CCompiler::insert_JT_Instruction(short idxAstNode,int jmp_statement, char instruction_index){

		//tInfoStatementOpCompiler *ptr_current_statement_op = &this->m_currentFunctionInfo->stament[this->m_currentFunctionInfo->stament.size()-1];
		tInfoAsmOpCompiler *asm_op = new tInfoAsmOpCompiler();
		asm_op->operator_type=ASM_OPERATOR::JT;
		asm_op->index_op1 = instruction_index;
		asm_op->index_op2 = jmp_statement;
		asm_op->idxAstNode = idxAstNode;
		m_currentFunctionInfo->asm_op.push_back(asm_op);
		return asm_op;
	}

	void CCompiler::insert_CreateArrayObject_Instruction(short idxAstNode){

		tInfoAsmOpCompiler *asm_op = new tInfoAsmOpCompiler();

		asm_op->operator_type=ASM_OPERATOR::DECL_VEC;
		asm_op->var_type=STK_PROPERTY_TYPE_SCRIPTVAR;
		asm_op->idxAstNode = idxAstNode;
		m_currentFunctionInfo->asm_op.push_back(asm_op);
	}

	void CCompiler::insert_ArrayAccess_Instruction(int vec_object, int index_instrucction, short idxAstNode){

		tInfoAsmOpCompiler *asm_op = new tInfoAsmOpCompiler();
		asm_op->index_op1 = vec_object;//&(this->m_currentFunctionInfo->stament[dest_statment]);
		asm_op->index_op2 = index_instrucction;//&(this->m_currentFunctionInfo->stament[dest_statment]);
		asm_op->operator_type=ASM_OPERATOR::VGET;
		asm_op->idxAstNode = idxAstNode;
		asm_op->var_type = STK_PROPERTY_TYPE_SCRIPTVAR;
		m_currentFunctionInfo->asm_op.push_back(asm_op);
	}

	void CCompiler::insert_CallFunction_Instruction(short idxAstNode,int  index_call,int  index_object){

		tInfoAsmOpCompiler *asm_op = new tInfoAsmOpCompiler();

		asm_op->index_op1 = index_call;//&(this->m_currentFunctionInfo->stament[dest_statment]);
		asm_op->index_op2 = index_object;//&(this->m_currentFunctionInfo->stament[dest_statment]);

		asm_op->idxAstNode = idxAstNode;
		asm_op->operator_type=ASM_OPERATOR::CALL;

		m_currentFunctionInfo->asm_op.push_back(asm_op);
	}

	void CCompiler::insertRet(short idxAstNode){

		tInfoAsmOpCompiler *asm_op = new tInfoAsmOpCompiler();


		tInfoAsmOpCompiler *last_asm = m_currentFunctionInfo->asm_op[m_currentFunctionInfo->asm_op.size()-1]; // get last operator
		if(last_asm->operator_type == ASM_OPERATOR::CALL){
			last_asm->runtime_prop |=INS_PROPERTY_DIRECT_CALL_RETURN;
		}

		asm_op->operator_type=ASM_OPERATOR::RET;
		asm_op->idxAstNode = idxAstNode;

		m_currentFunctionInfo->asm_op.push_back(asm_op);
	}

	void CCompiler::insert_ArrayObject_PushValueInstruction(short idxAstNode,int ref_vec_object_index,int index_instruction_to_push){

		tInfoAsmOpCompiler *asm_op = new tInfoAsmOpCompiler();
		asm_op->index_op1=ref_vec_object_index;
		asm_op->idxAstNode = idxAstNode;
		asm_op->index_op2=index_instruction_to_push;
		if(index_instruction_to_push == ZS_UNDEFINED_IDX){
			asm_op->index_op2=CCompiler::getCurrentInstructionIndex();
		}
		asm_op->operator_type=ASM_OPERATOR::VPUSH;
		m_currentFunctionInfo->asm_op.push_back(asm_op);
	}

	//--------------------------------------------------------------------------------------------------------------------------------------------------------------------

	bool CCompiler::insert_NewObject_Instruction(short idxAstNode, const string & class_name){

		tInfoAsmOpCompiler *asm_op = new tInfoAsmOpCompiler();
		if((asm_op->index_op1 = CScriptClass::getIdxScriptClass(class_name))==ZS_UNDEFINED_IDX){//&(this->m_currentFunctionInfo->stament[dest_statment]);
			writeErrorMsg(GET_AST_FILENAME_LINE(idxAstNode),"class \"%s\" is not registered", class_name.c_str());
			return false;
		}
		asm_op->operator_type=ASM_OPERATOR::NEW;
		asm_op->idxAstNode = idxAstNode;

		m_currentFunctionInfo->asm_op.push_back(asm_op);
		return true;
	}

	bool CCompiler::insert_DeleteObject_Instruction(short idxAstNode){

		tInfoAsmOpCompiler *asm_op = new tInfoAsmOpCompiler();

		asm_op->operator_type=ASM_OPERATOR::DELETE_OP;
		asm_op->idxAstNode = idxAstNode;
		m_currentFunctionInfo->asm_op.push_back(asm_op);

		return true;
	}

	bool CCompiler::insertPushScopeInstruction(short idxAstNode,int scope_idx,char save_breakpoint){
		if(scope_idx==ZS_UNDEFINED_IDX){
			THROW_RUNTIME_ERROR("Internal error undefined scope");
			return false;
		}

		tInfoAsmOpCompiler *asm_op = new tInfoAsmOpCompiler();

		asm_op->index_op1 = save_breakpoint;
		asm_op->index_op2 = scope_idx; // index from object cached node ?
		asm_op->operator_type=ASM_OPERATOR::PUSH_SCOPE;
		asm_op->idxAstNode = idxAstNode;


		m_currentFunctionInfo->asm_op.push_back(asm_op);

		return true;
	}

	void CCompiler::insertPopScopeInstruction(short idxAstNode, char restore_breakpoint){

		tInfoAsmOpCompiler *asm_op = new tInfoAsmOpCompiler();
		asm_op->operator_type=ASM_OPERATOR::POP_SCOPE;
		asm_op->idxAstNode = idxAstNode;

		asm_op->index_op1 = restore_breakpoint;

		m_currentFunctionInfo->asm_op.push_back(asm_op);
	}

	void CCompiler::insert_DeclStruct_Instruction(short idxAstNode){

		tInfoAsmOpCompiler *asm_op = new tInfoAsmOpCompiler();
		asm_op->operator_type=ASM_OPERATOR::DECL_STRUCT;
		asm_op->idxAstNode = idxAstNode;

		m_currentFunctionInfo->asm_op.push_back(asm_op);
	}

	void CCompiler::insert_PushAttribute_Instruction(short idxAstNode){

		tInfoAsmOpCompiler *asm_op = new tInfoAsmOpCompiler();
		asm_op->operator_type=ASM_OPERATOR::PUSH_ATTR;
		asm_op->idxAstNode = idxAstNode;
		m_currentFunctionInfo->asm_op.push_back(asm_op);
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
			THROW_RUNTIME_ERROR("%s Not implemented",CASTNode::defined_operator_punctuator[op].str );
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
		case INSTANCEOF_PUNCTUATOR:
			return ASM_OPERATOR::INSTANCEOF;
		}
		return INVALID_OP;
	}


	CCompiler::tInfoAsmOpCompiler * CCompiler::getLastInsertedInfoAsmOpCompiler(){
		return m_currentFunctionInfo->asm_op[m_currentFunctionInfo->asm_op.size()-1];
	}


	bool CCompiler::insertOperatorInstruction(PUNCTUATOR_TYPE op,short idxAstNode,  string & error_str){
		PASTNode _node=AST_NODE( idxAstNode);
		tInfoAsmOpCompiler *iao=NULL;

		// special cases ...
		switch(op){

			case ADD_ASSIGN_PUNCTUATOR: // a+b

				iao = new tInfoAsmOpCompiler();
				iao->operator_type = ASM_OPERATOR::ADD;
				iao->runtime_prop |= STK_PROPERTY_READ_TWO_POP_ONE;
				iao->idxAstNode=_node->idxAstNode;
				m_currentFunctionInfo->asm_op.push_back(iao);

				op = ASSIGN_PUNCTUATOR;
				break;
			case SUB_ASSIGN_PUNCTUATOR: // a + (-b)

				iao = new tInfoAsmOpCompiler();
				iao->operator_type = ASM_OPERATOR::NEG;

				iao->idxAstNode=_node->idxAstNode;
				m_currentFunctionInfo->asm_op.push_back(iao);

				iao = new tInfoAsmOpCompiler();
				iao->operator_type = ASM_OPERATOR::ADD;
				iao->runtime_prop |= STK_PROPERTY_READ_TWO_POP_ONE;
				iao->idxAstNode=_node->idxAstNode;
				m_currentFunctionInfo->asm_op.push_back(iao);

				op = ASSIGN_PUNCTUATOR;
				break;
			case MUL_ASSIGN_PUNCTUATOR: // a*b

				iao = new tInfoAsmOpCompiler();
				iao->operator_type = ASM_OPERATOR::MUL;
				iao->runtime_prop |= STK_PROPERTY_READ_TWO_POP_ONE;

				iao->idxAstNode=_node->idxAstNode;
				m_currentFunctionInfo->asm_op.push_back(iao);

				op = ASSIGN_PUNCTUATOR;
				break;
			case DIV_ASSIGN_PUNCTUATOR: // a/b

				iao = new tInfoAsmOpCompiler();
				iao->operator_type = ASM_OPERATOR::DIV;
				iao->runtime_prop |= STK_PROPERTY_READ_TWO_POP_ONE;

				iao->idxAstNode=_node->idxAstNode;
				m_currentFunctionInfo->asm_op.push_back(iao);

				op = ASSIGN_PUNCTUATOR;
				break;
			case MOD_ASSIGN_PUNCTUATOR: // a % b

				iao = new tInfoAsmOpCompiler();
				iao->operator_type = ASM_OPERATOR::MOD;
				iao->runtime_prop |= STK_PROPERTY_READ_TWO_POP_ONE;

				iao->idxAstNode=_node->idxAstNode;
				m_currentFunctionInfo->asm_op.push_back(iao);


				op = ASSIGN_PUNCTUATOR;
				break;
			default:
				break;

		}


		if(op == PUNCTUATOR_TYPE::FIELD_PUNCTUATOR){ // trivial access...
			return true;
		}
		ASM_OPERATOR asm_op;
		if((asm_op= puntuator2instruction(op))!=INVALID_OP){
			iao = new tInfoAsmOpCompiler();
			iao->operator_type = asm_op;

			if(_node!=NULL){
				iao->idxAstNode=_node->idxAstNode;
			}
			m_currentFunctionInfo->asm_op.push_back(iao);
			return true;
		}
		return false;
	}

	//------------------------------------------------------------------------------------------------------------------
	//
	// COMPILE EXPRESSIONS AND GENERATE ITS ASM
	//

	bool CCompiler::gacExpression_FunctionOrArrayAccess(PASTNode _node_access, CScope *_lc){


		bool  function_access = false;
		bool array_access = false;

		PASTNode eval_node_sp = _node_access;
		if(_node_access->node_type == CALLING_OBJECT_NODE){
			if(_node_access->children.size() > 0){
				eval_node_sp = AST_NODE(_node_access->children[0]);

				function_access = eval_node_sp->node_type == FUNCTION_OBJECT_NODE || eval_node_sp->node_type == FUNCTION_REF_NODE;
				array_access = eval_node_sp->node_type == ARRAY_OBJECT_NODE || eval_node_sp->node_type == ARRAY_REF_NODE;


			}else {
				THROW_RUNTIME_ERROR("internal error.Calling object should have at least 1 children");
				return false;
			}
		}

		if(array_access){
			return gacExpression_ArrayAccess(_node_access, _lc);
		}else if(function_access){
			return gacExpression_FunctionAccess(_node_access, _lc);
		}

		THROW_RUNTIME_ERROR("internal error. Expected function or array access");
		return false;
	}

	bool CCompiler::gacExpression_ArrayAccess(PASTNode _node, CScope *_lc){

		if(_node->node_type != CALLING_OBJECT_NODE ){THROW_RUNTIME_ERROR("node is not CALLING_OBJECT_NODE type or null");return false;}
		if(!(_node->children.size()==2 || _node->children.size()==3)) {THROW_RUNTIME_ERROR("Array access should have 2 children");return false;}


		PASTNode node_0=AST_NODE(_node->children[0]),
				 node_1=AST_NODE(_node->children[1]),
				 node_2=NULL;

		if(_node->children.size()==3){
			node_2 = AST_NODE(_node->children[2]);
		}

		if(node_0->node_type != ARRAY_REF_NODE && node_0->node_type != ARRAY_OBJECT_NODE ){THROW_RUNTIME_ERROR("Node is not ARRAY_OBJECT type"); return false;}
		if(node_1->node_type != ARRAY_ACCESS_NODE || node_1->children.size() == 0){THROW_RUNTIME_ERROR("Array has no index nodes "); return false;}

		int vec=0;

		if(node_0->node_type == ARRAY_OBJECT_NODE){ // must first create the object ...
			if(!gacExpression_ArrayObject(node_0,_lc)){
				return false;
			}
		}else{
			if(node_0->symbol_value != "--"){ // starts with symbol ...
				if(!insertLoadValueInstruction(node_0,  _lc)){
					return false;
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
					if(!(gacExpression(AST_NODE(AST_NODE(array_acces->children [k])->children[0]), _lc))){
						return false;
					}
					// insert vector access instruction ...
					insert_ArrayAccess_Instruction(vec,CCompiler::getCurrentInstructionIndex(), array_acces->children [k]);
					vec = getCurrentInstructionIndex();

				}else{
					THROW_RUNTIME_ERROR("Expected 1 children");
					return false;
				}
			}else{
				THROW_RUNTIME_ERROR("Node not ARRAY_INDEX_NODE");
				return false;
			}
		}

		if(_node->pre_post_operator_info != PUNCTUATOR_TYPE::UNKNOWN_PUNCTUATOR){ // there's pre/post increment...
			// get post/inc
			tInfoAsmOpCompiler *asm_op = m_currentFunctionInfo->asm_op[m_currentFunctionInfo->asm_op.size()-1];
			asm_op->pre_post_op_type=post_operator2instruction_property(_node->pre_post_operator_info);
		}

		if(_node->children.size()==3){ // array or function access ...

			return gacExpression_FunctionOrArrayAccess(node_2, _lc);
		}
		// return last instruction where was modified
		return true;
	}



	bool CCompiler::gacExpression_ArrayObject(PASTNode _node, CScope *_lc){

		if(_node->node_type != ARRAY_OBJECT_NODE ){THROW_RUNTIME_ERROR("node is not ARRAY_OBJECT_NODE type or null");return false;}

		// 1. create object ...
		insert_CreateArrayObject_Instruction(_node->idxAstNode);
		int index_created_vec = CCompiler::getCurrentInstructionIndex();

		// 2. evaluate expressions if any
		for(unsigned j=0; j < _node->children.size(); j++){

			// check whether is expression node...
			if(!gacExpression(AST_NODE(_node->children[j]), _lc)){
				return false;
			}
			insert_ArrayObject_PushValueInstruction(_node->idxAstNode,index_created_vec);
		}
		return true;//index_created_vec;//CCompiler::getCurrentInstructionIndex();
	}

	bool CCompiler::gacExpression_FunctionObject(PASTNode _node, CScope *_lc){

		if(_node->node_type != FUNCTION_OBJECT_NODE ){THROW_RUNTIME_ERROR("node is not FUNCTION_OBJECT_NODE type or null");return false;}
		if(_node->children.size()!=2) {THROW_RUNTIME_ERROR("Array access should have 2 children");return false;}

		CScriptFunctionObject * script_function=NULL;

		// 1. insert load reference created object ...
		if(functionSymbolExists(_node->symbol_value, _node->idxAstNode)){
				writeErrorMsg(GET_AST_FILENAME_LINE(_node->idxAstNode),"Function \"%s\" already defined",_node->symbol_value.c_str());
				return false;
		}

		if((script_function=addLocalFunctionSymbol(_node->symbol_value,_node->idxAstNode)) == NULL){
			return false;
		}

		// compiles anonymous function ...
		if(!gacFunctionOrOperator(_node,_lc, script_function)){
			return false;
		}

		return insertLoadValueInstruction(_node, _lc);
	}

	bool CCompiler::gacExpression_FunctionAccess(PASTNode _node, CScope *_lc){

		tInfoAsmOpCompiler *iao_call=NULL;

		if(_node == NULL) {THROW_RUNTIME_ERROR("NULL node");return false;}
		if(_node->node_type != CALLING_OBJECT_NODE ){THROW_RUNTIME_ERROR("node is not CALLING_OBJECT_NODE type or null");return false;}
		if(!(_node->children.size()==2 || _node->children.size()==3)) {THROW_RUNTIME_ERROR("Array access should have 2 or 3 children");return false;}

		PASTNode node_0=AST_NODE(_node->children[0]),
				 node_1=AST_NODE(_node->children[1]),
				 node_2=NULL;

		if(_node->children.size()==3){
			node_2 = AST_NODE(_node->children[2]);
		}

		if(node_0->node_type != FUNCTION_REF_NODE && node_0->node_type != FUNCTION_OBJECT_NODE){THROW_RUNTIME_ERROR("Node is not FUNCTION_OBJECT_NODE type"); return false;}
		if(node_1->node_type != ARGS_PASS_NODE){THROW_RUNTIME_ERROR("Function has no index nodes "); return false;}

		// load function ...
		if(node_0->symbol_value != "--"){ // starts with symbol ...
			if(!insertLoadValueInstruction(node_0,_lc,&iao_call)) {
				return false;
			}
			iao_call->runtime_prop|=INS_CHECK_IS_FUNCTION;
		}

		int call_index = getCurrentInstructionIndex();

		// 1. insert push to pass values to all args ...
		PASTNode function_args =node_1;

		if(function_args->children.size() > 0){
			for(unsigned k = 0; k < function_args->children.size(); k++){

				// check whether is expression node...
				if(!gacExpression(AST_NODE(function_args->children[k]), _lc)){
					return false;
				}
			}
		}

		// 2. insert call instruction itself.
		insert_CallFunction_Instruction(_node->idxAstNode,call_index);

		if(_node->children.size()==3){ // array or function access ...

			return gacExpression_FunctionOrArrayAccess(node_2, _lc);
		}

		return true;
	}

	bool CCompiler::gacExpression_StructAttribute(PASTNode _node, CScope *_lc){

		if(_node->node_type != EXPRESSION_NODE ){THROW_RUNTIME_ERROR("node is not EXPRESSION_NODE type or null");return false;}

		int index_attr = getCurrentInstructionIndex()+1;

		// 1st evalualte expression...
		if(!gacExpression(_node, _lc)){
			return ZS_UNDEFINED_IDX;
		}

		// 2nd insert strign constant ...
		insertStringConstantValueInstruction(_node->idxAstNode,_node->symbol_value);

		if(!(
			 m_currentFunctionInfo->asm_op[index_attr]->operator_type == ASM_OPERATOR::DECL_VEC
		  || m_currentFunctionInfo->asm_op[index_attr]->operator_type == ASM_OPERATOR::NEW
		  || m_currentFunctionInfo->asm_op[index_attr]->operator_type == ASM_OPERATOR::DECL_STRUCT
		)){
			index_attr = getCurrentInstructionIndex()-1;
		}

		// 3rd insert push attribute...
		insert_PushAttribute_Instruction(_node->idxAstNode);

		return true;
	}

	bool CCompiler::gacExpression_Struct(PASTNode _node, CScope *_lc){

		if(_node == NULL) {THROW_RUNTIME_ERROR("NULL node");return false;}
		if(_node->node_type != STRUCT_NODE ){THROW_RUNTIME_ERROR("node is not STRUCT_NODE type or null");return false;}

		insert_DeclStruct_Instruction(_node->idxAstNode);


		for(unsigned i = 0; i < _node->children.size(); i++){
			// evaluate attribute
			if(!gacExpression_StructAttribute(AST_NODE(_node->children[i]), _lc)){
				return false;
			}
		}
		return true;
	}

	bool CCompiler::gacExpression_Recursive(PASTNode _node, CScope *_lc){

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

		bool this_access_scope=false;

		if(_node->node_type == PUNCTUATOR_NODE){

			if((_node->children.size()==2 && AST_NODE(_node->children[0])->symbol_value=="this")){
				if(_node->operator_info == FIELD_PUNCTUATOR){

					this_access_scope =true;

				}
				else if(_node->operator_info == ASSIGN_PUNCTUATOR){

					writeErrorMsg(GET_AST_FILENAME_LINE(_node->idxAstNode),"\"this\" is not assignable ");
					return ZS_UNDEFINED_IDX;
				}
			}
		}

		if(this_access_scope)// only take care left children...
		{
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
					THROW_RUNTIME_ERROR("Calling object should have at least 1 children");
					return false;
				}
			}

			if(special_node ){
					if(array_access){
						if((!gacExpression_ArrayAccess(_node, _lc))){
													return false;
						}
					}else if(function_access){
						if((!gacExpression_FunctionAccess(_node, _lc))){
							return false;
						}
					}
					else{
						switch(eval_node_sp->node_type){
						case ARRAY_OBJECT_NODE: // should have 1 children
							if(!gacExpression_ArrayObject(_node, _lc)){
								return false;
							}
							break;

						case FUNCTION_OBJECT_NODE: // should have 1 children
							if(!gacExpression_FunctionObject(_node, _lc)){
								return false;
							}
							break;

						case NEW_OBJECT_NODE:
							if(!(gacNew(_node, _lc))){
								return false;
							}
							break;
						case STRUCT_NODE:
							if((!gacExpression_Struct(_node, _lc))){
								return false;
							}
							break;
						default:
							THROW_RUNTIME_ERROR("Unexpected node type %i",CZetScriptUtils::intToString(eval_node_sp->node_type));
							return false;
							break;
						}
					}
			}
			else{ // TERMINAL NODE

				if(!insertLoadValueInstruction(_node, _lc)){
					return false;
				}
			}
		}else{

			if(_node->operator_info == PUNCTUATOR_TYPE::UNKNOWN_PUNCTUATOR){
				writeErrorMsg(GET_AST_FILENAME_LINE(_node->idxAstNode),"Malformed expression at line %i");
				return false;
			}

			if(_node->operator_info == TERNARY_IF_PUNCTUATOR){
				if(AST_NODE(_node->children[1])->operator_info == TERNARY_ELSE_PUNCTUATOR){
					return gacInlineIf(_node,_lc);

				}else{
					writeErrorMsg(GET_AST_FILENAME_LINE(_node->idxAstNode)," Put parenthesis on the inner ternary conditional");
					return false;
				}
			}else{
				// only inserts terminal symbols...
				if(_node!=NULL ){
					access_node = _node->operator_info == PUNCTUATOR_TYPE::FIELD_PUNCTUATOR;
				}
				// check if there's inline-if-else
				bool right=false, left=false;
				if((left=gacExpression_Recursive(AST_NODE(_node->children[LEFT_NODE]), _lc))==false){
					return false;
				}

				if(_node->children.size()==2){
					if((right=gacExpression_Recursive(AST_NODE(_node->children[RIGHT_NODE]),_lc))==false){
						return false;
					}
				}

				if(left !=false && right!=false){ // 2 ops (i.e 2+2)

					// Ignore punctuator node. Only take cares about terminal symbols...
					if(!access_node){
						// particular case if operator is =
						if(!insertOperatorInstruction(_node->operator_info,_node->idxAstNode,error_str)){
							writeErrorMsg(GET_AST_FILENAME_LINE(_node->idxAstNode),"%s",error_str.c_str());
							return false;
						}
					}

				}else if(left!=false){ // one op.. (i.e -i;)
					if(!insertOperatorInstruction(_node->operator_info,_node->idxAstNode,  error_str)){
						writeErrorMsg(GET_AST_FILENAME_LINE(_node->idxAstNode),"%s",error_str.c_str());
						return false;
					}
				}else{ // ERROR
					THROW_RUNTIME_ERROR("ERROR both ops ==0");
					return false;
				}
			}
		}

		// we ignore field node instruction ...

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
				//tInfoStatementOpCompiler *ptr_current_statement_op = &this->m_currentFunctionInfo->stament[this->m_currentFunctionInfo->stament.size()-1];
				tInfoAsmOpCompiler *asm_op = m_currentFunctionInfo->asm_op[m_currentFunctionInfo->asm_op.size()-1];
				asm_op->pre_post_op_type=pre_operator==PRE_DEC_PUNCTUATOR?INS_PROPERTY_PRE_DEC:INS_PROPERTY_PRE_INC;
			}
			break;
		default:
			break;
		}

		return true;
	}

	bool CCompiler::registerVariableClassSymbol(short idx_var_node, const string & class_name_to_register,CScriptClass * current_class){
		tInfoVariableSymbol *irs_dest=NULL;
		string base_class_name = current_class->metadata_info.object_info.symbol_info.symbol_ref;

		PASTNode var_node = AST_NODE(idx_var_node);

		CScriptClass *sc=CScriptClass::getScriptClassByName(class_name_to_register);
		if(sc==NULL){
			return NULL;
		}

		PASTNode ast_class_to_register=AST_NODE(sc->metadata_info.object_info.symbol_info.idxAstNode);
		string symbol_ref = "";
		if(current_class->is_c_class()){
			symbol_ref=makeSymbolRef(var_node->symbol_value,IDX_C_CLASS_SCOPE);
		}
		else{
			symbol_ref=makeSymbolRef(var_node->symbol_value,ast_class_to_register->idxScope);
		}

		if(CScriptClass::variableSymbolExist(sc,symbol_ref)){ // let's search
			string conflict_class="unknow";
			CScriptClass *current_sc=sc;
			bool found=false;

			while( current_sc->idxBaseClass.size()>0 && !found){
				current_sc=CScriptClass::getScriptClassByIdx(current_sc->idxBaseClass[0]); // get base class...
				ast_class_to_register=AST_NODE(current_sc->metadata_info.object_info.symbol_info.idxAstNode);
				symbol_ref= makeSymbolRef(var_node->symbol_value,ast_class_to_register->idxScope);
				if(CScriptClass::variableSymbolExist(current_sc,symbol_ref)){
					conflict_class=current_sc->metadata_info.object_info.symbol_info.symbol_ref;
					found=true;
				}
			}

			writeErrorMsg(GET_AST_FILENAME_LINE(idx_var_node),"Symbol variable \"%s::%s\" has been already inherited from \"%s::%s\"",class_name_to_register.c_str(),var_node->symbol_value.c_str(), conflict_class.c_str(),var_node->symbol_value.c_str() );
			return false;
		}

		if(symbol_ref==""){
			THROW_RUNTIME_ERROR("symbol name is null");
			return false;
		}

		if((irs_dest=CScriptClass::registerVariableSymbol(
				class_name_to_register,
				symbol_ref,
				idx_var_node
			)) == NULL){
			return false;
		}

		if(current_class->is_c_class()){
			tInfoVariableSymbol *irs_src=CScriptClass::getRegisteredVariableSymbol(base_class_name, symbol_ref);
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
		return true;
	}

	bool CCompiler::registerFunctionClassSymbol(short idx_node_fun, const string & class_name_to_register,CScriptClass * current_class ){
			CScriptFunctionObject *irfs;
			string current_class_name = current_class->metadata_info.object_info.symbol_info.symbol_ref;
			PASTNode node_class = AST_NODE(current_class->metadata_info.object_info.symbol_info.idxAstNode);
			PASTNode _node_ret=NULL;
			PASTNode fun_node = AST_NODE(idx_node_fun);
			PASTNode args = AST_NODE(fun_node->children[0]);
			//string symbol_value = makeSymbolRef(node_fun->symbol_value,idx_node_fun);

			CScriptClass *sc=CScriptClass::getScriptClassByName(class_name_to_register);
			if(sc==NULL){
				return NULL;
			}

			PASTNode ast_class_to_register=AST_NODE(sc->metadata_info.object_info.symbol_info.idxAstNode);
			string symbol_ref = makeSymbolRef(fun_node->symbol_value,ast_class_to_register->idxScope);

			zs_print_debug_cr("* %s::%s",current_class_name.c_str(), symbol_ref.c_str());

			if(current_class_name == fun_node->symbol_value){ // constructor symbol...
				symbol_ref = makeSymbolRef(class_name_to_register, ast_class_to_register->idxScope); // we change the function name in order to match same function usign super keyword ...

				if((_node_ret=CASTNode::itHasReturnSymbol(fun_node))!=NULL){
					writeErrorMsg(GET_AST_FILENAME_LINE(_node_ret->idxAstNode),"return keyword is not allowed in constructor");
					return false;
				}
			}

			if((irfs=CScriptClass::registerFunctionSymbol(
					class_name_to_register,
					symbol_ref,
					fun_node->idxAstNode
			)) == NULL){
				return false;
			}

			if(current_class->is_c_class()){ // set c refs ...

				CScriptFunctionObject *irs_src=CScriptClass::getScriptFunctionObjectByClassFunctionName(
						current_class_name,
						CCompiler::makeSymbolRef(fun_node->symbol_value,IDX_C_CLASS_SCOPE)
				);

				if(irs_src){

					// copy c refs ...
					irfs->object_info.symbol_info.symbol_ref= symbol_ref;//irs_src->object_info.symbol_info.symbol_name;
					irfs->object_info.symbol_info.properties = irs_src->object_info.symbol_info.properties;

					irfs->object_info.symbol_info.ref_ptr = irs_src->object_info.symbol_info.ref_ptr;
					irfs->m_arg = irs_src->m_arg;
					irfs->idx_return_type = irs_src->idx_return_type;

				}else{
					return false;
				}

			}else{ // compile function ...
				// compile function (within scope class)...
				if(!gacFunctionOrOperator(fun_node, SCOPE_NODE(node_class->idxScope),irfs)){
					return false;
				}
			}
			return true;
	}

	bool CCompiler::doRegisterVariableSymbolsClass(const string & class_name, CScriptClass *current_class){

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

		//
		// register all vars...
		PASTNode vars_collection_node = AST_NODE(node_class->children[0]);
		for(unsigned i = 0; i < vars_collection_node->children.size(); i++){ // foreach declared var.
			// register variable...
			PASTNode var_node=AST_NODE(vars_collection_node->children[i]);
			for(unsigned j = 0; j < var_node->children.size(); j++){ // foreach declared var.

				if(!registerVariableClassSymbol(var_node->children[j],class_name, current_class)){
					return false;
				}
			}
		}

		// register all functions ...
		PASTNode function_collection_node = AST_NODE(node_class->children[1]);
		for(unsigned i = 0; i < function_collection_node->children.size(); i++){
			// register function...
			if(!registerFunctionClassSymbol(function_collection_node->children[i],class_name,current_class)){
				return false;
			}
		}
		return true;
	}

	bool CCompiler::gacClass(PASTNode _node, CScope * _lc){

		if(_node->node_type != KEYWORD_NODE){THROW_RUNTIME_ERROR("node is not keyword type or null");return false;}
		if(_node->keyword_info != KEYWORD_TYPE::CLASS_KEYWORD){THROW_RUNTIME_ERROR("node is not CLASS keyword type");return false;}
		if(_node->children.size()==3 && AST_NODE(_node->children[2])->node_type != BASE_CLASS_NODE){THROW_RUNTIME_ERROR("expected BASE CLASS keyword type");return false;}

		string base_class= "";
		if(_node->children.size()==3){
			base_class=	AST_NODE(_node->children[2])->symbol_value;
		}

		CScriptClass *irc =NULL;

		if(_node->children.size()!=2 && _node->children.size()!=3) {THROW_RUNTIME_ERROR("node CLASS has not valid number of nodes");return false;}

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

	bool CCompiler::gacNew(PASTNode _node, CScope * _lc){
		if(_node->node_type != NEW_OBJECT_NODE ){THROW_RUNTIME_ERROR("node is not NEW OBJECT NODE type");return false;}
		if(_node->children.size()!=1) {THROW_RUNTIME_ERROR("node NEW has not valid number of nodes");return false;}
		if(AST_NODE(_node->children[0])->node_type!=NODE_TYPE::ARGS_PASS_NODE) {THROW_RUNTIME_ERROR("children[0] is not args_pass_node");return false;}

		tInfoAsmOpCompiler *iao=NULL;

		// load function ...
		// 1. create object instruction ...
		if(!insert_NewObject_Instruction(_node->idxAstNode,_node->symbol_value)) // goto end  ...
		{
			return false;
		}

		// 2. load function ...
		if(!insertLoadValueInstruction(_node,_lc,&iao)) {
			return false;
		}

		// set load as function constructor ...
		(*iao).runtime_prop = INS_PROPERTY_CONSTRUCT_CALL;

		int call_index = getCurrentInstructionIndex();

		// 3. pass parameters ...
		PASTNode constructor_args = AST_NODE(_node->children[0]);

		if(constructor_args->children.size() > 0){

			for(unsigned k = 0; k < constructor_args->children.size(); k++){

				// check whether is expression node...
				if(!gacExpression(AST_NODE(constructor_args->children[k]), _lc)){
					return false;
				}
			}
		}

		// 4. call function...
		insert_CallFunction_Instruction(_node->idxAstNode,call_index);

		return true;
	}

	bool CCompiler::gacDelete(PASTNode _node, CScope * _lc){


		if(_node->node_type != DELETE_OBJECT_NODE ){THROW_RUNTIME_ERROR("gacDelete: node is not NEW OBJECT NODE type");return false;}
		if(_node->children.size() != 1 ){THROW_RUNTIME_ERROR("gacDelete: expected 1 children");return false;}

		// parse expression ...
		if(!ast2asm_Recursive(_node->children[0],SCOPE_NODE(_node->idxScope))){ return ZS_UNDEFINED_IDX;}

		// delete object instruction ...
		if(!insert_DeleteObject_Instruction(_node->idxAstNode)) // goto end  ...
		{
			return false;
		}
		return true;
	}

	bool CCompiler::gacFor(PASTNode _node, CScope * _lc){

		if(_node->node_type != KEYWORD_NODE ){THROW_RUNTIME_ERROR("node is not keyword type or null");return false;}
		if(_node->keyword_info != KEYWORD_TYPE::FOR_KEYWORD){THROW_RUNTIME_ERROR("node is not FOR keyword type");return false;}
		if(_node->children.size()!=4) {THROW_RUNTIME_ERROR("node FOR has not valid number of nodes");return false;}
		if(!(AST_NODE(_node->children[0])->node_type==PRE_FOR_NODE && AST_NODE(_node->children[1])->node_type==CONDITIONAL_NODE &&
		AST_NODE(_node->children[2])->node_type==POST_FOR_NODE && AST_NODE(_node->children[3])->node_type==BODY_BLOCK_NODE)) {THROW_RUNTIME_ERROR("node FOR has not valid TYPE nodes");return false;}
		tInfoAsmOpCompiler *asm_op=NULL;

		PASTNode ast_conditional=NULL;

		// push current continue/break instruction scope...
		pushContinueInstructionScope();
		pushBreakInstructionScope();


		// 0. insert push statment
		if(!insertPushScopeInstruction(_node->idxAstNode,_node->idxScope,SCOPE_BREAKPOINT_TYPE::BREAK)){ // scope for vars...
			return false;
		}

		// 1. compile var init ...
		if(AST_NODE(_node->children[0])->children.size()>0){
			if(!ast2asm_Recursive(_node->children[0],SCOPE_NODE(_node->idxScope))){ return false;}
		}

		// 2. compile conditional
		ast_conditional=AST_NODE(_node->children[1]);
		int index_statment_conditional_for_= ZS_UNDEFINED_IDX;
		if(ast_conditional->children.size()>0){ // it has conditional... (infinite else)
			index_statment_conditional_for_= getCurrentInstructionIndex()+1;

			if(!ast2asm_Recursive(ast_conditional->idxAstNode,SCOPE_NODE(_node->idxScope))){ return false;}

			asm_op = insert_JNT_Instruction(_node->children[1]);
		}
		// get current index statment in order to jmp from end body for.


		// 3. compile body
		PASTNode _body_node=AST_NODE(_node->children[3]);
		int instruction_pushscope=getCurrentInstructionIndex()+1;
		if(!gacBody(AST_NODE(_node->children[3]),SCOPE_NODE(_body_node->idxScope))){ return false;}
		int instruction_popscope=getCurrentInstructionIndex();

		// because continue behaviour es inner body we have to set instructions after for-body is compiled...
		if(m_currentFunctionInfo->asm_op[instruction_pushscope]->operator_type != ASM_OPERATOR::PUSH_SCOPE){
			THROW_RUNTIME_ERROR("expected push scope");return false;
		}

		m_currentFunctionInfo->asm_op[instruction_pushscope]->index_op1=SCOPE_BREAKPOINT_TYPE::CONTINUE;

		if(m_currentFunctionInfo->asm_op[instruction_popscope]->operator_type != ASM_OPERATOR::POP_SCOPE){
			THROW_RUNTIME_ERROR("expected pop scope");return false;
		}

		m_currentFunctionInfo->asm_op[instruction_popscope]->index_op1=SCOPE_BREAKPOINT_TYPE::CONTINUE;

		// 4. compile post oper
		PASTNode post_node=AST_NODE(_node->children[2]);
		for(unsigned i=0; i < post_node->children.size(); i++){
			if(!ast2asm_Recursive(post_node->children[i],SCOPE_NODE(_node->idxScope))){ return false;}
		}

		// 5. jmp to the conditional index ...
		if(ast_conditional->children.size()>0){
			insert_JMP_Instruction(ast_conditional->idxAstNode,index_statment_conditional_for_);
		}

		// 6. insert pop scope...
		insertPopScopeInstruction(_node->idxAstNode,SCOPE_BREAKPOINT_TYPE::BREAK);

		// save JNT value...
		if(asm_op != NULL){
			asm_op->index_op2=getCurrentInstructionIndex();
		}

		// set jmps from breaks...
		// and pop break/continue instructions scope...

		popContinueInstructionScope();
		popBreakInstructionScope();


		return true;
	}

	bool CCompiler::gacWhile(PASTNode _node, CScope * _lc){

		if(_node->node_type != KEYWORD_NODE){THROW_RUNTIME_ERROR("node is not keyword type or null");return false;}
		if(_node->keyword_info != KEYWORD_TYPE::WHILE_KEYWORD){THROW_RUNTIME_ERROR("node is not WHILE keyword type");return false;}
		if(_node->children.size()!=2) {THROW_RUNTIME_ERROR("node WHILE has not valid number of nodes");return false;}
		if(!(AST_NODE(_node->children[0])->node_type==CONDITIONAL_NODE && AST_NODE(_node->children[1])->node_type==BODY_BLOCK_NODE )) {THROW_RUNTIME_ERROR("node WHILE has not valid TYPE nodes");return false;}
		tInfoAsmOpCompiler *asm_op_jmp_end;
		int index_ini_while;

		// push current break instruction scope...
		pushBreakInstructionScope();


		// compile conditional expression...
		if(!ast2asm_Recursive(_node->children[0],_lc)){ return false;}
		index_ini_while = getCurrentInstructionIndex();
		asm_op_jmp_end = insert_JNT_Instruction(_node->children[0]); // goto end  ...

		// compile if-body ...
		if(!gacBody(AST_NODE(_node->children[1]),SCOPE_NODE(AST_NODE(_node->children[1])->idxScope))){ return false;}
		insert_JMP_Instruction(_node->children[0],index_ini_while); // goto end  ...

		// save jmp value ...
		asm_op_jmp_end->index_op2= getCurrentInstructionIndex()+1;

		// set jmps from breaks...


		// and pop break instructions scope...
		popBreakInstructionScope();
		return true;
	}

	bool CCompiler::gacDoWhile(PASTNode _node, CScope * _lc){

		if(_node->node_type != KEYWORD_NODE){THROW_RUNTIME_ERROR("node is not keyword type or null");return false;}
		if(_node->keyword_info != KEYWORD_TYPE::DO_WHILE_KEYWORD){THROW_RUNTIME_ERROR("node is not DO_WHILE keyword type");return false;}
		if(_node->children.size()!=2) {THROW_RUNTIME_ERROR("node DO-WHILE has not valid number of nodes");return false;}
		if(!(AST_NODE(_node->children[0])->node_type==CONDITIONAL_NODE && AST_NODE(_node->children[1])->node_type==BODY_BLOCK_NODE )) {THROW_RUNTIME_ERROR("node WHILE has not valid TYPE nodes");return false;}
		int index_start_do_while;

		// push current break instruction scope...
		pushBreakInstructionScope();

		// compile conditional expression...
		index_start_do_while =  getCurrentInstructionIndex()+1;

		// compile do-body ...
		if(!gacBody(AST_NODE(_node->children[1]),SCOPE_NODE(AST_NODE(_node->children[1])->idxScope))){ return false;}

		// compile while condition...
		if(!ast2asm_Recursive(_node->children[0],_lc)){ return false;}

		insert_JT_Instruction(_node->children[0],index_start_do_while); // goto end  ...

		// save jmp value ...
		//asm_op_jmp_end->index_op2= index_start_do_while;

		// set jmps from breaks...


		// and pop break instructions scope...
		popBreakInstructionScope();

		return true;
	}

	bool CCompiler::gacReturn(PASTNode _node, CScope * _lc){

		if(_node->node_type != KEYWORD_NODE){THROW_RUNTIME_ERROR("node is not keyword type or null");return false;}
		if(_node->keyword_info != KEYWORD_TYPE::RETURN_KEYWORD){THROW_RUNTIME_ERROR("node is not RETURN keyword type");return false;}
		if(_node->children.size() >= 1){ // expression
			if(!gacExpression(AST_NODE(_node->children[0]), _lc)){
				return false;
			}
		}

		insertRet(_node->idxAstNode);

		return true;
	}

	bool CCompiler::gacFunctionOrOperator(PASTNode _node, CScope * _lc, CScriptFunctionObject *irfs){


		if(
			! ( _node->keyword_info == KEYWORD_TYPE::FUNCTION_KEYWORD
			||  _node->node_type    == FUNCTION_OBJECT_NODE
			)) {
			writeErrorMsg(GET_AST_FILENAME_LINE(_node->idxAstNode),"Expected FUNCTION or OPERATOR or FUNCTION_OBJECT_NODE keyword type");
			return false;
		}

		if(_node->children.size() != 2){THROW_RUNTIME_ERROR("node FUNCTION has not 2 child");return false;}
		if(AST_NODE(_node->children[0])->node_type != NODE_TYPE::ARGS_DECL_NODE){THROW_RUNTIME_ERROR("node FUNCTION has not ARGS node");return false;}
		if(AST_NODE(_node->children[1])->node_type != NODE_TYPE::BODY_BLOCK_NODE){THROW_RUNTIME_ERROR("node FUNCTION has not BODY node");return false;}

		// 1. Processing args ...
		for(unsigned i = 0; i < AST_NODE(_node->children[0])->children.size(); i++){
			string ss = AST_NODE(AST_NODE(_node->children[0])->children[i])->symbol_value;
			irfs->m_arg.push_back(
					{ZS_UNDEFINED_IDX,ss}
			);
		}
		// 2. Compiles function's body ...
		return compile_body(AST_NODE(_node->children[1]), irfs);
	}

	bool CCompiler::gacBreak(PASTNode _node, CScope * _lc){

		if(_node->keyword_info != KEYWORD_TYPE::BREAK_KEYWORD ){THROW_RUNTIME_ERROR("node is not break type");return false;}

		tInfoAsmOpCompiler *iao= insert_JMP_Instruction(_node->idxAstNode, -1,-1);
		this->ptr_breakInstructionsScope->asm_op.push_back(iao);

		return true;
	}

	bool CCompiler::gacContinue(PASTNode _node, CScope * _lc){

		if(_node->keyword_info != KEYWORD_TYPE::CONTINUE_KEYWORD ){THROW_RUNTIME_ERROR("node is not continue type");return false;}

		tInfoAsmOpCompiler *iao= insert_JMP_Instruction(_node->idxAstNode, -1,-1);
		this->ptr_continueInstructionsScope->asm_op.push_back(iao);

		return true;
	}

	bool CCompiler::gacIf(PASTNode _node, CScope * _lc){

		if(_node->node_type != KEYWORD_NODE ){THROW_RUNTIME_ERROR("node is not keyword type or null");return false;}
		if(_node->keyword_info != KEYWORD_TYPE::IF_KEYWORD){THROW_RUNTIME_ERROR("node is not IF keyword type");return false;}
		if(_node->children.size()<1) {THROW_RUNTIME_ERROR("node IF has not valid number of nodes");return false;}

		CASTNode *if_group_nodes =  AST_NODE(_node->children[0]);

		if(if_group_nodes->node_type!=GROUP_IF_NODES) {THROW_RUNTIME_ERROR("node IF has not GROUP IF NODES");return false;}
		if((if_group_nodes->children.size()<1)) {THROW_RUNTIME_ERROR("GROUP IF NODES has to have at least one node.");return false;}

		tInfoAsmOpCompiler *asm_op_jmp_else_if;

		vector<tInfoAsmOpCompiler *> asm_op_jmp_end;

		for(unsigned int i=0; i < if_group_nodes->children.size(); i++){

			CASTNode * if_node =  AST_NODE(if_group_nodes->children[i]);
			if(if_node->children.size() != 2) {THROW_RUNTIME_ERROR(" IF NODE HAS TO HAVE 2 NODES");return false;}

			// compile conditional expression...
			if(!ast2asm_Recursive(if_node->children[0],_lc)){ return false;}
			asm_op_jmp_else_if = insert_JNT_Instruction(if_node->children[0]); // goto else body ...

			// compile if-body ...
			if(!gacBody(AST_NODE(if_node->children[1]),SCOPE_NODE(AST_NODE(if_node->children[1])->idxScope))){ return false;}

			asm_op_jmp_end.push_back(insert_JMP_Instruction(_node->idxAstNode));
			asm_op_jmp_else_if->index_op2 = getCurrentInstructionIndex()+1;
		}

		// if there's else body, compile-it
		if(_node->children.size()==2){
			//asm_op_jmp_end = insert_JMP_Instruction(_node->idxAstNode); // goto end
			asm_op_jmp_else_if->index_op2 = getCurrentInstructionIndex()+1;
			if(!gacBody(AST_NODE(_node->children[1]),_lc)){ return false;}
		}

		// assign all if jmp after block finishes (except else)
		for(unsigned j = 0; j < asm_op_jmp_end.size();j++){
			asm_op_jmp_end[j]->index_op2 = getCurrentInstructionIndex()+1;
		}

		return true;
	}

	bool CCompiler::gacInlineIf(PASTNode _node, CScope * _lc){

		if(_node->node_type != PUNCTUATOR_NODE ){THROW_RUNTIME_ERROR("node is not punctuator type or null");return false;}
		if(_node->operator_info != TERNARY_IF_PUNCTUATOR){THROW_RUNTIME_ERROR("node is not INLINE-IF PUNCTUATOR type");return false;}
		if(_node->children.size()!=2) {THROW_RUNTIME_ERROR("node INLINE-IF has not 2 nodes");return false;}
		if(!(AST_NODE(_node->children[1])->node_type==PUNCTUATOR_NODE && AST_NODE(_node->children[1])->operator_info==TERNARY_ELSE_PUNCTUATOR )) {THROW_RUNTIME_ERROR("node INLINE-ELSE has not found");return false;}
		if(AST_NODE(_node->children[1])->children.size() != 2) {THROW_RUNTIME_ERROR("node INLINE-ELSE has not 2 nodes");return false;}
		tInfoAsmOpCompiler *asm_op_jmp_else_if,*asm_op_jmp_end;

		// compile conditional expression...
		if((!gacExpression_Recursive(AST_NODE(_node->children[0]),_lc))){ return false;}
		asm_op_jmp_else_if = insert_JNT_Instruction(_node->children[0]); // goto else body ...

		// compile if-body ...
		if((!gacExpression_Recursive(AST_NODE(AST_NODE(_node->children[1])->children[0]),_lc))){ return false;}

		// compile else-body ...
		asm_op_jmp_end = insert_JMP_Instruction(_node->idxAstNode); // goto end+

		asm_op_jmp_else_if->index_op1 = ZS_UNDEFINED_IDX;
		asm_op_jmp_else_if->index_op2 = getCurrentInstructionIndex()+1;//getCurrentStatmentIndex();


		if((!gacExpression_Recursive(AST_NODE(AST_NODE(_node->children[1])->children[1]),_lc))){ return false;}

		asm_op_jmp_end->index_op1 = ZS_UNDEFINED_IDX;//getCurrentInstructionIndex()+1;
		asm_op_jmp_end->index_op2 = getCurrentInstructionIndex()+1;



		return true;
	}

	bool CCompiler::gacSwitch(PASTNode _node, CScope * _lc){

		if(_node->node_type != KEYWORD_NODE ){THROW_RUNTIME_ERROR("node is not keyword type or null");return false;}
		if(_node->keyword_info != SWITCH_KEYWORD){THROW_RUNTIME_ERROR("node is not SWITCH keyword type");return false;}
		if(_node->children.size() != 2){THROW_RUNTIME_ERROR("SWITCH node has no 2 nodes");return false;}
		bool has_default = false;
		PASTNode switch_node;

		bool last_keyword_was_break=false;

		struct tCaseInfo{
			PASTNode case_node;
			vector<short> body_node;
			tInfoAsmOpCompiler *info_jt_instruction;
		};

		/**
		 * A case group is a group of cases that ends with break or } (i.e switch end)
		 */
		struct tCaseGroup{
			vector <tCaseInfo> case_info;
			PASTNode end_body_node;
		};

		vector <tCaseGroup> case_group;
		tCaseGroup current_case_group;
		tCaseGroup *default_case_group_ptr;
		tCaseGroup default_case_group;

		tInfoAsmOpCompiler * asm_op=NULL;
		tInfoAsmOpCompiler *jmp_default_end_switch;

		string error_str;
		string detected_type_str;
		 vector<tInfoAsmOpCompiler *> jmp_instruction;//=NULL;
		 vector<vector<tInfoAsmOpCompiler *>> jt_instruction;

		 // override current scope by switch node...
		 _lc = SCOPE_NODE(_node->idxScope);


		// create new statment ...

		if(!ast2asm_Recursive(_node->children[0],_lc)){ // insert condition value node 0)...
			return false;
		}

		// push current break instruction scope...
		pushBreakInstructionScope();


		if(!insertPushScopeInstruction(_node->idxAstNode,_node->idxScope,SCOPE_BREAKPOINT_TYPE::BREAK)){
			return false;
		}


		// get current instruction value to take as ref for compare within value cases...
		int switch_value_index  = getCurrentInstructionIndex();

		switch_node = AST_NODE(_node->children[1]);
		PASTNode last_case_default_node = NULL;
		tCaseGroup *current_case_group_ptr=NULL;
		tCaseGroup *last_case_group_ptr=NULL;
		default_case_group_ptr=NULL;



		for(unsigned i = 0; i < switch_node->children.size(); i++){ // find cases/default within switch body ...

			PASTNode current_node=AST_NODE(switch_node->children[i]);

			if(current_case_group_ptr == NULL){ // init by case or default...

				 if(current_node->keyword_info == KEYWORD_TYPE::CASE_KEYWORD){

						tCaseGroup current_case_group;
						current_case_group.end_body_node=NULL;
						current_case_group.case_info.clear();

						case_group.push_back(current_case_group);

						current_case_group_ptr=&case_group[case_group.size()-1];

				 }else if(current_node->keyword_info == KEYWORD_TYPE::DEFAULT_KEYWORD){
						if(default_case_group_ptr==NULL){
							default_case_group_ptr == &default_case_group;
							current_case_group_ptr=default_case_group_ptr;
						}else{
							writeErrorMsg(GET_AST_FILENAME_LINE(current_node->idxAstNode),"default statement already defined at line %i",default_case_group_ptr->case_info[0].case_node->line_value);
							return false;
						}
				 }else{
						writeErrorMsg(GET_AST_FILENAME_LINE(current_node->idxAstNode),"expected case or default");
						return false;
				 }
			}

			if(current_node->keyword_info == KEYWORD_TYPE::CASE_KEYWORD || current_node->keyword_info == KEYWORD_TYPE::DEFAULT_KEYWORD){

				tCaseInfo case_info;// = {current_node}; // init case info...
				case_info.case_node=current_node;

				if(last_case_default_node != NULL){
					if( (last_case_default_node->keyword_info == KEYWORD_TYPE::DEFAULT_KEYWORD && current_node->keyword_info == KEYWORD_TYPE::CASE_KEYWORD)
					 || (current_node->keyword_info == KEYWORD_TYPE::DEFAULT_KEYWORD && last_case_default_node->keyword_info == KEYWORD_TYPE::CASE_KEYWORD)){
						writeErrorMsg(GET_AST_FILENAME_LINE(current_node->idxAstNode),"default statement cannot combain with default \"cases\"");
						return false;
					}
				}

				if(current_node->keyword_info == KEYWORD_TYPE::CASE_KEYWORD){

				 // CASE : write CMP + JT

					// load case X:
					insertLoadValueInstruction(current_node,_lc);

					// is equal ? ==
					if(!insertOperatorInstruction(LOGIC_EQUAL_PUNCTUATOR,0, error_str)){
						writeErrorMsg(GET_AST_FILENAME_LINE(current_node->idxAstNode),"%s",error_str.c_str());
						return false;
					}

					// set property as READ_TWO_AND_ONE_POP...
					asm_op=getLastInsertedInfoAsmOpCompiler();
					asm_op->runtime_prop|=STK_PROPERTY_READ_TWO_POP_ONE;

					// insert jmp instruction and save its information to store where to jmp when we know the total code size of cases...
					case_info.info_jt_instruction=insert_JT_Instruction(current_node->idxAstNode);
					//jt_instruction[i-1].push_back();
				}

				current_case_group_ptr->case_info.push_back(case_info);

			}
			else{
				 if(current_node->keyword_info == KEYWORD_TYPE::BREAK_KEYWORD){ // set jmp value to accumulated cases-default
					 current_case_group_ptr->end_body_node=current_node;
					 current_case_group_ptr=NULL;
				 }
				 else{ // set start expression node ...
					 current_case_group_ptr->case_info[current_case_group_ptr->case_info.size()-1].body_node.push_back(current_node->idxAstNode);
				 }
			}
		}

		// write JMP (for default or end switch)
		jmp_default_end_switch = insert_JMP_Instruction(-1);


		// build body-case ...
		for(unsigned i = 0; i < case_group.size(); i++){

			for(unsigned j = 0; j < case_group[i].case_info.size(); j++){

				//tInfoStatementOpCompiler *st= newStatment();
				case_group[i].case_info[j].info_jt_instruction->index_op1 = ZS_UNDEFINED_IDX;
				case_group[i].case_info[j].info_jt_instruction->index_op2 = getCurrentInstructionIndex();

				// compile all body nodes ...
				for(unsigned k = 0; k < case_group[i].case_info[j].body_node.size(); k++){
					if(!ast2asm_Recursive(case_group[i].case_info[j].body_node[k],_lc)){
						return false;
					}
				}
			}

		}

		// build body-default (if exist)...

		// link jmp-break body-case...

		insertPopScopeInstruction(_node->idxAstNode,SCOPE_BREAKPOINT_TYPE::BREAK);



		// and pop break instructions scope...
		popBreakInstructionScope();

		return true;
	}

	bool CCompiler::gacVar(PASTNode _node, CScope * _lc){


		if(_node->node_type != KEYWORD_NODE ){THROW_RUNTIME_ERROR("node is not keyword type or null");return false;}
		if(_node->keyword_info != VAR_KEYWORD){THROW_RUNTIME_ERROR("node is not VAR keyword type");return false;}

		int local_variable_idx;

		for(unsigned i = 0; i < _node->children.size(); i++){ // for all vars ...
			PASTNode node_var=AST_NODE(_node->children[i]);
			CScope * scope_var=SCOPE_NODE(node_var->idxScope);

			bool is_var_member = (scope_var->getIdxBaseScope() != 0) && (node_var->idxScope == scope_var->getIdxBaseScope());

			if(is_var_member){ // member var
				short idxRootAstClass = scope_var->getIdxBaseAstNode();
				PASTNode class_node=AST_NODE(idxRootAstClass);
				CScriptClass * sc=CScriptClass::getScriptClassByName(class_node->symbol_value,false);
				if(sc==NULL){
					THROW_RUNTIME_ERROR("cannot get class_node %s",class_node->symbol_value.c_str());return false;
					return false;
				}

				if(!registerVariableClassSymbol(node_var->idxAstNode ,class_node->symbol_value,sc)){
						return false;
				}

			}else { // normal var

				if((local_variable_idx=addLocalVarSymbol(node_var->symbol_value,node_var->idxAstNode)) == ZS_UNDEFINED_IDX){
					return false;
				}

				//for(unsigned i = 0 ; i < _node->children.size(); i++){ // init all requested vars...
				if(node_var->children.size()==1){
					if(!gacExpression(AST_NODE(node_var->children[0]), _lc)){
						return false;
					}
				}
			}
		}
		return true;
	}

	bool CCompiler::gacKeyword(PASTNode _node, CScope * _lc){


		CScope * _scope_node = NULL;//SCOPE_NODE(_node->idxScope);
		bool is_function_member;

		CScriptFunctionObject *function_object=NULL;
		if(_node->node_type != KEYWORD_NODE){THROW_RUNTIME_ERROR("node is not keyword type or null");return false;}

		switch(_node->keyword_info){
		default:
			THROW_RUNTIME_ERROR("Keyword [ %s ] not implemented yet",CASTNode::defined_keyword[_node->keyword_info].str);
			break;
		case KEYWORD_TYPE::CLASS_KEYWORD:
			return gacClass(_node, _lc);
			break;
		case KEYWORD_TYPE::SWITCH_KEYWORD:
			return gacSwitch(_node, _lc);
			break;
		case KEYWORD_TYPE::FOR_KEYWORD:
			return gacFor(_node, _lc);
			break;
		case KEYWORD_TYPE::WHILE_KEYWORD:
			return gacWhile(_node, _lc);
			break;
		case KEYWORD_TYPE::DO_WHILE_KEYWORD:
			return gacDoWhile(_node, _lc);
			break;
		case KEYWORD_TYPE::IF_KEYWORD:
			return gacIf(_node, _lc);
			break;
		case KEYWORD_TYPE::BREAK_KEYWORD:
			return gacBreak(_node, _lc);
			break;
		case KEYWORD_TYPE::CONTINUE_KEYWORD:
			return gacContinue(_node, _lc);
			break;
		case KEYWORD_TYPE::VAR_KEYWORD:
			return gacVar(_node, _lc);
			break;
		case KEYWORD_TYPE::FUNCTION_KEYWORD: // don't compile function. It will compiled later, after main body

			_scope_node = SCOPE_NODE(_node->idxScope);
			// check if function belongs to class...
			is_function_member = (_scope_node->getIdxBaseScope() != 0) && (_scope_node->idxScope == _scope_node->getIdxBaseScope());
			if(is_function_member){
				short idxRootAstClass = _scope_node->getIdxBaseAstNode();
				PASTNode class_node=AST_NODE(idxRootAstClass);
				CScriptClass * sc=CScriptClass::getScriptClassByName(class_node->symbol_value,false);
				if(sc==NULL){
					THROW_RUNTIME_ERROR("cannot get class_node %s",class_node->symbol_value.c_str());return false;
					return false;
				}

				if(!registerFunctionClassSymbol(_node->idxAstNode ,class_node->symbol_value,sc)){
						return false;
				}

				return true;
			}else{

				if(functionSymbolExists(_node->symbol_value, _node->idxAstNode)){
					writeErrorMsg(GET_AST_FILENAME_LINE(_node->idxAstNode),"Function \"%s\" already defined",_node->symbol_value.c_str());
						return false;
				}

				if((function_object=addLocalFunctionSymbol(_node->symbol_value,_node->idxAstNode)) == NULL){
					return false;
				}

				return gacFunctionOrOperator(_node, _lc,function_object);
			}
			break;
		case KEYWORD_TYPE::RETURN_KEYWORD:
			return gacReturn(_node, _lc);
			break;
		}

		return false;
	}

	bool CCompiler::gacBody(PASTNode _node, CScope * _lc){

		if(_node->node_type != BODY_BLOCK_NODE ){THROW_RUNTIME_ERROR("node is not BODY type or null");return false;}

		if(!insertPushScopeInstruction(_node->idxAstNode,_node->idxScope)){
			return false;
		}

		if(_node->children.size() > 0){ // body has code ...
			for(unsigned i = 0; i < _node->children.size(); i++){
				if(!ast2asm_Recursive(_node->children[i], _lc))
					return false;
			}
		}

		insertPopScopeInstruction(_node->idxAstNode);//,index);

		return true;
	}

	bool CCompiler::gacExpression(PASTNode _node, CScope *_lc){


		if(_node == NULL) {THROW_RUNTIME_ERROR("NULL node");return false;}
		if(_node->node_type != EXPRESSION_NODE){THROW_RUNTIME_ERROR("node is not Expression");return false;}

		return gacExpression_Recursive(AST_NODE(_node->children[0]), _lc);//,index_instruction);
	}

	bool CCompiler::ast2asm_Recursive(short idxAstNode, CScope *_lc){

		PASTNode _node = AST_NODE(idxAstNode);

		if(_node != NULL){
			switch(_node->node_type){
				default:
				case UNKNOWN_NODE:
					THROW_RUNTIME_ERROR("UNKNOWN_NODE (%i)",_node->node_type);
					return false;
					break;
				case DELETE_OBJECT_NODE:
					zs_print_debug_cr("DELETE NODE");
					return gacDelete(_node, _lc);
					break;
				case EXPRESSION_NODE: // in fact is EXPRESSION NODE
					zs_print_debug_cr("EXPRESSION_NODE");
					return gacExpression(_node, _lc);
					break;
				case GROUP_CASES_NODE:
					zs_print_debug_cr("GROUP_CASES_NODE");
					break;
				case KEYWORD_NODE:
					zs_print_debug_cr("KEYWORD_NODE %s",CASTNode::defined_keyword[_node->keyword_info].str);
					return gacKeyword(_node, _lc);
					break;
				case BODY_BLOCK_NODE:
					zs_print_debug_cr("BODY_BLOCK_NODE");
					return gacBody(_node, SCOPE_NODE(_node->idxScope)); // we pass scope node
					break;
				case POST_FOR_NODE:
				case CONDITIONAL_NODE:
					zs_print_debug_cr("%s",_node->node_type == CONDITIONAL_NODE ? "CONDITIONAL_NODE":"POST_FOR_NODE");
					if(_node->children.size() == 1){
						return gacExpression(AST_NODE(_node->children[0]), _lc);
					}else{
						THROW_RUNTIME_ERROR("Expected nodes for %i",_node->node_type);
					}

					break;
				case PRE_FOR_NODE:
					if(_node->children.size() == 1){
						return ast2asm_Recursive(_node->children[0], _lc);
					}else{
						THROW_RUNTIME_ERROR("Expected nodes for %i",_node->node_type);
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
			THROW_RUNTIME_ERROR("Node is null");
		}
		return false;
	}

	void CCompiler::pushFunction(short idxAstNode,CScriptFunctionObject *sf){


		stk_scriptFunction.push_back(m_currentFunctionInfo=new tInfoFunctionCompile(sf));
		this->m_treescope = SCOPE_NODE(AST_NODE(idxAstNode)->idxScope);
	}

	void CCompiler::popFunction(bool save_asm_op){

		m_currentFunctionInfo->function_info_object->object_info.asm_op=NULL;

		if (save_asm_op) {
			// get total size op + 1 ends with NULL
			unsigned size = (m_currentFunctionInfo->asm_op.size() + 1) * sizeof(tInfoAsmOp);
			m_currentFunctionInfo->function_info_object->object_info.asm_op = (PtrAsmOp)malloc(size);
			memset(m_currentFunctionInfo->function_info_object->object_info.asm_op, 0, size);

			tInfoAsmOp *dst_op = m_currentFunctionInfo->function_info_object->object_info.asm_op;

			for (unsigned j = 0; j < m_currentFunctionInfo->asm_op.size(); j++,dst_op++) {


				tInfoAsmOpCompiler *src_op = m_currentFunctionInfo->asm_op[j];

				dst_op->idxAstNode = src_op->idxAstNode;
				dst_op->operator_type = src_op->operator_type;
				dst_op->index_op1 = src_op->index_op1;
				dst_op->index_op2 = src_op->index_op2;
				dst_op->instruction_properties = src_op->pre_post_op_type | src_op->scope_type | src_op->runtime_prop;
			}
		}

		// delete and popback function information...
		delete(m_currentFunctionInfo);
		stk_scriptFunction.pop_back();

		if(stk_scriptFunction.size() > 0){
			m_currentFunctionInfo = stk_scriptFunction[stk_scriptFunction.size()-1];
			//this->m_currentListStatements = &m_currentFunctionInfo->object_info.asm_op;
			this->m_treescope =AST_SCOPE_INFO(m_currentFunctionInfo->function_info_object->object_info.symbol_info.idxAstNode);
		}
	}

	void CCompiler::pushBreakInstructionScope(){
		tBreakInstructionScope bis;
		stk_breakInstructionsScope.push_back(bis);
		ptr_breakInstructionsScope=&stk_breakInstructionsScope[stk_breakInstructionsScope.size()-1];
	}

	void CCompiler::popBreakInstructionScope(){
		stk_breakInstructionsScope.pop_back();
		ptr_breakInstructionsScope=NULL;
		if(stk_breakInstructionsScope.size() > 0){
			ptr_breakInstructionsScope=&stk_breakInstructionsScope[stk_breakInstructionsScope.size()-1];
		}
	}

	void CCompiler::pushContinueInstructionScope(){
		tContinueInstructionScope cis;
		stk_continueInstructionsScope.push_back(cis);
		ptr_continueInstructionsScope=&stk_continueInstructionsScope[stk_continueInstructionsScope.size()-1];
	}

	void CCompiler::popContinueInstructionScope(){

		stk_continueInstructionsScope.pop_back();
		ptr_continueInstructionsScope=NULL;
		if(stk_continueInstructionsScope.size() > 0){
			ptr_continueInstructionsScope=&stk_continueInstructionsScope[stk_continueInstructionsScope.size()-1];
		}
	}

	bool CCompiler::compile_body(PASTNode _node ,CScriptFunctionObject *sf){


		if(_node->node_type == NODE_TYPE::BODY_BLOCK_NODE ){

			bool error=false;
			pushFunction(_node->idxAstNode,sf);

			for(unsigned i = 0; i < _node->children.size() && !error; i++){

				if(!ast2asm_Recursive(_node->children[i], m_treescope)){
					error=true;
				}
			}

			popFunction();
			return !error;
		}
		else{
			THROW_RUNTIME_ERROR("Body node expected");
		}

		return false;
	}

	// clears statments on main function ...
	void CCompiler::clear() {
		CScriptFunctionObject *info_function=MAIN_SCRIPT_FUNCTION_OBJECT;

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

		stk_breakInstructionsScope.clear();
		ptr_breakInstructionsScope=NULL;

		stk_continueInstructionsScope.clear();
		ptr_continueInstructionsScope=NULL;
	}

	bool CCompiler::compile(){

		CScriptFunctionObject *sf=MAIN_SCRIPT_FUNCTION_OBJECT;

		vector<zetscript::tInfoAstNodeToCompile> astNodeToCompileAux=*CASTNode::astNodeToCompile;//->clear();

		// removes all ast node to compile...
		CASTNode::astNodeToCompile->clear();

		// remove old compile information...
		clear();

		// compile main code ...

		pushFunction(IDX_MAIN_AST_NODE,sf);
		for(unsigned i = 0; i < astNodeToCompileAux.size(); i++){

			short idxNodeToCompile =  astNodeToCompileAux.at(i).ast_node_to_compile;

			// compile ast node ...
			if(!ast2asm_Recursive(idxNodeToCompile, m_treescope)){
				return false;
			}
		}
		popFunction();


		// update reference symbols (like link))
		if(!CScriptClass::updateReferenceSymbols()){
			return false;
		}

		// build current cache after compile...
		CURRENT_VM->buildCache();

		return true;
	}

	CCompiler::~CCompiler(){

	}
}
