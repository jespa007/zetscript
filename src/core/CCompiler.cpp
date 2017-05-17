#include "CZetScript.h"

#ifdef __DEBUG__ // incoment __VERBOSE_MESSAGE__ to print all messages (wrning is going to be slow because of the prints)
//#define __VERBOSE_MESSAGE__
#endif



#ifdef  __VERBOSE_MESSAGE__

#define print_com_cr print_ast_cr
#else
#define print_com_cr(s,...)
#endif


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
		print_error_cr("constant %s already exist",const_name.c_str());
	}

	return info_ptr;
}

int CCompiler::addLocalVarSymbol(const string & var_name,short idxAstNode){



	if(!localVarSymbolExists(var_name,idxAstNode)){
		tInfoVariableSymbol info_symbol;

		info_symbol.idxAstNode = idxAstNode;
		info_symbol.symbol_name = var_name;



		this->m_currentFunctionInfo->function_info_object->object_info.local_symbols.m_registeredVariable.push_back(info_symbol);

		return this->m_currentFunctionInfo->function_info_object->object_info.local_symbols.m_registeredVariable.size()-1;
	}else{
		print_error_cr("variable \"%s\" defined at line %i already defined!",var_name.c_str(),AST_NODE(idxAstNode)->line_value);
	}
	return ZS_UNDEFINED_IDX;
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
		tScopeVar *irv = SCOPE_INFO_NODE(AST_NODE(idxAstNode)->idxScope)->getInfoRegisteredSymbol(function_name,true);
		if(irv != NULL){

			CScriptFunctionObject *info_symbol = CScriptFunctionObject::newScriptFunctionObject();

			info_symbol->object_info.symbol_info.idxAstNode = irv->idxAstNode;
			//info_symbol.object_info.symbol_info.idxScopeVar = irv->idxScopeVar;
			info_symbol->object_info.symbol_info.symbol_name = name;

			this->m_currentFunctionInfo->function_info_object->object_info.local_symbols.vec_idx_registeredFunction.push_back(info_symbol->object_info.idxScriptFunctionObject);

			return info_symbol;
		}

	}else{
		print_error_cr("function symbol \"%s\" defined at line %i not defined!",function_name.c_str(), AST_NODE(idxAstNode)->line_value);
	}
	return NULL;
}

bool CCompiler::functionSymbolExists(const string & name, short idxAstNode){

	unsigned int scope_type;
	return getIdxFunctionObject(name,idxAstNode,scope_type,false) != ZS_UNDEFINED_IDX;
}

int  CCompiler::getIdxFunctionObject(const string & name,short idxAstNode, unsigned int & scope_type, bool print_msg){
	tScopeVar *irv=SCOPE_INFO_NODE(AST_NODE(idxAstNode)->idxScope)->getInfoRegisteredSymbol(name);
	scope_type = INS_PROPERTY_LOCAL_SCOPE;
	if(irv != NULL){

		PASTNode ast = AST_NODE(irv->idxAstNode);

		if((ast != NULL) && (AST_NODE(idxAstNode)->idxScope == ast->idxScope)){
			for(unsigned i = 0; i < this->m_currentFunctionInfo->function_info_object->object_info.local_symbols.vec_idx_registeredFunction.size(); i++){
				if(GET_SCRIPT_FUNCTION_OBJECT(m_currentFunctionInfo->function_info_object->object_info.local_symbols.vec_idx_registeredFunction[i])->object_info.symbol_info.symbol_name == name ){
					return i;
				}
			}
		}
		else{ //global

			scope_type = 0;//INST_PROPERTY_GLOBAL_SCOPE;
			CScriptFunctionObject *main_function = GET_SCRIPT_FUNCTION_OBJECT(GET_MAIN_SCRIPT_FUNCTION_IDX);


			for(unsigned i = 0; i < main_function->object_info.local_symbols.vec_idx_registeredFunction.size(); i++){
				if(GET_SCRIPT_FUNCTION_OBJECT(main_function->object_info.local_symbols.vec_idx_registeredFunction[i])->object_info.symbol_info.symbol_name == name ){
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
			case INS_PROPERTY_TYPE_INTEGER:
				//delete (int *)icv->varRef;
				break;
			case INS_PROPERTY_TYPE_BOOLEAN:
				//delete (bool *)icv->stkValue;
				break;
			case INS_PROPERTY_TYPE_NUMBER:
				//delete (float *)icv->stkValue;
				break;
			case INS_PROPERTY_TYPE_STRING:
				delete (string *)icv->stkValue;
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
	def_operator[NOP]         ={"NOP" ,NOP ,0      };
	def_operator[STORE]         ={"STORE" ,STORE ,2}; // mov expression to var
	def_operator[LOAD]        ={"LOAD",LOAD ,1}; // primitive value like number/string or boolean...
	def_operator[EQU]         ={"EQU" ,EQU ,2};  // ==
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
	def_operator[OBJECT_ACCESS]={"OBJECT_ACCESS",OBJECT_ACCESS,2}; // New object (CREATE)
	//def_operator[SAVE_I]={"SAVE_I",SAVE_I,0}; // New object (CREATE)
	//def_operator[LOAD_I]={"LOAD_I",LOAD_I,0}; // New object (CREATE)
	def_operator[POP_SCOPE]={"POP_SCOPE",POP_SCOPE,1}; // New object (CREATE)
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
		print_error_cr("Node is NULL!");
		return false;
	}

	return node->node_type == FUNCTION_OBJECT_NODE || (node->node_type==FUNCTION_REF_NODE);
}

int CCompiler::getIdxArgument(const string & var){
	// search if symbol belongs to arg vector...
	for(unsigned i = 0; i < this->m_currentFunctionInfo->function_info_object->m_arg.size(); i++){

		if(this->m_currentFunctionInfo->function_info_object->m_arg[i] == var){
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

	unsigned int type=INS_PROPERTY_TYPE_STRING;
	CCompiler::tInfoConstantValue *get_obj;
	void *obj;

	if((get_obj = getConstant(v))!=NULL){
		obj = get_obj;
	}else{

		obj=addConstant(v,new string(v),type);
	}

	tInfoAsmOpCompiler *asm_op = new tInfoAsmOpCompiler();

	asm_op->var_type=type;
	asm_op->index_op1=LOAD_TYPE_CONSTANT;
	asm_op->index_op2=(intptr_t)obj;
	asm_op->idxAstNode=idxAstNode;
	asm_op->operator_type=ASM_OPERATOR::LOAD;
	ptr_current_statement_op->asm_op.push_back(asm_op);
}

bool CCompiler::insertLoadValueInstruction(short idxAstNode, CScope * _lc){
	PASTNode _node=AST_NODE(idxAstNode);
	CScope *_scope = SCOPE_INFO_NODE(_node->idxScope);
	string v = _node->symbol_value;

	if(_scope == NULL){
		print_error_cr("error scope null");
		return false;
	}

	// ignore node this ...
	if(_node->symbol_value == "this"){

		print_error_cr("\"%s\" cannot be processed here!",_node->symbol_value.c_str());
		return false;
	}

	unsigned int pre_post_operator_type =0;//INS_PROPERTY_UNKNOW_PRE_POST_OPERATOR;
	tInfoStatementOpCompiler *ptr_current_statement_op = &(this->m_currentFunctionInfo->stament)[this->m_currentFunctionInfo->stament.size()-1];
	void *const_obj;
	void *obj;
	CCompiler::tInfoConstantValue *get_obj;
	unsigned short type=INS_PROPERTY_TYPE_SCRIPTVAR;
	LOAD_TYPE load_type=LOAD_TYPE_NOT_DEFINED;
	unsigned int scope_type=0;//INS_PROPERTY_UNKNOWN_SCOPE;
	bool is_constant = true;



	pre_post_operator_type=preoperator2instruction_property(_node->pre_post_operator_info);

	// try parse value...
	if(v=="null"){
		type=INS_PROPERTY_TYPE_NULL;
		load_type=LOAD_TYPE_NULL;
		obj=NULL_SYMBOL;//CScriptVariable::NullSymbol;
		print_com_cr("%s detected as null\n",v.c_str());
	}else if(v=="undefined"){
			type=INS_PROPERTY_TYPE_UNDEFINED;
			load_type=LOAD_TYPE_UNDEFINED;
			obj=UNDEFINED_SYMBOL;// CScriptVariable::UndefinedSymbol;
			print_com_cr("%s detected as undefined\n",v.c_str());

	}else if((const_obj=CStringUtils::ParseInteger(v))!=NULL){
		intptr_t value = *((int *)const_obj);
		delete (int *)const_obj;

		type=INS_PROPERTY_TYPE_INTEGER;
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


		type=INS_PROPERTY_TYPE_NUMBER;
		load_type=LOAD_TYPE_CONSTANT;
		print_com_cr("%s detected as float\n",v.c_str());

		if((get_obj = getConstant(v))!=NULL){
			obj = get_obj;
		}else{
			obj=addConstant(v,value_ptr,type);
		}
	}
	else if(v[0]=='\"' && v[v.size()-1]=='\"'){
		type=INS_PROPERTY_TYPE_STRING;
		load_type=LOAD_TYPE_CONSTANT;
		print_com_cr("%s detected as string\n",v.c_str());

		if((get_obj = getConstant(v))!=NULL){
			obj = get_obj;
		}else{
			string s=v.substr(1,v.size()-2);
			obj=addConstant(v,new string(s),type);
		}
	}
	else if((const_obj=CStringUtils::ParseBoolean(v))!=NULL){

		bool value = *((bool *)const_obj);
		delete (bool *)const_obj;

		type=INS_PROPERTY_TYPE_BOOLEAN;
		load_type=LOAD_TYPE_CONSTANT;
		print_com_cr("%s detected as boolean\n",v.c_str());

		if((get_obj = getConstant(v))!=NULL){
			obj = get_obj;
		}else{
			obj=addConstant(v,new bool(value),type);
		}
	}else{

		is_constant = false;
		string symbol_name = _node->symbol_value;

		intptr_t idx_local_var=ZS_UNDEFINED_IDX;

		if(checkAccessObjectMember(_node->idxAstNode)){
			scope_type = INS_PROPERTY_ACCESS_SCOPE;


			if((_node->idxAstParent != ZS_UNDEFINED_IDX) && (AST_NODE(AST_NODE(_node->idxAstParent)->children[0])->symbol_value == "this") //|| // single access ?
			){
				scope_type=INS_PROPERTY_THIS_SCOPE;
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
				else{ // ... if not argument finally, we deduce that the value is a local symbol... check whether it exist in the current scope...
					if(_node->node_type == SYMBOL_NODE){
						if(!_scope->existRegisteredSymbol(symbol_name)){
							print_error_cr("line %i: variable \"%s\" not defined",_node->line_value,symbol_name.c_str());
							return false;
						}
					}
				}
			}
			obj = (CScriptVariable *)idx_local_var;
	}

	if((pre_post_operator_type !=0 && GET_INS_PROPERTY_PRE_POST_OP(pre_post_operator_type) !=INS_PROPERTY_PRE_NEG) &&
		is_constant){
		print_error_cr("line %i: operation \"%s\" not allowed for constants ",_node->line_value,CASTNode::defined_operator_punctuator[_node->pre_post_operator_info].str);
		return false;
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

	return true;
}

bool CCompiler::insertMovVarInstruction(short idxAstNode,int left_index, int right_index){

	tInfoStatementOpCompiler *ptr_current_statement_op = &(this->m_currentFunctionInfo->stament)[this->m_currentFunctionInfo->stament.size()-1];
	tInfoAsmOpCompiler * left_asm_op = ptr_current_statement_op->asm_op[left_index];

	// check whether left operant is object...
	if(left_asm_op->var_type != INS_PROPERTY_TYPE_SCRIPTVAR){
		int line = -1;

		if(left_asm_op->idxAstNode!=ZS_UNDEFINED_IDX)
			line=AST_LINE_VALUE(left_asm_op->idxAstNode);
		print_error_cr("line %i. left operand must be l-value for '=' operator",line);
		return false;
	}

	tInfoAsmOpCompiler *asm_op = new tInfoAsmOpCompiler();
	asm_op->index_op1 = left_index;//&(this->m_currentFunctionInfo->stament[dest_statment]);
	asm_op->index_op2 =  right_index;
	asm_op->idxAstNode = idxAstNode;
	//asm_op->symbol_name="";
	asm_op->operator_type=ASM_OPERATOR::STORE;

	ptr_current_statement_op->asm_op.push_back(asm_op);
	return true;
}

CCompiler::tInfoAsmOpCompiler * CCompiler::insert_JMP_Instruction(int jmp_statement, int instruction_index){

	tInfoStatementOpCompiler *ptr_current_statement_op = &this->m_currentFunctionInfo->stament[this->m_currentFunctionInfo->stament.size()-1];
	tInfoAsmOpCompiler *asm_op = new tInfoAsmOpCompiler();
	asm_op->index_op1 = instruction_index;
	asm_op->index_op2 = jmp_statement;//&(this->m_currentFunctionInfo->stament[dest_statment]);
	asm_op->operator_type=ASM_OPERATOR::JMP;
	ptr_current_statement_op->asm_op.push_back(asm_op);
	return asm_op;
}

CCompiler::tInfoAsmOpCompiler * CCompiler::insert_JNT_Instruction(int jmp_statement, int instruction_index){

	tInfoStatementOpCompiler *ptr_current_statement_op = &this->m_currentFunctionInfo->stament[this->m_currentFunctionInfo->stament.size()-1];
	tInfoAsmOpCompiler *asm_op = new tInfoAsmOpCompiler();
	asm_op->index_op1 = instruction_index;
	asm_op->index_op2 = jmp_statement;//&(this->m_currentFunctionInfo->stament[dest_statment]);
	asm_op->operator_type=ASM_OPERATOR::JNT;
	ptr_current_statement_op->asm_op.push_back(asm_op);

	return asm_op;
}

CCompiler::tInfoAsmOpCompiler * CCompiler::insert_JT_Instruction(int jmp_statement, int instruction_index){

	tInfoStatementOpCompiler *ptr_current_statement_op = &this->m_currentFunctionInfo->stament[this->m_currentFunctionInfo->stament.size()-1];
	tInfoAsmOpCompiler *asm_op = new tInfoAsmOpCompiler();
	asm_op->operator_type=ASM_OPERATOR::JT;
	asm_op->index_op1 = instruction_index;
	asm_op->index_op2 = jmp_statement;
	ptr_current_statement_op->asm_op.push_back(asm_op);
	return asm_op;
}

void CCompiler::insert_NOP_Instruction(){
	tInfoStatementOpCompiler *ptr_current_statement_op = &this->m_currentFunctionInfo->stament[this->m_currentFunctionInfo->stament.size()-1];
	tInfoAsmOpCompiler *asm_op = new tInfoAsmOpCompiler();
	asm_op->index_op1 = 0;//&(this->m_currentFunctionInfo->stament[dest_statment]);
	asm_op->operator_type=ASM_OPERATOR::NOP;
	ptr_current_statement_op->asm_op.push_back(asm_op);
}

void CCompiler::insert_CreateArrayObject_Instruction(short idxAstNode){
	tInfoStatementOpCompiler *ptr_current_statement_op = &this->m_currentFunctionInfo->stament[this->m_currentFunctionInfo->stament.size()-1];
	tInfoAsmOpCompiler *asm_op = new tInfoAsmOpCompiler();

	asm_op->operator_type=ASM_OPERATOR::DECL_VEC;
	asm_op->var_type=INS_PROPERTY_TYPE_SCRIPTVAR;
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
	asm_op->var_type = INS_PROPERTY_TYPE_SCRIPTVAR;
	ptr_current_statement_op->asm_op.push_back(asm_op);
}



/*
void CCompiler::insert_StartArgumentStack_Instruction(short idxAstNode){
	tInfoStatementOpCompiler *ptr_current_statement_op = &this->m_currentFunctionInfo->stament[this->m_currentFunctionInfo->stament.size()-1];
	tInfoAsmOpCompiler *asm_op = new tInfoAsmOpCompiler();
	asm_op->operator_type=ASM_OPERATOR::START_ARG;
	asm_op->idxAstNode = idxAstNode;
	ptr_current_statement_op->asm_op.push_back(asm_op);

}*/

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

	// if type return is object return first index
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
		print_error_cr("class \"%s\" is not registered", class_name.c_str());
		return false;
	}
	asm_op->operator_type=ASM_OPERATOR::NEW;
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


void CCompiler::insertPopScopeInstruction(short idxAstNode,int scope_idx){
	tInfoStatementOpCompiler *ptr_current_statement_op = &this->m_currentFunctionInfo->stament[this->m_currentFunctionInfo->stament.size()-1];
	tInfoAsmOpCompiler *asm_op = new tInfoAsmOpCompiler();
	asm_op->index_op1 = scope_idx;
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
unsigned int CCompiler::preoperator2instruction_property(PUNCTUATOR_TYPE op){

	switch(op){
	default:
//		print_error_cr("Cannot match pre/post operator %i!",op);
		break;
	case PUNCTUATOR_TYPE::PRE_INC_PUNCTUATOR:
		return INS_PROPERTY_PRE_INC;
	case PUNCTUATOR_TYPE::POST_INC_PUNCTUATOR:
		return INS_PROPERTY_POST_INC;
	case PUNCTUATOR_TYPE::PRE_DEC_PUNCTUATOR:
		return INS_PROPERTY_PRE_DEC;
	case PUNCTUATOR_TYPE::POST_DEC_PUNCTUATOR:
		return INS_PROPERTY_POST_DEC;
	case PUNCTUATOR_TYPE::SUB_PUNCTUATOR:
		return INS_PROPERTY_PRE_NEG;

	}

	return 0;//INS_PROPERTY_UNKNOW_PRE_POST_OPERATOR;
}

ASM_OPERATOR CCompiler::puntuator2instruction(PUNCTUATOR_TYPE op){

	switch(op){
	default:
		print_error_cr("%s Not implemented", CASTNode::defined_operator_punctuator[op].str);
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

	}
	return NOP;
}

bool CCompiler::insertOperatorInstruction(PUNCTUATOR_TYPE op,short idxAstNode,  string & error_str, int op_index_left, int op_index_right){

	PASTNode _node=AST_NODE( idxAstNode);

	tInfoStatementOpCompiler *ptr_current_statement_op = &this->m_currentFunctionInfo->stament[this->m_currentFunctionInfo->stament.size()-1];
	tInfoAsmOpCompiler * left_asm_op = ptr_current_statement_op->asm_op[op_index_left];
	tInfoAsmOpCompiler *iao;

	// special cases ...
	switch(op){
		case ADD_ASSIGN_PUNCTUATOR: // a+b

			iao = new tInfoAsmOpCompiler();
			iao->operator_type = ASM_OPERATOR::ADD;
			iao->index_op1 = op_index_left;
			iao->index_op2 = op_index_right;
			iao->idxAstNode=_node->idxAstNode;

			ptr_current_statement_op->asm_op.push_back(iao);

			op_index_right+=1;
			op = ASSIGN_PUNCTUATOR;
			break;
		case SUB_ASSIGN_PUNCTUATOR: // a + (-b)

			iao = new tInfoAsmOpCompiler();
			iao->operator_type = ASM_OPERATOR::NEG;
			iao->index_op1 = op_index_right;
			iao->idxAstNode=_node->idxAstNode;
			ptr_current_statement_op->asm_op.push_back(iao);

			iao = new tInfoAsmOpCompiler();
			iao->operator_type = ASM_OPERATOR::ADD;
			iao->index_op1 = op_index_left;
			iao->index_op2 = op_index_right+1;
			iao->idxAstNode=_node->idxAstNode;
			ptr_current_statement_op->asm_op.push_back(iao);

			op_index_right+=2;
			op = ASSIGN_PUNCTUATOR;
			break;
		case MUL_ASSIGN_PUNCTUATOR: // a*b

			iao = new tInfoAsmOpCompiler();
			iao->operator_type = ASM_OPERATOR::MUL;
			iao->index_op1 = op_index_left;
			iao->index_op2 = op_index_right;
			iao->idxAstNode=_node->idxAstNode;
			ptr_current_statement_op->asm_op.push_back(iao);

			op_index_right+=1;
			op = ASSIGN_PUNCTUATOR;
			break;
		case DIV_ASSIGN_PUNCTUATOR: // a/b

			iao = new tInfoAsmOpCompiler();
			iao->operator_type = ASM_OPERATOR::DIV;
			iao->index_op1 = op_index_left;
			iao->index_op2 = op_index_right;
			iao->idxAstNode=_node->idxAstNode;
			ptr_current_statement_op->asm_op.push_back(iao);

			op_index_right+=1;
			op = ASSIGN_PUNCTUATOR;
			break;
		case MOD_ASSIGN_PUNCTUATOR: // a % b

			iao = new tInfoAsmOpCompiler();
			iao->operator_type = ASM_OPERATOR::MOD;
			iao->index_op1 = op_index_left;
			iao->index_op2 = op_index_right;
			iao->idxAstNode=_node->idxAstNode;
			ptr_current_statement_op->asm_op.push_back(iao);

			op_index_right+=1;
			op = ASSIGN_PUNCTUATOR;
			break;
		default:
			break;
	}

	if((op == ASSIGN_PUNCTUATOR) && (left_asm_op->var_type != INS_PROPERTY_TYPE_SCRIPTVAR)){

			error_str = "left operand must be l-value for '=' operator";
			return false;
	}

	if(op == PUNCTUATOR_TYPE::FIELD_PUNCTUATOR){ // trivial access...
		return true;
	}
	ASM_OPERATOR asm_op;
	if((asm_op= puntuator2instruction(op))!=NOP){
		iao = new tInfoAsmOpCompiler();
		iao->operator_type = asm_op;
		iao->index_op1 = op_index_left;
		iao->index_op2 = op_index_right;

		if(_node!=NULL){
			iao->idxAstNode=_node->idxAstNode;
		}
		ptr_current_statement_op->asm_op.push_back(iao);
		return true;
	}
	return false;
}

/*
CCompiler::tInfoAsmOpCompiler * CCompiler::insert_Save_CurrentInstruction(){

	tInfoStatementOpCompiler *ptr_current_statement_op = &this->m_currentFunctionInfo->stament[this->m_currentFunctionInfo->stament.size()-1];
	tInfoAsmOpCompiler *asm_op = new tInfoAsmOpCompiler();
	asm_op->operator_type=ASM_OPERATOR::SAVE_I;
	ptr_current_statement_op->asm_op.push_back(asm_op);
	return asm_op;
}

CCompiler::tInfoAsmOpCompiler * CCompiler::insert_Load_CurrentInstruction(){

	tInfoStatementOpCompiler *ptr_current_statement_op = &this->m_currentFunctionInfo->stament[this->m_currentFunctionInfo->stament.size()-1];
	tInfoAsmOpCompiler *asm_op = new tInfoAsmOpCompiler();
	asm_op->operator_type=ASM_OPERATOR::LOAD_I;
	ptr_current_statement_op->asm_op.push_back(asm_op);
	return asm_op;
}*/

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
			print_error_cr("Calling object should have at least 1 children");
			return ZS_UNDEFINED_IDX;
		}
	}

	if(array_access){
		return gacExpression_ArrayAccess(_node_access->idxAstNode, _lc);
	}else if(function_access){
		return gacExpression_FunctionAccess(_node_access->idxAstNode, _lc);
	}

	print_error_cr("Expected function or array access");
	return ZS_UNDEFINED_IDX;

}


int CCompiler::gacExpression_ArrayAccess(short idxAstNode, CScope *_lc){

	PASTNode _node = AST_NODE(idxAstNode);

	if(_node == NULL) {print_error_cr("NULL node");return ZS_UNDEFINED_IDX;}
	if(_node->node_type != CALLING_OBJECT_NODE ){print_error_cr("node is not CALLING_OBJECT_NODE type or null");return ZS_UNDEFINED_IDX;}
	if(!(_node->children.size()==2 || _node->children.size()==3)) {print_error_cr("Array access should have 2 children");return ZS_UNDEFINED_IDX;}


	PASTNode node_0=AST_NODE(_node->children[0]),
			 node_1=AST_NODE(_node->children[1]),
			 node_2=NULL;

	if(_node->children.size()==3){
		node_2 = AST_NODE(_node->children[2]);
	}


	if(node_0->node_type != ARRAY_REF_NODE && node_0->node_type != ARRAY_OBJECT_NODE ){print_error_cr("Node is not ARRAY_OBJECT type"); return ZS_UNDEFINED_IDX;}
	if(node_1->node_type != ARRAY_ACCESS_NODE || node_1->children.size() == 0){print_error_cr("Array has no index nodes "); return ZS_UNDEFINED_IDX;}

	int vec=0;

	if(node_0->node_type == ARRAY_OBJECT_NODE){ // must first create the object ...
		if((vec=gacExpression_ArrayObject(node_0->idxAstNode,_lc))==ZS_UNDEFINED_IDX){
			return ZS_UNDEFINED_IDX;
		}
	}else{
		if(node_0->symbol_value != "--"){ // starts with symbol ...
			if(!insertLoadValueInstruction(node_0->idxAstNode,  _lc)){
				return ZS_UNDEFINED_IDX;
			}
		}
		vec = CCompiler::getCurrentInstructionIndex();
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
				print_error_cr("Expected 1 children");
				return ZS_UNDEFINED_IDX;
			}
		}else{
			print_error_cr("Node not ARRAY_INDEX_NODE");
			return ZS_UNDEFINED_IDX;
		}
	}

	if(_node->pre_post_operator_info != PUNCTUATOR_TYPE::UNKNOWN_PUNCTUATOR){ // there's pre/post increment...
		// get post/inc
		tInfoStatementOpCompiler *ptr_current_statement_op = &this->m_currentFunctionInfo->stament[this->m_currentFunctionInfo->stament.size()-1];
		tInfoAsmOpCompiler *asm_op = ptr_current_statement_op->asm_op[ptr_current_statement_op->asm_op.size()-1];
		asm_op->pre_post_op_type=preoperator2instruction_property(_node->pre_post_operator_info);

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

int CCompiler::gacExpression_ArrayObject(short idxAstNode, CScope *_lc){

	PASTNode _node=AST_NODE(idxAstNode);

	if(_node == NULL) {print_error_cr("NULL node");return ZS_UNDEFINED_IDX;}
	if(_node->node_type != ARRAY_OBJECT_NODE ){print_error_cr("node is not ARRAY_OBJECT_NODE type or null");return ZS_UNDEFINED_IDX;}
	int r=0;

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
		if((r=gacExpression(_node->children[j], _lc,ini_instruction)) == ZS_UNDEFINED_IDX){
			return ZS_UNDEFINED_IDX;
		}
		insert_ArrayObject_PushValueInstruction(_node->idxAstNode,index_created_vec,ini_instruction);
	}
	return index_created_vec;//CCompiler::getCurrentInstructionIndex();
}

int CCompiler::gacExpression_FunctionObject(short idxAstNode, CScope *_lc){

	PASTNode _node=AST_NODE(idxAstNode);

	if(_node == NULL) {print_error_cr("NULL node");return ZS_UNDEFINED_IDX;}
	if(_node->node_type != FUNCTION_OBJECT_NODE ){print_error_cr("node is not FUNCTION_OBJECT_NODE type or null");return ZS_UNDEFINED_IDX;}
	if(_node->children.size()!=2) {print_error_cr("Array access should have 2 children");return ZS_UNDEFINED_IDX;}

	CScriptFunctionObject * script_function=NULL;

	// 1. insert load reference created object ...
	if(functionSymbolExists(_node->symbol_value, _node->idxAstNode)){
			print_error_cr("Function \"%s\" already defined !",_node->symbol_value.c_str());
			return false;
	}

	if((script_function=addLocalFunctionSymbol(_node->symbol_value,_node->idxAstNode)) == NULL){
		return false;
	}

	// compiles anonymous function ...
	if(!gacFunctionOrOperator(_node->idxAstNode,_lc, script_function)){
		return ZS_UNDEFINED_IDX;
	}

	return insertLoadValueInstruction(_node->idxAstNode, _lc);
}

int CCompiler::gacExpression_FunctionAccess(short idxAstNode, CScope *_lc){

	PASTNode _node=AST_NODE(idxAstNode);

	if(_node == NULL) {print_error_cr("NULL node");return ZS_UNDEFINED_IDX;}
	if(_node->node_type != CALLING_OBJECT_NODE ){print_error_cr("node is not CALLING_OBJECT_NODE type or null");return ZS_UNDEFINED_IDX;}
	if(!(_node->children.size()==2 || _node->children.size()==3)) {print_error_cr("Array access should have 2 or 3 children");return ZS_UNDEFINED_IDX;}

	PASTNode node_0=AST_NODE(_node->children[0]),
			 node_1=AST_NODE(_node->children[1]),
			 node_2=NULL;

	if(_node->children.size()==3){
		node_2 = AST_NODE(_node->children[2]);
	}


	if(node_0->node_type != FUNCTION_REF_NODE && node_0->node_type != FUNCTION_OBJECT_NODE){print_error_cr("Node is not FUNCTION_OBJECT_NODE type"); return ZS_UNDEFINED_IDX;}
	if(node_1->node_type != ARGS_PASS_NODE){print_error_cr("Function has no index nodes "); return ZS_UNDEFINED_IDX;}

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

			/*if(k==0){
				// insert clear push arguments stack

			}else{
				// insert vector access instruction ...
				insert_PushArgument_Instruction(_node->idxAstNode);
			}*/
		}
	}
	/*}else{
		// clear the stack only ..
		insert_ClearArgumentStack_Instruction(_node->idxAstNode);
	}*/

	// 2. insert call instruction itself.
	insert_CallFunction_Instruction(_node->idxAstNode,call_index);

	if(_node->children.size()==3){ // array or function access ...

		return gacExpression_FunctionOrArrayAccess(node_2->idxAstNode, _lc);
	}

	return CCompiler::getCurrentInstructionIndex();
}

int CCompiler::gacExpression_StructAttribute(short idxAstNode, CScope *_lc, int index_ref_object){

	PASTNode _node = AST_NODE(idxAstNode);

	if(_node == NULL) {print_error_cr("NULL node");return ZS_UNDEFINED_IDX;}
	if(_node->node_type != EXPRESSION_NODE ){print_error_cr("node is not EXPRESSION_NODE type or null");return ZS_UNDEFINED_IDX;}


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

	if(_node == NULL) {print_error_cr("NULL node");return ZS_UNDEFINED_IDX;}
	if(_node->node_type != STRUCT_NODE ){print_error_cr("node is not STRUCT_NODE type or null");return ZS_UNDEFINED_IDX;}

	insert_DeclStruct_Instruction(_node->idxAstNode);
	int ref_obj = getCurrentInstructionIndex();

	for(unsigned i = 0; i < _node->children.size(); i++){
		// evaluate attribute
		if(gacExpression_StructAttribute(_node->children[i], _lc,ref_obj) == ZS_UNDEFINED_IDX){
			return ZS_UNDEFINED_IDX;
		}
	}

	// 2st push create and push attribute


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
				print_error_cr("Calling object should have at least 1 children");
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
						if((r=gacExpression_ArrayObject(_node->idxAstNode, _lc)) == ZS_UNDEFINED_IDX){
							return ZS_UNDEFINED_IDX;
						}

						index_instruction = r;
						return r;
						break;

					case FUNCTION_OBJECT_NODE: // should have 1 children
						if((!gacExpression_FunctionObject(_node->idxAstNode, _lc))){
							return ZS_UNDEFINED_IDX;
						}
						r=CCompiler::getCurrentInstructionIndex();
						break;

					case NEW_OBJECT_NODE:
						if((!gacNew(_node->idxAstNode, _lc))){
							return ZS_UNDEFINED_IDX;
						}

						r=CCompiler::getCurrentInstructionIndex();
						break;
					case STRUCT_NODE:
						if((r=gacExpression_Struct(_node->idxAstNode, _lc))==ZS_UNDEFINED_IDX){
							return ZS_UNDEFINED_IDX;
						}
						//r=CCompiler::getCurrentInstructionIndex();
						break;
					default:
						print_error_cr("Unexpected node type %i",eval_node_sp->node_type);
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
			print_error_cr("Malformed expression at line %i",_node->line_value);
			return ZS_UNDEFINED_IDX;
		}

		if(_node->operator_info == TERNARY_IF_PUNCTUATOR){
			if(AST_NODE(_node->children[1])->operator_info == TERNARY_ELSE_PUNCTUATOR){
				// node children[0]: conditional.
				// node children[1]: body-if
				// node children[2]: body-else
				//inline_if_else = true;
				print_error_cr("Ternary operator not implemented!!!!");
				return ZS_UNDEFINED_IDX;

				int t1= gacInlineIf(_node->idxAstNode,_lc,index_instruction);

				return t1;

			}else{
				print_error_cr("line %i: Put parenthesis on the inner ternary conditional",_node->line_value);
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
						print_error_cr("%s at line %i",error_str.c_str(),_node->line_value);
						return ZS_UNDEFINED_IDX;
					}
				}

			}else if(right!=ZS_UNDEFINED_IDX){ // one op..
				if(!insertOperatorInstruction(_node->operator_info,_node->idxAstNode,  error_str,right)){
					print_error_cr("%s at line %i",error_str.c_str(),_node->line_value);
					return ZS_UNDEFINED_IDX;
				}

			}else if(left!=ZS_UNDEFINED_IDX){ // one op..
				if(!insertOperatorInstruction(_node->operator_info,_node->idxAstNode,error_str,left)){
					print_error_cr("%s at line %i",error_str.c_str(),_node->line_value);
					return ZS_UNDEFINED_IDX;
				}
			}else{ // ERROR
				print_error_cr("ERROR both ops ==0!");
				return ZS_UNDEFINED_IDX;
			}
		}
	}

	// we ignore field node instruction ...
	if(!access_node){
		index_instruction++;
	}


	return r;
}

int findConstructorIdxNode(short idxAstNode){

	PASTNode _node=AST_NODE(idxAstNode);

	if(_node->node_type!=NODE_TYPE::ARGS_PASS_NODE) {print_error_cr("children[0] is not args_pass_node");return ZS_UNDEFINED_IDX;}
	for(unsigned i = 0; i < _node->children.size(); i++){
		PASTNode child_node = AST_NODE(_node->children[i]);
		if(child_node->node_type == NODE_TYPE::KEYWORD_NODE){

			if(child_node->keyword_info==KEYWORD_TYPE::FUNCTION_KEYWORD || child_node->keyword_info==KEYWORD_TYPE::OPERATOR_KEYWORD){
				if(child_node->symbol_value == _node->symbol_value){
					return i;
				}
			}

		}
	}
	return ZS_UNDEFINED_IDX;
}

bool CCompiler::doRegisterVariableSymbolsClass(const string & class_name, CScriptClass *current_class){

	if(current_class == NULL){
		return true;
	}

	if(current_class->baseClass.size() !=0){
		for(int i = 0; i < (int)current_class->baseClass.size() ; i++){
			if(!CCompiler::doRegisterVariableSymbolsClass(class_name,current_class->baseClass.at(i))){
				return false;
			}
		}
	}

	PASTNode node_class = AST_NODE(current_class->metadata_info.object_info.symbol_info.idxAstNode);
	string current_class_name = current_class->metadata_info.object_info.symbol_info.symbol_name;

	// register all vars...
	for(unsigned i = 0; i < AST_NODE(node_class->children[0])->children.size(); i++){ // foreach declared var.

		PASTNode child_node = AST_NODE(AST_NODE(node_class->children[0])->children[i]);

		for(unsigned j = 0; j < child_node->children.size(); j++){ // foreach element declared within ','
			if(CScriptClass::registerVariableSymbol(
					class_name,
					AST_NODE(child_node->children[j])->symbol_value,
					child_node->children[j]
				) == NULL){
				return false;
			}
		}
	}

	// register all functions ...
	for(unsigned i = 0; i < AST_NODE(node_class->children[1])->children.size(); i++){
		CScriptFunctionObject *irfs;
		PASTNode node_fun = AST_NODE(AST_NODE(node_class->children[1])->children[i]);
		string symbol_value = node_fun->symbol_value;

		if(current_class_name == symbol_value){ // constructor symbol...
			symbol_value = class_name; // rename to be base constructor later ...
		}

		if((irfs=CScriptClass::registerFunctionSymbol(
				class_name,
				symbol_value,
				node_fun->idxAstNode
		)) == NULL){
			return false;
		}

		// compile function (within scope class)...
		if(!gacFunctionOrOperator(node_fun->idxAstNode, SCOPE_INFO_NODE(node_class->idxScope),irfs)){
			return false;
		}
	}
	return true;
}

bool CCompiler::gacClass(short idxAstNode, CScope * _lc){

	PASTNode _node = AST_NODE(idxAstNode);

	if(_node == NULL) {print_error_cr("NULL node");return false;}
	if(_node->node_type != KEYWORD_NODE){print_error_cr("node is not keyword type or null");return false;}
	if(_node->keyword_info != KEYWORD_TYPE::CLASS_KEYWORD){print_error_cr("node is not CLASS keyword type");return false;}
	if(_node->children.size()==3 && AST_NODE(_node->children[2])->node_type != BASE_CLASS_NODE){print_error_cr("expected BASE CLASS keyword type");return false;}

	string base_class= "";
	if(_node->children.size()==3){
		base_class=	AST_NODE(_node->children[2])->symbol_value;
	}

	CScriptClass *irc;

	// children[0]==var_collection && children[1]=function_collection
	if(_node->children.size()!=2 && _node->children.size()!=3) {print_error_cr("node CLASS has not valid number of nodes");return false;}

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
	// pop class ref so we go back to main scope...
	popFunction();
	return true;
}

int CCompiler::gacNew(short idxAstNode, CScope * _lc){


	PASTNode _node = AST_NODE(idxAstNode);

	if(_node == NULL) {print_error_cr("NULL node");return ZS_UNDEFINED_IDX;}
	if(_node->node_type != NEW_OBJECT_NODE ){print_error_cr("node is not NEW OBJECT NODE type");return ZS_UNDEFINED_IDX;}
	if(_node->children.size()!=1) {print_error_cr("node NEW has not valid number of nodes");return ZS_UNDEFINED_IDX;}
	if(AST_NODE(_node->children[0])->node_type!=NODE_TYPE::ARGS_PASS_NODE) {print_error_cr("children[0] is not args_pass_node");return ZS_UNDEFINED_IDX;}


	// load function ...

	// 1. insert push to pass values to all args ...
	PASTNode constructor_args = AST_NODE(_node->children[0]);

	if(constructor_args->children.size() > 0){

		//insert_StartArgumentStack_Instruction(_node->idxAstNode);

		for(unsigned k = 0; k < constructor_args->children.size(); k++){

			// check whether is expression node...
			if(!gacExpression(constructor_args->children[k], _lc,getCurrentInstructionIndex()+1)){
				return ZS_UNDEFINED_IDX;
			}

			/*if(k==0){
				// insert clear push arguments stack
				insert_ClearArgumentStack_And_PushFirstArgument_Instructions(_node->idxAstNode);
			}else{
				// insert vector access instruction ...
				insert_PushArgument_Instruction(_node->idxAstNode);
			}*/
		}
	}else{
		// clear the stack only ..
		//insert_ClearArgumentStack_Instruction(_node->idxAstNode);
	}

	// 1. create object instruction ...
	if(!insert_NewObject_Instruction(_node->idxAstNode,_node->symbol_value)) // goto end  ...
	{
		return ZS_UNDEFINED_IDX;
	}

	return CCompiler::getCurrentInstructionIndex();
}

bool CCompiler::gacFor(short idxAstNode, CScope * _lc){

	PASTNode _node=AST_NODE(idxAstNode);

	if(_node == NULL) {print_error_cr("NULL node");return false;}
	if(_node->node_type != KEYWORD_NODE ){print_error_cr("node is not keyword type or null");return false;}
	if(_node->keyword_info != KEYWORD_TYPE::FOR_KEYWORD){print_error_cr("node is not FOR keyword type");return false;}
	if(_node->children.size()!=4) {print_error_cr("node FOR has not valid number of nodes");return false;}
	if(!(AST_NODE(_node->children[0])->node_type==PRE_FOR_NODE && AST_NODE(_node->children[1])->node_type==CONDITIONAL_NODE &&
	AST_NODE(_node->children[2])->node_type==POST_FOR_NODE && AST_NODE(_node->children[3])->node_type==BODY_NODE)) {print_error_cr("node FOR has not valid TYPE nodes");return false;}
	tInfoAsmOpCompiler *asm_op;
	// 1. compile var init ...
	if(AST_NODE(_node->children[0])->children.size()>0){
		if(!ast2asm_Recursive(_node->children[0],SCOPE_INFO_NODE(_node->idxScope))){ return false;}
	}

	// 2. compile conditional
	if(!ast2asm_Recursive(_node->children[1],SCOPE_INFO_NODE(_node->idxScope))){ return false;}
	// get current index statment in order to jmp from end body for.
	int index_statment_conditional_for_= getCurrentStatmentIndex();

	// insert conditional jmp (if not true go to the end)
	asm_op = insert_JNT_Instruction();

	// 3. compile body
	if(!gacBody(_node->children[3],SCOPE_INFO_NODE(AST_NODE(_node->children[3])->idxScope))){ return false;}

	// 4. compile post oper
	if(AST_NODE(_node->children[2])->children.size()>0 && AST_NODE(_node->children[2])->children[0] != ZS_UNDEFINED_IDX){
		if(!ast2asm_Recursive(_node->children[2],SCOPE_INFO_NODE(_node->idxScope))){ return false;}
	}

	// 5. jmp to the conditional index ...
	insert_JMP_Instruction(index_statment_conditional_for_);

	// 6. insert pop scope...
	newStatment();
	int index=_node->idxScope;
	if(index != ZS_UNDEFINED_IDX){
			insertPopScopeInstruction(_node->idxAstNode,index);
	}

	// save jmp value...
	asm_op->index_op2=getCurrentStatmentIndex();
	return true;
}

bool CCompiler::gacWhile(short idxAstNode, CScope * _lc){

	PASTNode _node = AST_NODE(idxAstNode);

	if(_node == NULL) {print_error_cr("NULL node");return false;}
	if(_node->node_type != KEYWORD_NODE){print_error_cr("node is not keyword type or null");return false;}
	if(_node->keyword_info != KEYWORD_TYPE::WHILE_KEYWORD){print_error_cr("node is not WHILE keyword type");return false;}
	if(_node->children.size()!=2) {print_error_cr("node WHILE has not valid number of nodes");return false;}
	if(!(AST_NODE(_node->children[0])->node_type==CONDITIONAL_NODE && AST_NODE(_node->children[1])->node_type==BODY_NODE )) {print_error_cr("node WHILE has not valid TYPE nodes");return false;}
	tInfoAsmOpCompiler *asm_op_jmp_end;
	int index_ini_while;

	// compile conditional expression...
	if(!ast2asm_Recursive(_node->children[0],_lc)){ return false;}
	index_ini_while = getCurrentStatmentIndex();
	asm_op_jmp_end = insert_JNT_Instruction(); // goto end  ...

	// compile if-body ...
	if(!gacBody(_node->children[1],_lc)){ return false;}
	insert_JMP_Instruction(index_ini_while); // goto end  ...

	// save jmp value ...
	asm_op_jmp_end->index_op2= getCurrentStatmentIndex()+1;
	return true;
}

bool CCompiler::gacReturn(short idxAstNode, CScope * _lc){

	PASTNode _node = AST_NODE(idxAstNode);

	if(_node == NULL) {print_error_cr("NULL node");return false;}
	if(_node->node_type != KEYWORD_NODE){print_error_cr("node is not keyword type or null");return false;}
	if(_node->keyword_info != KEYWORD_TYPE::RETURN_KEYWORD){print_error_cr("node is not RETURN keyword type");return false;}
	if(_node->children.size() != 1){print_error_cr("node RETURN has not 1 child");return false;}

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

	if(_node == NULL) {print_error_cr("NULL node");return false;}
	//if(!(_node->node_type == KEYWORD_NODE && _node->keyword_info != NULL) && !(_node->node_type != FUNCTION_OBJECT_NODE))>{print_error_cr("node is not keyword type or null");return false;}


	if(
	    ! ( _node->keyword_info == KEYWORD_TYPE::FUNCTION_KEYWORD
	    ||	_node->keyword_info == KEYWORD_TYPE::OPERATOR_KEYWORD
		||  _node->node_type    == FUNCTION_OBJECT_NODE
		)) {
		print_error_cr("Expected FUNCTION or OPERATOR or FUNCTION_OBJECT_NODE keyword type at line %i",_node->line_value);
		return false;
	}

	//if(!(_node->keyword_info->id == KEYWORD_TYPE::FUNCTION_KEYWORD) && !(_node->node_type != FUNCTION_OBJECT_NODE)){print_error_cr("node is not FUNCTION keyword type");return false;}
	if(_node->children.size() != 2){print_error_cr("node FUNCTION has not 2 child");return false;}
	if(AST_NODE(_node->children[0])->node_type != NODE_TYPE::ARGS_DECL_NODE){print_error_cr("node FUNCTION has not ARGS node");return false;}
	if(AST_NODE(_node->children[1])->node_type != NODE_TYPE::BODY_NODE){print_error_cr("node FUNCTION has not BODY node");return false;}

	// 2. Processing args ...

	for(unsigned i = 0; i < AST_NODE(_node->children[0])->children.size(); i++){
		irfs->m_arg.push_back(AST_NODE(AST_NODE(_node->children[0])->children[i])->symbol_value);
	}
	// 2. Compiles the function ...
	return compile(_node->children[1], irfs);
}

bool CCompiler::gacIf(short idxAstNode, CScope * _lc){

	PASTNode _node = AST_NODE(idxAstNode);

	if(_node == NULL) {print_error_cr("NULL node");return false;}
	if(_node->node_type != KEYWORD_NODE ){print_error_cr("node is not keyword type or null");return false;}
	if(_node->keyword_info != KEYWORD_TYPE::IF_KEYWORD){print_error_cr("node is not IF keyword type");return false;}
	if(_node->children.size()<2) {print_error_cr("node IF has not valid number of nodes");return false;}
	if(!(AST_NODE(_node->children[0])->node_type==CONDITIONAL_NODE && AST_NODE(_node->children[1])->node_type==BODY_NODE )) {print_error_cr("node IF has not valid TYPE nodes");return false;}
	tInfoAsmOpCompiler *asm_op_jmp_else_if,*asm_op_jmp_end;

	// compile conditional expression...
	if(!ast2asm_Recursive(_node->children[0],_lc)){ return false;}
	asm_op_jmp_else_if = insert_JNT_Instruction(); // goto else body ...

	// compile if-body ...
	if(!gacBody(_node->children[1],_lc)){ return false;}


	// if there's else body, compile-it
	if(_node->children.size()==3){
		asm_op_jmp_end = insert_JMP_Instruction(); // goto end
		asm_op_jmp_else_if->index_op2 = getCurrentStatmentIndex()+1;
		if(!gacBody(_node->children[2],_lc)){ return false;}

		asm_op_jmp_end->index_op2 = getCurrentStatmentIndex()+1;
	}
	else{
		asm_op_jmp_else_if->index_op2 = getCurrentStatmentIndex()+1;
	}
	return true;
}

int CCompiler::gacInlineIf(short idxAstNode, CScope * _lc, int & instruction){

	PASTNode _node = AST_NODE(idxAstNode);

	if(_node == NULL) {print_error_cr("NULL node");return ZS_UNDEFINED_IDX;}
	if(_node->node_type != PUNCTUATOR_NODE ){print_error_cr("node is not punctuator type or null");return ZS_UNDEFINED_IDX;}
	if(_node->operator_info != TERNARY_IF_PUNCTUATOR){print_error_cr("node is not INLINE-IF PUNCTUATOR type");return ZS_UNDEFINED_IDX;}
	if(_node->children.size()!=2) {print_error_cr("node INLINE-IF has not 2 nodes");return ZS_UNDEFINED_IDX;}
	if(!(AST_NODE(_node->children[1])->node_type==PUNCTUATOR_NODE && AST_NODE(_node->children[1])->operator_info==TERNARY_ELSE_PUNCTUATOR )) {print_error_cr("node INLINE-ELSE has not found");return ZS_UNDEFINED_IDX;}
	if(AST_NODE(_node->children[1])->children.size() != 2) {print_error_cr("node INLINE-ELSE has not 2 nodes");return ZS_UNDEFINED_IDX;}
	tInfoAsmOpCompiler *asm_op_jmp_else_if,*asm_op_jmp_end;

	int r=instruction;

	// compile conditional expression...
	if((r=gacExpression_Recursive(_node->children[0],_lc,r))==ZS_UNDEFINED_IDX){ return ZS_UNDEFINED_IDX;}
	asm_op_jmp_else_if = insert_JNT_Instruction(); // goto else body ...

	// compile if-body ...
	r+=2; // in order to execute recursive expression we have to advance r pointer jnt (+2)
	if((r=gacExpression_Recursive(AST_NODE(_node->children[1])->children[0],_lc,r))==ZS_UNDEFINED_IDX){ return ZS_UNDEFINED_IDX;}

	//insert_Save_CurrentInstruction();

	// compile else-body ...
	asm_op_jmp_end = insert_JMP_Instruction(); // goto end+

	asm_op_jmp_else_if->index_op1 = getCurrentInstructionIndex()+1;
	asm_op_jmp_else_if->index_op2 = getCurrentStatmentIndex();


	r+=3;

	if((r=gacExpression_Recursive(AST_NODE(_node->children[1])->children[1],_lc,r))==ZS_UNDEFINED_IDX){ return ZS_UNDEFINED_IDX;}

	//insert_Save_CurrentInstruction();
	//insert_Load_CurrentInstruction();

	r+=2; // add +2 load +save ...

	asm_op_jmp_end->index_op1 = getCurrentStatmentIndex();
	asm_op_jmp_end->index_op2 = getCurrentInstructionIndex();

	instruction = r+1;

	return r;
}

bool CCompiler::gacSwitch(short idxAstNode, CScope * _lc){

	PASTNode _node = AST_NODE(idxAstNode);

	if(_node == NULL) {print_error_cr("NULL node");return false;}
	if(_node->node_type != KEYWORD_NODE ){print_error_cr("node is not keyword type or null");return false;}
	if(_node->keyword_info != SWITCH_KEYWORD){print_error_cr("node is not SWITCH keyword type");return false;}
	bool has_default = false;
	PASTNode switch_node;
	PASTNode group_cases;
	PASTNode case_value;
	PASTNode case_body;

	tInfoAsmOpCompiler * asm_op=NULL;

	string error_str;
	string detected_type_str;
	int idxScope=AST_SCOPE_INFO_IDX(this->m_currentFunctionInfo->function_info_object->object_info.symbol_info.idxAstNode);
	 CScope *_scope = SCOPE_INFO_NODE(idxScope)->getCurrentScopePointer();
	 tInfoAsmOpCompiler *jmp_instruction=NULL;

	// create new statment ...
	CCompiler::getInstance()->newStatment();

	if(ast2asm_Recursive(_node->children[0],_scope)){ // insert condition value ...

		// get current instruction value to take as ref for compare within value cases...
		int switch_value_index  = getCurrentInstructionIndex();

		// the stratege is first evaluate all cases and then their bodies...
		for(unsigned s=0; s < 3; s++){
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

							if(group_cases->node_type == GROUP_CASES_NODE){

								//int condition_index  = getCurrentInstructionIndex();

								for(unsigned j = 0; j < group_cases->children.size(); j++){ // generate condition case ...
									case_value = AST_NODE(group_cases->children[j]);
									jmp_instruction=NULL;

									if(case_value->node_type == KEYWORD_NODE && case_value->keyword_info != KEYWORD_TYPE::UNKNOWN_KEYWORD){

										switch(case_value->keyword_info){
										default:
											print_error_cr("Unexpected %s keyword node in SWITCH node",CASTNode::defined_keyword[case_value->keyword_info].str);
											break;
										case DEFAULT_KEYWORD:

											if(!has_default){
												has_default = true;
												// insert jmp instruction and save its information to store where to jmp when we know the total code size of cases + body...
												jmp_instruction = insert_JMP_Instruction();
											}else{
												print_error_cr("case already defined!");
												return false;
											}
											break;
										case CASE_KEYWORD:

											// load case X:
											insertLoadValueInstruction(case_value->idxAstNode,_scope);

											// is equal ? ==
											if(!insertOperatorInstruction(LOGIC_EQUAL_PUNCTUATOR,0, error_str, switch_value_index ,getCurrentInstructionIndex())){
													print_error_cr("%s",error_str.c_str());
													return false;
											}

											// insert jmp instruction and save its information to store where to jmp when we know the total code size of cases...
											jmp_instruction = insert_JT_Instruction();

											break;

										}

									}else{
										print_error_cr("Not SWITCH case or NULL keyword info");
										return false;
									}
								}
							}else{
								print_error_cr("Expected group cases type node in SWITCH node");
								return false;
							}
							break;

						// Body!
						case 1: // GENERATE ASM FOR BODY AND WRITE initial JMP

							if(gacBody(case_body->idxAstNode,_lc)){
								for(unsigned i = 0; i < group_cases->children.size(); i++){
									case_value = AST_NODE(group_cases->children[i]);
									asm_op = jmp_instruction; // load jt instruction and set current instruction before write asm code.
									asm_op->index_op2 = getCurrentStatmentIndex();

								}

								if(i < (_node->children.size()-1))
									jmp_instruction = insert_JMP_Instruction();
							}else{
								return false;
							}
							break;

						case 2: // FINALLY, WRITE JMP's to end statment

							if((asm_op = (tInfoAsmOpCompiler *)jmp_instruction) != NULL){
								asm_op->index_op2 = getCurrentStatmentIndex()+1;
							}
							break;
						}
					}
					else{
						print_error_cr("SWITCH node has not 2 nodes");
						return false;
					}
				}
				else{
					print_error_cr("SWITCH node NULL");
					return false;
				}
			}
		}
	}else{
		return false;
	}
	return true;
}

bool CCompiler::gacVar(short idxAstNode, CScope * _lc){

	PASTNode _node = AST_NODE(idxAstNode);

	if(_node == NULL) {print_error_cr("NULL node");return false;}
	if(_node->node_type != KEYWORD_NODE ){print_error_cr("node is not keyword type or null");return false;}
	if(_node->keyword_info != VAR_KEYWORD){print_error_cr("node is not VAR keyword type");return false;}

	int local_variable_idx;

	for(unsigned i = 0; i < _node->children.size(); i++){ // for all vars ...

		if(localVarSymbolExists(AST_NODE(_node->children[i])->symbol_value, _node->idxAstNode)){
			print_error_cr("Variable \"%s\" already defined !",_node->symbol_value.c_str());
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
	if(_node == NULL) {print_error_cr("NULL node");return false;}
	if(_node->node_type != KEYWORD_NODE){print_error_cr("node is not keyword type or null");return false;}

	switch(_node->keyword_info){
	default:
		print_error_cr("Keyword [ %s ] not implemented yet!",CASTNode::defined_keyword[_node->keyword_info].str);
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
	case KEYWORD_TYPE::IF_KEYWORD:
		return gacIf(_node->idxAstNode, _lc);
		break;
	case KEYWORD_TYPE::VAR_KEYWORD:
		return gacVar(_node->idxAstNode, _lc);
		break;
	case KEYWORD_TYPE::FUNCTION_KEYWORD: // don't compile function. It will compiled later, after main body
	case KEYWORD_TYPE::OPERATOR_KEYWORD:

		if(_node->keyword_info == KEYWORD_TYPE::OPERATOR_KEYWORD){
			print_error_cr("operator keyword not implemented");
			return false;
		}

		if(functionSymbolExists(_node->symbol_value, _node->idxAstNode)){
				print_error_cr("Function \"%s\" already defined !",_node->symbol_value.c_str());
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

	if(_node == NULL) {print_error_cr("NULL node");return false;}
	if(_node->node_type != BODY_NODE ){print_error_cr("node is not BODY type or null");return false;}

	if(_node->children.size() > 0){ // body has code ...

		for(unsigned i = 0; i < _node->children.size(); i++){
			if(!ast2asm_Recursive(_node->children[i], _lc))
				return false;
		}

	}else{ // no block. Insert one statment at least ..
		newStatment();
	}

	int index=_node->idxScope;//_node->scope_info_ptr);

	if(index != ZS_UNDEFINED_IDX){
		insertPopScopeInstruction(_node->idxAstNode,index);
	}else{
		print_error_cr("Cannot find scope_info!");
		return false;
	}

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

	if(_node == NULL) {print_error_cr("NULL node");return false;}
	if(_node->node_type != EXPRESSION_NODE){print_error_cr("node is not Expression");return false;}

	int r = gacExpression_Recursive(_node->children[0], _lc,index_instruction);

	return r!= ZS_UNDEFINED_IDX;
}

bool CCompiler::ast2asm_Recursive(short idxAstNode, CScope *_lc){

	PASTNode _node = AST_NODE(idxAstNode);

	if(_node != NULL){
		switch(_node->node_type){
			default:
			case UNKNOWN_NODE:
				print_debug_cr("UNKNOWN_NODE (%i)",_node->node_type);
				return false;
				break;
			case EXPRESSION_NODE: // in fact is EXPRESSION NODE
				print_debug_cr("EXPRESSION_NODE");
				return gacExpression(_node->idxAstNode, _lc);
				break;
			case GROUP_CASES_NODE:
				print_debug_cr("GROUP_CASES_NODE");
				break;
			case KEYWORD_NODE:
				print_debug_cr("KEYWORD_NODE %s",CASTNode::defined_keyword[_node->keyword_info].str);
				return gacKeyword(_node->idxAstNode, _lc);
				break;
			case BODY_NODE:
				print_debug_cr("BODY_NODE");
				return gacBody(_node->idxAstNode, SCOPE_INFO_NODE(_node->idxScope)); // we pass scope node
				break;
			case POST_FOR_NODE:
			case CONDITIONAL_NODE:
				print_debug_cr("%s",_node->node_type == CONDITIONAL_NODE ? "CONDITIONAL_NODE":"POST_FOR_NODE");
				if(_node->children.size() == 1){
					return gacExpression(_node->children[0], _lc);
				}else{
					print_error_cr("Expected nodes for %i",_node->node_type);
				}

				break;
			case PRE_FOR_NODE:
				if(_node->children.size() == 1){
					return ast2asm_Recursive(_node->children[0], _lc);
				}else{
					print_error_cr("Expected nodes for %i",_node->node_type);
				}
				break;
			case FUNCTION_OBJECT_NODE:
				print_debug_cr("FUNCTION_OBJECT");break;
				break;
			case CLASS_VAR_COLLECTION_NODE:print_debug_cr("CLASS_VAR_COLLECTION_NODE");break;
			case CLASS_FUNCTION_COLLECTION_NODE:print_debug_cr("CLASS_FUNCTION_COLLECTION_NODE");break;
			case BASE_CLASS_NODE:print_debug_cr("BASE_CLASS_NODE");break;
			case CALLING_OBJECT_NODE:print_debug_cr("CALLING_OBJECT_NODE");break;
		}
	}else{
		print_error_cr("Node is null!");
	}
	return false;
}

void CCompiler::pushFunction(short idxAstNode,CScriptFunctionObject *sf){

	PASTNode _node =AST_NODE(idxAstNode);

	stk_scriptFunction.push_back(m_currentFunctionInfo=new tInfoFunctionCompile(sf));
	//this->m_currentFunctionInfo = sf;
	//this->m_currentListStatements = &sf->object_info.statment_op;
	this->m_treescope = SCOPE_INFO_NODE(_node->idxScope);
}

void CCompiler::popFunction(){

	//m_currentListStatements=m_functionAsmStatements[m_functionAsmStatements.size()-1];
	//m_currentFunctionInfo = stk_scriptFunction[stk_scriptFunction.size()-1];

	// reserve memory for statment struct...
	vector<tInfoStatementOpCompiler> *vec_comp_statment;
	vec_comp_statment = &m_currentFunctionInfo->stament;

	// get total size op + 1 ends with NULL
	unsigned size=(vec_comp_statment->size()+1)*sizeof(PtrStatment);
	m_currentFunctionInfo->function_info_object->object_info.statment_op = (PtrStatment)malloc(size);
	memset(m_currentFunctionInfo->function_info_object->object_info.statment_op,0,size);
	//m_currentFunctionInfo->function_info_object->object_info.n_statment_op = vec_comp_statment->size();

	for(unsigned i = 0; i < vec_comp_statment->size();i++){ // foreach statment...
		// reserve memory for n ops + 1 NULL end of instruction...
		size = (vec_comp_statment->at(i).asm_op.size()+1)*sizeof(tInfoAsmOp);
		m_currentFunctionInfo->function_info_object->object_info.statment_op[i] = (tInfoAsmOp *)malloc(size);

		memset(m_currentFunctionInfo->function_info_object->object_info.statment_op[i],0,size);
		//m_currentFunctionInfo->function_info_object->object_info.statment_op[i].n_asm_op=vec_comp_statment->at(i).asm_op.size();
		//m_currentFunctionInfo->object_info.statment_op[i] = m_currentListStatements->at(i);
		for(unsigned j = 0; j < vec_comp_statment->at(i).asm_op.size();j++){

			tInfoAsmOpCompiler *asm_op = vec_comp_statment->at(i).asm_op[j];

			m_currentFunctionInfo->function_info_object->object_info.statment_op[i][j].idxAstNode=asm_op->idxAstNode;
			m_currentFunctionInfo->function_info_object->object_info.statment_op[i][j].operator_type=asm_op->operator_type;
			m_currentFunctionInfo->function_info_object->object_info.statment_op[i][j].index_op1=asm_op->index_op1;
			m_currentFunctionInfo->function_info_object->object_info.statment_op[i][j].index_op2=asm_op->index_op2;
			m_currentFunctionInfo->function_info_object->object_info.statment_op[i][j].instruction_properties=asm_op->pre_post_op_type | asm_op->scope_type | asm_op->runtime_prop;
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

bool CCompiler::compile(short idxAstNode, CScriptFunctionObject *sf){

	PASTNode _node =AST_NODE(idxAstNode);

	if(_node == NULL){
		print_error_cr("NULL node!");
		return false;
	}

	if(_node->node_type == NODE_TYPE::BODY_NODE ){
		pushFunction(_node->idxAstNode,sf);
		// reset current pointer ...
		{ // main node ?
			for(unsigned i = 0; i < _node->children.size(); i++){

				if(!ast2asm_Recursive(_node->children[i], m_treescope)){
					return false;
				}
			}
		}
		popFunction();
		return true;
	}
	else{
		print_error_cr("Body node expected");
	}

	return false;
}

CCompiler::~CCompiler(){

}
