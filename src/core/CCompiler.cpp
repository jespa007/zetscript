#include "zg_core.h"

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

CCompiler::tInfoConstantValue * CCompiler::addConstant(const string & const_name, void *obj, VALUE_INSTRUCTION_TYPE type){

	CCompiler::tInfoConstantValue * info_ptr=NULL;

	if(getConstant(const_name) == NULL){
		info_ptr=new tInfoConstantValue;
		info_ptr->ptr=obj;
		info_ptr->type=type;
		(*constant_pool)[const_name]=info_ptr;
	}else{
		print_error_cr("constant %s already exist",const_name.c_str());
	}

	return info_ptr;
}

int CCompiler::addLocalVarSymbol(const string & var_name,tASTNode *ast){


	if(!localVarSymbolExists(var_name,ast)){
		tInfoScopeVar *irv=ast->scope_info_ptr->getInfoRegisteredSymbol(var_name,true);

		if(irv != NULL){

			tInfoRegisteredVariableSymbol info_symbol;

			info_symbol.ast = ast;
			info_symbol.symbol_name = var_name;
			info_symbol.info_var_scope = irv;


			this->m_currentFunctionInfo->object_info.local_symbols.m_registeredVariable.push_back(info_symbol);

			return this->m_currentFunctionInfo->object_info.local_symbols.m_registeredVariable.size()-1;
		}else{
			print_error_cr("variable symbol \"%s\" at line %i not defined!",var_name.c_str(), ast->definedValueline);
		}

	}else{
		print_error_cr("(Internal error) variable symbol \"%s\" already defined!",var_name.c_str());
	}

	return -1;
}

bool CCompiler::localVarSymbolExists(const string & name,tASTNode *ast){


	return getIdxLocalVarSymbol(name,ast, false) != -1;
}



int  CCompiler::getIdxLocalVarSymbol(const string & name,tASTNode *ast, bool print_msg){

	string  var_name = ast->value_symbol;
	tInfoScopeVar *irv=ast->scope_info_ptr->getInfoRegisteredSymbol(var_name,print_msg);

	if(irv != NULL){
		for(unsigned i = 0; i < this->m_currentFunctionInfo->object_info.local_symbols.m_registeredVariable.size(); i++){
			if(this->m_currentFunctionInfo->object_info.local_symbols.m_registeredVariable[i].info_var_scope == irv ){
				return i;
			}
		}
	}

	return -1;
}


int CCompiler::addLocalFunctionSymbol(const string & name,tASTNode *ast){

	string  function_name = name;

	if(!functionSymbolExists(name,ast)){
		tInfoScopeVar *irv=ast->scope_info_ptr->getInfoRegisteredSymbol(function_name,true);

		if(irv != NULL){

			tInfoRegisteredFunctionSymbol info_symbol;

			info_symbol.object_info.symbol_info.ast = irv->ast;
			info_symbol.object_info.symbol_info.info_var_scope = irv;
			info_symbol.object_info.symbol_info.symbol_name = name;


			this->m_currentFunctionInfo->object_info.local_symbols.m_registeredFunction.push_back(info_symbol);

			return this->m_currentFunctionInfo->object_info.local_symbols.m_registeredFunction.size()-1;//&this->m_currentFunctionInfo->local_symbols.m_registeredFunction[this->m_currentFunctionInfo->local_symbols.m_registeredFunction.size()-1];
		}

	}else{
		print_error_cr("function symbol \"%s\" defined at line %i not defined!",function_name.c_str(), ast->definedValueline);
	}

	return -1;
}

bool CCompiler::functionSymbolExists(const string & name, tASTNode *ast){

	SCOPE_TYPE scope_type;
	return getIdxFunctionSymbol(name,ast,scope_type,false) != -1;
}


int  CCompiler::getIdxFunctionSymbol(const string & name,tASTNode *ast, SCOPE_TYPE & scope_type, bool print_msg){
	tInfoScopeVar *irv=ast->scope_info_ptr->getInfoRegisteredSymbol(name);
	scope_type = SCOPE_TYPE::LOCAL_SCOPE;
	if(irv != NULL){



		if((irv->ast != NULL) && (irv->ast->scope_info_ptr == ast->scope_info_ptr)){
			for(unsigned i = 0; i < this->m_currentFunctionInfo->object_info.local_symbols.m_registeredFunction.size(); i++){
				if(this->m_currentFunctionInfo->object_info.local_symbols.m_registeredFunction[i].object_info.symbol_info.symbol_name == name ){
					return i;
				}
			}
		}
		else{ //global

			scope_type = SCOPE_TYPE::GLOBAL_SCOPE;

			for(unsigned i = 0; i < CZG_ScriptCore::getInstance()->getMainStructInfo()->object_info.local_symbols.m_registeredFunction.size(); i++){
				if(CZG_ScriptCore::getInstance()->getMainStructInfo()->object_info.local_symbols.m_registeredFunction[i].object_info.symbol_info.symbol_name == name ){
					return i;
				}
			}



		}
	}

	return -1;
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
		delete m_compiler;
		m_compiler=NULL;
		constant_pool->clear();
		delete constant_pool;
		constant_pool = NULL;
	}
}







//------------------------------------------------------------------------------------------------------------------
//
// CONSTRUCTOR
//

CCompiler::CCompiler(){
	m_currentListStatements = NULL;
	m_currentFunctionInfo = NULL;
	m_treescope = NULL;

	//		VAR  			|	STR   | ID | NUM OP
	//----------------------+---------+----+-------
	def_operator[NOP]         ={"NOP" ,NOP ,0      };
	def_operator[MOV]         ={"MOV" ,MOV ,2}; // mov expression to var
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
	//def_operator[PRE_INC]     ={"PRE_INC",PRE_INC,1};// ++
	//def_operator[POST_INC]    ={"POST_INC",POST_INC,1}; // ++
	//def_operator[PRE_DEC]     ={"PRE_DEC",PRE_DEC,1}; // ++
	//def_operator[POST_DEC]    ={"POST_DEC",POST_DEC,1}; // ++
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
	def_operator[CLR]={"CLR",CLR,0};
	def_operator[VGET]={"VGET",VGET,1}; // vector access after each index is processed...

	def_operator[VEC]={"VEC",VEC,1}; // Vector object (CREATE)

	def_operator[VPUSH]={"VPUSH",VPUSH,1}; // Value push for vector
	def_operator[VPOP]={"VPOP",VPOP,1}; // Value pop for vector
	def_operator[RET]={"RET",RET,1}; // Value pop for vector

	def_operator[NEW]={"NEW",NEW,1}; // New object (CREATE)
	def_operator[OBJECT_ACCESS]={"OBJECT_ACCESS",OBJECT_ACCESS,2}; // New object (CREATE)
	def_operator[SAVE_I]={"SAVE_I",SAVE_I,0}; // New object (CREATE)
	def_operator[LOAD_I]={"LOAD_I",LOAD_I,0}; // New object (CREATE)


}

//------------------------------------------------------------------------------------------------------------------
//
// COMPILE COMPILER MANAGEMENT
//
int CCompiler::getCurrentInstructionIndex(){
	tInfoStatementOp *ptr_current_statement_op = &(*m_currentListStatements)[m_currentListStatements->size()-1];
	return ptr_current_statement_op->asm_op.size()-1;
}

int CCompiler::getCurrentStatmentIndex(){
	return (int)(m_currentListStatements->size()-1);
}

tInfoStatementOp * CCompiler::newStatment(){
	tInfoStatementOp st;

	m_currentListStatements->push_back(st);

	return  &(*m_currentListStatements)[m_currentListStatements->size()-1];
}

bool CCompiler::isFunctionNode(PASTNode node){

	if(node==NULL){
		print_error_cr("Node is NULL!");
		return false;
	}

	return node->node_type == FUNCTION_OBJECT_NODE || (node->node_type==FUNCTION_REF_NODE);
}

int CCompiler::getIdxArgument(const string & var){
	// search if symbol belongs to arg vector...
	for(unsigned i = 0; i < this->m_currentFunctionInfo->m_arg.size(); i++){

		if(this->m_currentFunctionInfo->m_arg[i] == var){
			return i;
		}
	}

	return -1;

}

bool checkAccessObjectMember(PASTNode _node){

	bool node_access=false;
	if(_node == NULL) return false;

	PASTNode check_node = _node->parent;
	PASTNode child_compare=_node;


	if(check_node != NULL){
		if(check_node->node_type==NODE_TYPE::CALLING_OBJECT_NODE) {// function / array access.
			child_compare=check_node;
			check_node=check_node->parent;

		}

		if(check_node != NULL){
			node_access =check_node->operator_info != NULL &&
						check_node->operator_info->id==PUNCTUATOR_TYPE::FIELD_PUNCTUATOR; // trivial case...
		}

		if(node_access){

			// check first ...
			if(check_node->parent != NULL){

				if(!checkAccessObjectMember(check_node->parent)){

					// if c.b is trivial but c.b.a must check that parent it has another punctuator.
					node_access = (check_node->children[0] != child_compare) || (
									check_node->parent->operator_info != NULL &&
									check_node->parent->operator_info->id==PUNCTUATOR_TYPE::FIELD_PUNCTUATOR); // parent not access left is first...
				}

			}

		}

	}


	return node_access;
}

bool CCompiler::insertLoadValueInstruction(PASTNode _node, CScopeInfo * _lc){


	string v = _node->value_symbol;

	// ignore node this ...
	if(_node->value_symbol == "this"){
	  // ||(_node->value_symbol == "super")
		//	){
		print_error_cr("\"%s\" cannot be processed here!",_node->value_symbol.c_str());
		return false;
	}


	//this_instruction=false;
	//int m_var_at_line = _node->definedValueline;
	ASM_PRE_POST_OPERATORS pre_post_operator_type =ASM_PRE_POST_OPERATORS::UNKNOW_PRE_POST_OPERATOR;

	tInfoStatementOp *ptr_current_statement_op = &(*m_currentListStatements)[m_currentListStatements->size()-1];
	void *const_obj;
	void *obj;
	CCompiler::tInfoConstantValue *get_obj;
	VALUE_INSTRUCTION_TYPE type=INS_TYPE_VAR;
	LOAD_TYPE load_type=LOAD_TYPE_NOT_DEFINED;
	SCOPE_TYPE scope_type=SCOPE_TYPE::UNKNOWN_SCOPE;
	bool is_constant = true;


	if(_node->pre_post_operator_info != NULL){

		pre_post_operator_type=preoperator2instruction(_node->pre_post_operator_info->id);
	}
	// try parse value...
	if(v=="null"){
		type=INS_TYPE_NULL;
		load_type=LOAD_TYPE_NULL;
		obj=CScriptVariable::NullSymbol;
		print_com_cr("%s detected as null\n",v.c_str());
	}else if(v=="undefined"){
			type=INS_TYPE_UNDEFINED;
			load_type=LOAD_TYPE_UNDEFINED;
			obj=CScriptVariable::UndefinedSymbol;
			print_com_cr("%s detected as undefined\n",v.c_str());

	}else if((const_obj=CInteger::Parse(v))!=NULL){
		int value = *((int *)const_obj);
		delete (int *)const_obj;

		type=INS_TYPE_INTEGER;
		load_type=LOAD_TYPE_CONSTANT;
		print_com_cr("%s detected as int\n",v.c_str());
		if((get_obj = getConstant(v))!=NULL){
			obj = get_obj;
		}else{
			obj=addConstant(v,new int(value),type);
		}
	}
	else if((const_obj=CNumber::Parse(v))!=NULL){
		float value = *((float *)const_obj);
		delete (float *)const_obj;

		type=INS_TYPE_NUMBER;
		load_type=LOAD_TYPE_CONSTANT;
		print_com_cr("%s detected as float\n",v.c_str());

		if((get_obj = getConstant(v))!=NULL){
			obj = get_obj;
		}else{
			obj=addConstant(v,new float(value),type);
		}
	}
	else if(v[0]=='\"' && v[v.size()-1]=='\"'){

		//string value = ((CString *)obj)->m_value;
		//delete obj;

		type=INS_TYPE_STRING;
		load_type=LOAD_TYPE_CONSTANT;
		print_com_cr("%s detected as string\n",v.c_str());

		if((get_obj = getConstant(v))!=NULL){
			obj = get_obj;
		}else{
			string s=v.substr(1,v.size()-2);
			//CString *os=new CString();
			//os->m_value = s;
			//obj = os;
			obj=addConstant(v,new string(s),type);
		}

	}
	else if((const_obj=CBoolean::Parse(v))!=NULL){

		bool value = *((bool *)const_obj);
		delete (bool *)const_obj;

		type=INS_TYPE_BOOLEAN;
		load_type=LOAD_TYPE_CONSTANT;
		print_com_cr("%s detected as boolean\n",v.c_str());

		if((get_obj = getConstant(v))!=NULL){
			obj = get_obj;
		}else{
			obj=addConstant(v,new bool(value),type);
		}
	}else{

		is_constant = false;

		int idx_local_var=-1;

		/*if(_node->value_symbol == "gg"){
			int kkkk=0;
		}*/


		if(checkAccessObjectMember(_node)){
			scope_type = SCOPE_TYPE::ACCESS_SCOPE;


			if((_node->parent != NULL &&_node->parent->children[0]->value_symbol == "this") //|| // single access ?
			   //(_node->value_symbol == "super")//_node->parent->node_type == NODE_TYPE::CALLING_OBJECT_NODE && _node->parent->parent != NULL && _node->parent->parent->children[0]->value_symbol == "this"  ) // calling object needs +1 step more to check whether is valid this scope...
			){

				scope_type=SCOPE_TYPE::THIS_SCOPE;
				//scope_type=SCOPE_TYPE::THIS_SCOPE;
				//string class_name = m_currentFunctionInfo->object_info.symbol_info.symbol_name;

				//if(!CScriptClassFactory::getInstance()->getIdxRegisteredVariableSymbol(class_name, _node->value_symbol)){
				//	return false;
				//}
			}

		}
		else if(
			(_node->value_symbol == "super")
	          // (_node->parent->node_type == NODE_TYPE::CALLING_OBJECT_NODE && _node->parent->parent != NULL && _node->parent->parent->children[0]->value_symbol == "super"  ) // calling object needs +1 step more to check whether is valid this scope...
			){
						scope_type=SCOPE_TYPE::SUPER_SCOPE;
						//load_type=LOAD_TYPE::LOAD_TYPE_FUNCTION;
					}else{


			//if(isFunctionNode(_node))
			// try function ....


			// Local Functions are already inserted during "gacFunction" process, so we don't have
			// to register again...


			//if(idx_local_var==-1)
			{ // if not function then is var or arg node ?


				// first we find the list of argments
				if((idx_local_var = getIdxArgument(_node->value_symbol))!=-1){
					load_type=LOAD_TYPE_ARGUMENT;
				}
				else{ // ... if not argument finally, we deduce that the value is a local symbol...

					//PASTNode aa = _node->parent;

					/*if(node_access){

						print_warning_cr("load \"%s\" at run-time",_node->value_symbol.c_str());
						checkAccessObjectMember(_node);
					}
					else{*/

						/*if((idx_local_var=getIdxLocalVarSymbol(_node->value_symbol,_node)) != -1){ //if not exist add symbol ...
							load_type=LOAD_TYPE_VARIABLE;
							//print_error_cr("local variable %s not found",_node->value_symbol.c_str());
							//return false;
							if((idx_local_var = addLocalVarSymbol(_node->value_symbol, _node)) != -1){
								load_type=LOAD_TYPE_VARIABLE;


							}else{
								return false;
							}
						}*/
					}

				}
		}



		obj = (CScriptVariable *)idx_local_var;
	}

	if((pre_post_operator_type !=ASM_PRE_POST_OPERATORS::UNKNOW_PRE_POST_OPERATOR) &&
		is_constant){
		print_error_cr("line %i: operation \"%s\" not allowed for constants ",_node->definedValueline,_node->pre_post_operator_info->str);
		return false;

	}

	tInfoAsmOp *asm_op = new tInfoAsmOp();

	asm_op->variable_type=type;
	asm_op->index_op1=load_type;
	asm_op->index_op2=(int)obj;
	asm_op->scope_type = scope_type;
	asm_op->ast_node=_node;
	asm_op->pre_post_operator_type=pre_post_operator_type;

	asm_op->operator_type=ASM_OPERATOR::LOAD;
	ptr_current_statement_op->asm_op.push_back(asm_op);

	return true;
}

bool CCompiler::insertMovVarInstruction(PASTNode _node,int left_index, int right_index){

	tInfoStatementOp *ptr_current_statement_op = &(*m_currentListStatements)[m_currentListStatements->size()-1];
	tInfoAsmOp * left_asm_op = ptr_current_statement_op->asm_op[left_index];

	// check whether left operant is object...
	if(left_asm_op->variable_type != INS_TYPE_VAR){
		int line = -1;

		if(left_asm_op->ast_node!=NULL)
			line=left_asm_op->ast_node->definedValueline;
		print_error_cr("line %i. left operand must be l-value for '=' operator",line);
		return false;
	}

	tInfoAsmOp *asm_op = new tInfoAsmOp();
	asm_op->index_op1 = left_index;//&((*m_currentListStatements)[dest_statment]);
	asm_op->index_op2 =  right_index;
	asm_op->ast_node = _node;
	//asm_op->symbol_name="";
	asm_op->operator_type=ASM_OPERATOR::MOV;

	ptr_current_statement_op->asm_op.push_back(asm_op);

	return true;
}

tInfoAsmOp * CCompiler::insert_JMP_Instruction(int jmp_statement, int instruction_index){

	tInfoStatementOp *ptr_current_statement_op = &(*m_currentListStatements)[m_currentListStatements->size()-1];
	tInfoAsmOp *asm_op = new tInfoAsmOp();
	asm_op->index_op1 = jmp_statement;//&((*m_currentListStatements)[dest_statment]);
	asm_op->index_op2 = instruction_index;
	asm_op->operator_type=ASM_OPERATOR::JMP;
	ptr_current_statement_op->asm_op.push_back(asm_op);
	//printf("[%02i:%02i]\tJMP\t[??]\n",m_currentListStatements->size(),ptr_current_statement_op->asm_op.size());

	return asm_op;
}

tInfoAsmOp * CCompiler::insert_JNT_Instruction(int jmp_statement, int instruction_index){

	tInfoStatementOp *ptr_current_statement_op = &(*m_currentListStatements)[m_currentListStatements->size()-1];
	tInfoAsmOp *asm_op = new tInfoAsmOp();
	asm_op->index_op1 = jmp_statement;//&((*m_currentListStatements)[dest_statment]);
	asm_op->index_op2 = instruction_index;
	asm_op->operator_type=ASM_OPERATOR::JNT;

	//printf("[%02i:%02i]\tJNT\t[%02i:%02i],[??]\n",m_currentListStatements->size(),ptr_current_statement_op->asm_op.size(),m_currentListStatements->size(),ptr_current_statement_op->asm_op.size()-1);
	ptr_current_statement_op->asm_op.push_back(asm_op);

	return asm_op;
}

tInfoAsmOp * CCompiler::insert_JT_Instruction(int jmp_statement, int instruction_index){

	tInfoStatementOp *ptr_current_statement_op = &(*m_currentListStatements)[m_currentListStatements->size()-1];
	tInfoAsmOp *asm_op = new tInfoAsmOp();
	asm_op->index_op1 = jmp_statement;//&((*m_currentListStatements)[dest_statment]);
	asm_op->operator_type=ASM_OPERATOR::JT;
	asm_op->index_op2 = instruction_index;
	//printf("[%02i:%02i]\tJT \t[%02i:%02i],[??]\n",m_currentListStatements->size(),ptr_current_statement_op->asm_op.size(),m_currentListStatements->size(),ptr_current_statement_op->asm_op.size()-1);
	ptr_current_statement_op->asm_op.push_back(asm_op);
	return asm_op;
}

void CCompiler::insert_NOP_Instruction(){
	tInfoStatementOp *ptr_current_statement_op = &(*m_currentListStatements)[m_currentListStatements->size()-1];
	tInfoAsmOp *asm_op = new tInfoAsmOp();
	asm_op->index_op1 = 0;//&((*m_currentListStatements)[dest_statment]);
	asm_op->operator_type=ASM_OPERATOR::NOP;
	//printf("[%02i:%02i]\tNOP\n",m_currentListStatements->size(),ptr_current_statement_op->asm_op.size());
	ptr_current_statement_op->asm_op.push_back(asm_op);

}

void CCompiler::insert_CreateArrayObject_Instruction(PASTNode _node){
	tInfoStatementOp *ptr_current_statement_op = &(*m_currentListStatements)[m_currentListStatements->size()-1];
	tInfoAsmOp *asm_op = new tInfoAsmOp();

	asm_op->operator_type=ASM_OPERATOR::VEC;
	asm_op->variable_type =INS_TYPE_VAR;
	asm_op->ast_node = _node;
	//printf("[%02i:%02i]\tJT \t[%02i:%02i],[??]\n",m_currentListStatements->size(),ptr_current_statement_op->asm_op.size(),m_currentListStatements->size(),ptr_current_statement_op->asm_op.size()-1);
	ptr_current_statement_op->asm_op.push_back(asm_op);

}

void CCompiler::insert_ArrayAccess_Instruction(int vec_object, int index_instrucction, PASTNode _ast){
	tInfoStatementOp *ptr_current_statement_op = &(*m_currentListStatements)[m_currentListStatements->size()-1];
	tInfoAsmOp *asm_op = new tInfoAsmOp();
	asm_op->index_op1 = vec_object;//&((*m_currentListStatements)[dest_statment]);
	asm_op->index_op2 = index_instrucction;//&((*m_currentListStatements)[dest_statment]);
	asm_op->operator_type=ASM_OPERATOR::VGET;
	asm_op->ast_node = _ast;
	asm_op->variable_type = INS_TYPE_VAR;
	//printf("[%02i:%02i]\tJT \t[%02i:%02i],[??]\n",m_currentListStatements->size(),ptr_current_statement_op->asm_op.size(),m_currentListStatements->size(),ptr_current_statement_op->asm_op.size()-1);
	ptr_current_statement_op->asm_op.push_back(asm_op);

}


void CCompiler::insert_ClearArgumentStack_Instruction(PASTNode _node){
	tInfoStatementOp *ptr_current_statement_op = &(*m_currentListStatements)[m_currentListStatements->size()-1];
	tInfoAsmOp *asm_op = new tInfoAsmOp();
	asm_op->ast_node = _node;
	asm_op->operator_type=ASM_OPERATOR::CLR;
	//printf("[%02i:%02i]\tNOP\n",m_currentListStatements->size(),ptr_current_statement_op->asm_op.size());
	ptr_current_statement_op->asm_op.push_back(asm_op);
}


void CCompiler::insert_PushArgument_Instruction(PASTNode _node){
	tInfoStatementOp *ptr_current_statement_op = &(*m_currentListStatements)[m_currentListStatements->size()-1];
	tInfoAsmOp *asm_op = new tInfoAsmOp();
	asm_op->ast_node = _node;
	asm_op->index_op1 = getCurrentInstructionIndex();//&((*m_currentListStatements)[dest_statment]);
	asm_op->operator_type=ASM_OPERATOR::PUSH;
	//printf("[%02i:%02i]\tJT \t[%02i:%02i],[??]\n",m_currentListStatements->size(),ptr_current_statement_op->asm_op.size(),m_currentListStatements->size(),ptr_current_statement_op->asm_op.size()-1);
	ptr_current_statement_op->asm_op.push_back(asm_op);

}

void CCompiler::insert_ClearArgumentStack_And_PushFirstArgument_Instructions(PASTNode _node){
	tInfoStatementOp *ptr_current_statement_op = &(*m_currentListStatements)[m_currentListStatements->size()-1];
	tInfoAsmOp *asm_op = new tInfoAsmOp();
	asm_op->operator_type=ASM_OPERATOR::CLR;
	asm_op->ast_node = _node;
	//printf("[%02i:%02i]\tNOP\n",m_currentListStatements->size(),ptr_current_statement_op->asm_op.size());
	ptr_current_statement_op->asm_op.push_back(asm_op);

	// push one less instruction to get the value
	asm_op = new tInfoAsmOp();
	asm_op->index_op1 = getCurrentInstructionIndex()-1;//&((*m_currentListStatements)[dest_statment]);
	asm_op->operator_type=ASM_OPERATOR::PUSH;
	asm_op->ast_node=_node;
	//printf("[%02i:%02i]\tJT \t[%02i:%02i],[??]\n",m_currentListStatements->size(),ptr_current_statement_op->asm_op.size(),m_currentListStatements->size(),ptr_current_statement_op->asm_op.size()-1);
	ptr_current_statement_op->asm_op.push_back(asm_op);

}

void CCompiler::insert_CallFunction_Instruction(PASTNode _node,int  index_call,int  index_object){
	tInfoStatementOp *ptr_current_statement_op = &(*m_currentListStatements)[m_currentListStatements->size()-1];
	tInfoAsmOp *asm_op = new tInfoAsmOp();

	asm_op->index_op1 = index_call;//&((*m_currentListStatements)[dest_statment]);
	asm_op->index_op2 = index_object;//&((*m_currentListStatements)[dest_statment]);

	asm_op->ast_node = _node;
	asm_op->operator_type=ASM_OPERATOR::CALL;

	//printf("[%02i:%02i]\tJT \t[%02i:%02i],[??]\n",m_currentListStatements->size(),ptr_current_statement_op->asm_op.size(),m_currentListStatements->size(),ptr_current_statement_op->asm_op.size()-1);
	ptr_current_statement_op->asm_op.push_back(asm_op);
}

void CCompiler::insertRet(PASTNode _node,int index){
	tInfoStatementOp *ptr_current_statement_op = &(*m_currentListStatements)[m_currentListStatements->size()-1];
	tInfoAsmOp *asm_op = new tInfoAsmOp();
	asm_op->index_op1 = index;//&((*m_currentListStatements)[dest_statment]);
	//asm_op->symbol_name="";
	asm_op->operator_type=ASM_OPERATOR::RET;
	asm_op->ast_node = _node;

	ptr_current_statement_op->asm_op.push_back(asm_op);
}


void CCompiler::insert_ArrayObject_PushValueInstruction(PASTNode _node,int ref_vec_object_index,int index_instruciont_to_push){
	tInfoStatementOp *ptr_current_statement_op = &(*m_currentListStatements)[m_currentListStatements->size()-1];
	tInfoAsmOp *asm_op = new tInfoAsmOp();
	asm_op->index_op1=ref_vec_object_index;
	asm_op->ast_node = _node;
	asm_op->index_op2=index_instruciont_to_push;
	if(index_instruciont_to_push == -1){
		asm_op->index_op2=CCompiler::getCurrentInstructionIndex();
	}
	asm_op->operator_type=ASM_OPERATOR::VPUSH;
	ptr_current_statement_op->asm_op.push_back(asm_op);
}

//--------------------------------------------------------------------------------------------------------------------------------------------------------------------

bool CCompiler::insert_NewObject_Instruction(PASTNode _node, const string & class_name){
	tInfoStatementOp *ptr_current_statement_op = &(*m_currentListStatements)[m_currentListStatements->size()-1];
	tInfoAsmOp *asm_op = new tInfoAsmOp();
	if((asm_op->index_op1 = CScriptClassFactory::getInstance()->getIdxRegisteredClass(class_name))==-1){//&((*m_currentListStatements)[dest_statment]);
		print_error_cr("class \"%s\" is not registered", class_name.c_str());
		return false;
	}
	//asm_op->symbol_name="";
	asm_op->operator_type=ASM_OPERATOR::NEW;
	asm_op->ast_node = _node;

	ptr_current_statement_op->asm_op.push_back(asm_op);

	return true;
}

bool CCompiler::insertObjectMemberAccessFrom(PASTNode _node, int ref_node_index){
	tInfoStatementOp *ptr_current_statement_op = &(*m_currentListStatements)[m_currentListStatements->size()-1];
	tInfoAsmOp *asm_op = new tInfoAsmOp();
	asm_op->index_op1 = ref_node_index;
	asm_op->index_op2 = -1; // index from object cached node ?
	//asm_op->symbol_name="";
	asm_op->operator_type=ASM_OPERATOR::OBJECT_ACCESS;
	asm_op->ast_node = _node;

	ptr_current_statement_op->asm_op.push_back(asm_op);

	return true;
}


//--------------------------------------------------------------------------------------------------------------------------------------------------------------------
ASM_PRE_POST_OPERATORS CCompiler::preoperator2instruction(PUNCTUATOR_TYPE op){

	switch(op){
	default:
		return ASM_PRE_POST_OPERATORS::UNKNOW_PRE_POST_OPERATOR;
	case PUNCTUATOR_TYPE::PRE_INC_PUNCTUATOR:
		return PRE_INC;
	case PUNCTUATOR_TYPE::POST_INC_PUNCTUATOR:
		return POST_INC;
	case PUNCTUATOR_TYPE::PRE_DEC_PUNCTUATOR:
		return PRE_DEC;
	case PUNCTUATOR_TYPE::POST_DEC_PUNCTUATOR:
		return POST_DEC;
	}

	return ASM_PRE_POST_OPERATORS::UNKNOW_PRE_POST_OPERATOR;
}

ASM_OPERATOR CCompiler::puntuator2instruction(tInfoPunctuator * op){

	switch(op->id){
	default:
		print_error_cr("%s Not implemented", op->str);
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
		return ASM_OPERATOR::MOV;
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

	return INVALID_OP;
}

bool CCompiler::insertOperatorInstruction(tInfoPunctuator * op, PASTNode _node, string & error_str, int op_index_left, int op_index_right){
	tInfoStatementOp *ptr_current_statement_op = &(*m_currentListStatements)[m_currentListStatements->size()-1];
	tInfoAsmOp * left_asm_op = ptr_current_statement_op->asm_op[op_index_left];

	if(op->id == ASSIGN_PUNCTUATOR && left_asm_op->variable_type != INS_TYPE_VAR){

			error_str = "left operand must be l-value for '=' operator";
			return false;
	}

	if(op->id == PUNCTUATOR_TYPE::FIELD_PUNCTUATOR){ // trivial access...
		return true;
	}


	ASM_OPERATOR asm_op;
	if((asm_op= puntuator2instruction(op))!=INVALID_OP){


		tInfoAsmOp *iao = new tInfoAsmOp();
		//asm_op->type_op=OPERATOR;
		iao->operator_type = asm_op;
		iao->index_op1 = op_index_left;
		iao->index_op2 = op_index_right;

		iao->ast_node=_node;


		ptr_current_statement_op->asm_op.push_back(iao);

		return true;
	}
	return false;
}



tInfoAsmOp * CCompiler::insert_Save_CurrentInstruction(){

	tInfoStatementOp *ptr_current_statement_op = &(*m_currentListStatements)[m_currentListStatements->size()-1];
	tInfoAsmOp *asm_op = new tInfoAsmOp();
	asm_op->operator_type=ASM_OPERATOR::SAVE_I;
	ptr_current_statement_op->asm_op.push_back(asm_op);
	//printf("[%02i:%02i]\tJMP\t[??]\n",m_currentListStatements->size(),ptr_current_statement_op->asm_op.size());

	return asm_op;
}

tInfoAsmOp * CCompiler::insert_Load_CurrentInstruction(){

	tInfoStatementOp *ptr_current_statement_op = &(*m_currentListStatements)[m_currentListStatements->size()-1];
	tInfoAsmOp *asm_op = new tInfoAsmOp();
	asm_op->operator_type=ASM_OPERATOR::LOAD_I;
	ptr_current_statement_op->asm_op.push_back(asm_op);
	//printf("[%02i:%02i]\tJMP\t[??]\n",m_currentListStatements->size(),ptr_current_statement_op->asm_op.size());

	return asm_op;
}

//------------------------------------------------------------------------------------------------------------------
//
// COMPILE EXPRESSIONS AND GENERATE ITS ASM
//
int CCompiler::gacExpression_ArrayAccess(PASTNode _node, CScopeInfo *_lc)
{
	if(_node == NULL) {print_error_cr("NULL node");return -1;}
	if(_node->node_type != CALLING_OBJECT_NODE ){print_error_cr("node is not CALLING_OBJECT_NODE type or null");return -1;}
	if(_node->children.size()!=2) {print_error_cr("Array access should have 2 children");return -1;}
	if(_node->children[0]->node_type != ARRAY_REF_NODE && _node->children[0]->node_type != ARRAY_OBJECT_NODE ){print_error_cr("Node is not ARRAY_OBJECT type"); return -1;}
	if(_node->children[1]->node_type != ARRAY_ACCESS_NODE || _node->children[1]->children.size() == 0){print_error_cr("Array has no index nodes "); return -1;}

	int vec=0;

	if(_node->children[0]->node_type == ARRAY_OBJECT_NODE){ // must first create the object ...
		if((vec=gacExpression_ArrayObject(_node->children[0],_lc))==-1){
			return -1;
		}
	}else{


		if(!insertLoadValueInstruction(_node->children[0],  _lc)){
			return -1;
		}


		vec = CCompiler::getCurrentInstructionIndex();
	}


	PASTNode array_acces = _node->children[1];

	// get all indexes ...
	for(unsigned k = 0; k < array_acces->children.size(); k++){
		if(array_acces->children [k]->node_type == ARRAY_INDEX_NODE){
			if(array_acces->children [k]->children.size() == 1){
				// check whether is expression node...
				if(!(gacExpression(array_acces->children [k]->children[0], _lc,getCurrentInstructionIndex()+1))){
					return -1;
				}

				// insert vector access instruction ...
				insert_ArrayAccess_Instruction(vec,CCompiler::getCurrentInstructionIndex(),array_acces->children [k]);
				vec = getCurrentInstructionIndex();

			}else{
				print_error_cr("Expected 1 children");
				return -1;
			}

		}else{
			print_error_cr("Node not ARRAY_INDEX_NODE");
			return -1;
		}
	}

	if(_node->pre_post_operator_info != NULL){ // there's pre/post increment...


		// get post/inc
		tInfoStatementOp *ptr_current_statement_op = &(*m_currentListStatements)[m_currentListStatements->size()-1];
		tInfoAsmOp *asm_op = ptr_current_statement_op->asm_op[ptr_current_statement_op->asm_op.size()-1];
		asm_op->pre_post_operator_type=preoperator2instruction(_node->pre_post_operator_info->id);

	}

	// return last instruction where was modified
	return getCurrentInstructionIndex();
}

int CCompiler::gacExpression_ArrayObject_Recursive(PASTNode _node, CScopeInfo *_lc){

	return 0;
}

int CCompiler::gacExpression_ArrayObject(PASTNode _node, CScopeInfo *_lc)
{
	if(_node == NULL) {print_error_cr("NULL node");return -1;}
	if(_node->node_type != ARRAY_OBJECT_NODE ){print_error_cr("node is not ARRAY_OBJECT_NODE type or null");return -1;}

	int r=0;

	// 1. create object ...
	insert_CreateArrayObject_Instruction(_node);
	int index_created_vec = CCompiler::getCurrentInstructionIndex();

	//bool children_is_vector_object;

	// 2. evaluate expressions if any
	for(unsigned j=0; j < _node->children.size(); j++){

		int ini_instruction = -1;

		if(_node->children[j]->node_type == EXPRESSION_NODE){
			/*children_is_vector_object = _node->children[j]->children[0]->node_type == ARRAY_OBJECT_NODE;*/
			ini_instruction = getCurrentInstructionIndex()+1;
		}



		// check whether is expression node...
		if((r=gacExpression(_node->children[j], _lc,ini_instruction)) == -1){
			return -1;
		}

		/*if(index_created_vec==0){
			int yy=0;
			yy++;

		}*/

		insert_ArrayObject_PushValueInstruction(_node,index_created_vec,ini_instruction);
	}
	return index_created_vec;//CCompiler::getCurrentInstructionIndex();
}

int CCompiler::gacExpression_FunctionObject(PASTNode _node, CScopeInfo *_lc)
{
	if(_node == NULL) {print_error_cr("NULL node");return -1;}
	if(_node->node_type != FUNCTION_OBJECT_NODE ){print_error_cr("node is not FUNCTION_OBJECT_NODE type or null");return -1;}
	if(_node->children.size()!=2) {print_error_cr("Array access should have 2 children");return -1;}

	int idx=0;

	// 1. insert load reference created object ...
	if(functionSymbolExists(_node->value_symbol, _node)){
			print_error_cr("Function \"%s\" already defined !",_node->value_symbol.c_str());
			return false;
	}

	if((idx=addLocalFunctionSymbol(_node->value_symbol,_node)) == -1){
		return false;
	}

	// compiles anonymous function ...
	if(!gacFunction(_node,_lc, &this->m_currentFunctionInfo->object_info.local_symbols.m_registeredFunction[idx])){
		return -1;
	}

	return insertLoadValueInstruction(_node, _lc);

}

int CCompiler::gacExpression_FunctionAccess(PASTNode _node, CScopeInfo *_lc)
{
	if(_node == NULL) {print_error_cr("NULL node");return -1;}
	if(_node->node_type != CALLING_OBJECT_NODE ){print_error_cr("node is not CALLING_OBJECT_NODE type or null");return -1;}
	if(_node->children.size()!=2) {print_error_cr("Array access should have 2 children");return -1;}
	if(_node->children[0]->node_type != FUNCTION_REF_NODE && _node->children[0]->node_type != FUNCTION_OBJECT_NODE){print_error_cr("Node is not FUNCTION_OBJECT_NODE type"); return -1;}
	if(_node->children[1]->node_type != ARGS_PASS_NODE){print_error_cr("Function has no index nodes "); return -1;}
	//int r=0;

	// load function ...
	if(!insertLoadValueInstruction(_node->children[0],_lc)) {
		return -1;
	}
	int call_index = getCurrentInstructionIndex();

	// 1. insert push to pass values to all args ...
	PASTNode function_args = _node->children[1];

	if(function_args->children.size() > 0){
	for(unsigned k = 0; k < function_args->children.size(); k++){

		// check whether is expression node...
		if(!gacExpression(function_args->children[k], _lc,getCurrentInstructionIndex()+1)){
			return -1;
		}

		if(k==0){

			// insert clear push arguments stack
			insert_ClearArgumentStack_And_PushFirstArgument_Instructions(_node);

			// insert vector access instruction ...
			//insert_PushArgument_Instruction_Less1();

		}else{
			// insert vector access instruction ...
			insert_PushArgument_Instruction(_node);
		}


	}
	}else{
		// clear the stack only ..
		insert_ClearArgumentStack_Instruction(_node);
	}

	// 2. insert call instruction itself.
	insert_CallFunction_Instruction(_node,call_index);

	return CCompiler::getCurrentInstructionIndex();
}

bool CCompiler::isThisScope(PASTNode _node){
	if(_node == NULL){
		return false;
	}

	return ((_node->node_type == PUNCTUATOR_NODE) &&
			   //(_node->parent != NULL && _node->parent->node_type != PUNCTUATOR_NODE) &&
			   (_node->children.size()==2 && _node->children[0]->value_symbol=="this")
			   );

}

bool CCompiler::isSuperScope(PASTNode _node){
	if(_node == NULL){
		return false;
	}

	return ((_node->node_type == PUNCTUATOR_NODE) &&
			   //(_node->parent != NULL && _node->parent->node_type != PUNCTUATOR_NODE) &&
			   (_node->children.size()==2 && _node->children[0]->value_symbol=="super")
			   );

}

int CCompiler::gacExpression_Recursive(PASTNode _node, CScopeInfo *_lc, int & index_instruction){

	//CScopeInfo * _lc = m_currentFunctionInfo->getScope();
	int r=index_instruction;
	//bool this_object = false;
	bool inline_if_else=false;
	string error_str;
	bool access_node = false;

	if(_node==NULL){
		return -1;
	}



	if( isThisScope(_node)
	 || isSuperScope(_node)){ // only take care left children...
		_node = _node->children[1];
	}


	bool special_node =	 _node->node_type == ARRAY_OBJECT_NODE || // =[]
						_node->node_type == FUNCTION_OBJECT_NODE || // =function()
						_node->node_type == CALLING_OBJECT_NODE ||  // pool[] or pool()
						_node->node_type == NEW_OBJECT_NODE;  // new




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
				eval_node_sp = _node->children[0];

				function_access = eval_node_sp->node_type == FUNCTION_OBJECT_NODE || eval_node_sp->node_type == FUNCTION_REF_NODE;
				array_access = eval_node_sp->node_type == ARRAY_OBJECT_NODE || eval_node_sp->node_type == ARRAY_REF_NODE;


			}else {
				print_error_cr("Calling object should have at least 1 children");
				return -1;
			}
		}

		if(special_node ){

				if(array_access){
					if((r=gacExpression_ArrayAccess(_node, _lc)) == -1){
												return -1;
											}
				}else if(function_access){
					if((r=gacExpression_FunctionAccess(_node, _lc)) == -1){
						return -1;
					}
				}
				else{

					switch(eval_node_sp->node_type){
					case ARRAY_OBJECT_NODE: // should have 1 children
						if((r=gacExpression_ArrayObject(_node, _lc)) == -1){
							return -1;
						}

						index_instruction = r;
						return r;
						break;

					case FUNCTION_OBJECT_NODE: // should have 1 children



						if((!gacExpression_FunctionObject(_node, _lc))){
							return -1;
						}
						r=CCompiler::getCurrentInstructionIndex();
						break;

					case NEW_OBJECT_NODE:
						if((!gacNew(_node, _lc))){
							return -1;
						}

						r=CCompiler::getCurrentInstructionIndex();
						break;


					default:
						print_error_cr("Unexpected node type %i",eval_node_sp->node_type);
						return -1;
						break;

					}
				}
		}
		else{ // TERMINAL NODE




			//printf("CONST \tE[%i],%s\n",index_instruction,op->value.c_str());
			//if(!var_assign){
				/*bool this_object = checkAccessObjectMember(_node);
				//bool this_object=false;

				if(_node->value_symbol == "this"){
					if(node_access){ // check "this"
						print_error_cr("bad using \"this\" at line %i",-1);
						return -1;
					}
				}

				if(!this_object){*/

				if(!insertLoadValueInstruction(_node, _lc)){
					return -1;

				}


				//}

			//}
		}
		index_instruction=r;
		//r=index_instruction;

	}else{

		if(_node->operator_info->id == TERNARY_IF_PUNCTUATOR){


			if(_node->children[1]->operator_info->id == TERNARY_ELSE_PUNCTUATOR){
				// node children[0]: conditional.
				// node children[1]: body-if
				// node children[2]: body-else
				//inline_if_else = true;
				int t1= gacInlineIf(_node,_lc,index_instruction);

				return t1;


			}else{
				print_error_cr("line %i: Put parenthesis on the inner ternary conditional",_node->definedValueline);
				return -1;
			}


		}else{

			// only inserts terminal symbols...

			if(_node!=NULL && _node->operator_info != NULL){
				access_node = _node->operator_info->id == PUNCTUATOR_TYPE::FIELD_PUNCTUATOR;
			}



			//bool access_node =  _node->operator_info->id == FIELD_PUNCTUATOR;

			// check if there's inline-if-else
			int right=-1, left=-1;


				if((left=gacExpression_Recursive(_node->children[LEFT_NODE], _lc,index_instruction)) == -1){
					return -1;
				}


			if(_node->children.size()==2){
				if((right=gacExpression_Recursive(_node->children[RIGHT_NODE],_lc,index_instruction)) == -1){
					return -1;
				}
			}
			else {
				right = -1;
			}

			r=index_instruction;

			// get last load symbol ..
			if(access_node){
				r=index_instruction-1;
			}



			if(left !=-1 && right!=-1){ // 2 ops

				// Ignore punctuator node. Only take cares about terminal symbols...
				if(!access_node){

				// particular case if operator is =
					//printf("%s\tE[%i],E[%i],E[%i]\n",op->token.c_str(),index_instruction,left,right);
					if(!insertOperatorInstruction(_node->operator_info,_node,error_str,left,right)){
						print_error_cr("%s at line %i",error_str.c_str(),_node->definedValueline);
						return -1;
					}
				//}
				}

			}else if(right!=-1){ // one op..
				//printf("%s\tE[%i],E[%i]\n",op->token.c_str(),index_instruction,right);
				if(!insertOperatorInstruction(_node->operator_info,_node,  error_str,right)){
					print_error_cr("%s at line %i",error_str.c_str(),_node->definedValueline);
					return -1;
				}

			}else if(left!=-1){ // one op..
			//	printf("%s\tE[%i],E[%i]\n",op->token.c_str(),index_instruction,left);
				if(!insertOperatorInstruction(_node->operator_info,_node,error_str,left)){
					print_error_cr("%s at line %i",error_str.c_str(),_node->definedValueline);
					return -1;
				}

			}else{ // ERROR
				print_error_cr("ERROR both ops ==0!");
				return -1;
			}
		}

	}

	// we ignore field node instruction ...
	if(!access_node){
		index_instruction++;
	}


	return r;
}

int findConstructorIdxNode(PASTNode _node ){
	if(_node->node_type!=NODE_TYPE::ARGS_PASS_NODE) {print_error_cr("children[0] is not args_pass_node");return -1;}
	for(unsigned i = 0; i < _node->children.size(); i++){
		if(_node->children[i]->node_type == NODE_TYPE::KEYWORD_NODE){
			if(_node->children[i]->keyword_info != NULL){
				if(_node->children[i]->keyword_info->id==KEYWORD_TYPE::FUNCTION_KEYWORD){
					if(_node->children[i]->value_symbol == _node->value_symbol){
						return i;
					}
				}
			}else{
				print_error_cr("keyword = NULL");
				return -1;
			}
		}

	}


	return -1;
}




bool CCompiler::doRegisterVariableSymbolsClass(const string & class_name, tInfoRegisteredClass *current_class){


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


	PASTNode node_class = current_class->metadata_info.object_info.symbol_info.ast;
	string current_class_name = current_class->metadata_info.object_info.symbol_info.symbol_name;


	// register all vars...
	for(unsigned i = 0; i < node_class->children[0]->children.size(); i++){ // foreach declared var.


		for(unsigned j = 0; j < node_class->children[0]->children[i]->children.size(); j++){ // foreach element declared within ','
			if(CScriptClassFactory::getInstance()->registerVariableSymbol(
					class_name,
					node_class->children[0]->children[i]->children[j]->value_symbol,
					node_class->children[0]->children[i]->children[j]
				) == NULL){
				return false;
			}
		}
	}

	// register all functions ...
	for(unsigned i = 0; i < node_class->children[1]->children.size(); i++){
		tInfoRegisteredFunctionSymbol *irfs;
		PASTNode node_fun = node_class->children[1]->children[i];
		string value_symbol = node_fun->value_symbol;

		if(current_class_name == value_symbol){ // constructor symbol...
			value_symbol = class_name; // rename to be base constructor later ...
		}


		if((irfs=CScriptClassFactory::getInstance()->registerFunctionSymbol(
				class_name,
				value_symbol,
				node_fun
		)) == NULL){
			return false;
		}

		// compile function (within scope class)...
		if(!gacFunction(node_fun, node_class->scope_info_ptr,irfs)){
			return false;
		}

	}



	return true;
}

bool CCompiler::gacClass(PASTNode _node, CScopeInfo * _lc){
	if(_node == NULL) {print_error_cr("NULL node");return false;}
	if(_node->node_type != KEYWORD_NODE || _node->keyword_info == NULL){print_error_cr("node is not keyword type or null");return false;}
	if(_node->keyword_info->id != KEYWORD_TYPE::CLASS_KEYWORD){print_error_cr("node is not CLASS keyword type");return false;}
	if(_node->children.size()==3 && _node->children[2]->node_type != BASE_CLASS_NODE){print_error_cr("expected BASE CLASS keyword type");return false;}

	string base_class= "";
	if(_node->children.size()==3){
		base_class=	_node->children[2]->value_symbol;
	}




	tInfoRegisteredClass *irc;

	// children[0]==var_collection && children[1]=function_collection
	if(_node->children.size()!=2 && _node->children.size()!=3) {print_error_cr("node CLASS has not valid number of nodes");return false;}


	if(_node->children.size() == 3){
		_node->children[2]->value_symbol = base_class;
	}

	// verify class is not already registered...
	if((irc=CScriptClassFactory::getInstance()->registerScriptClass(_node->value_symbol,base_class,_node)) == NULL){
		return false;
	}

	// we push class symbol ref as function, then all registered symbols will take account with this scope...
	pushFunction(_node,&irc->metadata_info);


	// register all symbols ...
	if(!doRegisterVariableSymbolsClass(_node->value_symbol,irc)){
		return false;
	}

	// register class functions...
	/*for(unsigned i = 0; i < _node->children[1]->children.size(); i++){
		tInfoRegisteredFunctionSymbol *irfs;
		PASTNode node_fun = _node->children[1]->children[i];

		if((irfs=CScriptClassFactory::getInstance()->registerFunctionSymbol(
				_node->value_symbol,
				node_fun->value_symbol,
				node_fun
		)) == NULL){
			return false;
		}

		// compile function (within scope class)...
		if(!gacFunction(node_fun, _node->scope_info_ptr,irfs)){
			return false;
		}

	}*/


	// pop class ref so we go back to main scope...
	popFunction();

	return true;
}

bool CCompiler::gacNew(PASTNode _node, CScopeInfo * _lc){
	if(_node == NULL) {print_error_cr("NULL node");return false;}
	if(_node->node_type != NEW_OBJECT_NODE ){print_error_cr("node is not NEW OBJECT NODE type");return false;}
	if(_node->children.size()!=1) {print_error_cr("node NEW has not valid number of nodes");return false;}
	if(_node->children[0]->node_type!=NODE_TYPE::ARGS_PASS_NODE) {print_error_cr("children[0] is not args_pass_node");return false;}

	// create new statment ...
	//CCompiler::getInstance()->newStatment();

	// 1. create object instruction ...
	if(!insert_NewObject_Instruction(_node,_node->value_symbol)) // goto end  ...
	{
		return false;
	}

	int index_created_object = getCurrentStatmentIndex();


	int idx_constructor_node = findConstructorIdxNode(_node->children[0]);



	// 2. load constructor function ...
	if(idx_constructor_node != -1){
		if(!insertLoadValueInstruction(_node->children[idx_constructor_node],_lc)) {
			return false;
		}
		int call_index = getCurrentInstructionIndex();


		// 2. load constructor arguments ...
		// 1. insert push to pass values to all args ...
		PASTNode function_args = _node->children[0];

		if(function_args->children.size() > 0){
		for(unsigned k = 0; k < function_args->children.size(); k++){

			// check whether is expression node...
			if(!gacExpression(function_args->children[k], _lc,getCurrentInstructionIndex()+1)){
				return false;
			}

			if(k==0){

				// insert clear push arguments stack
				insert_ClearArgumentStack_And_PushFirstArgument_Instructions(_node);

				// insert vector access instruction ...
				//insert_PushArgument_Instruction_Less1();

			}else{
				// insert vector access instruction ...
				insert_PushArgument_Instruction(_node);
			}


		}
		}else{
			// clear the stack only ..
			insert_ClearArgumentStack_Instruction(_node);
		}


		// 2. insert call instruction itself.
		insert_CallFunction_Instruction(_node,call_index,index_created_object);
	}






	return true;

}

bool CCompiler::gacFor(PASTNode _node, CScopeInfo * _lc){
	if(_node == NULL) {print_error_cr("NULL node");return false;}
	if(_node->node_type != KEYWORD_NODE || _node->keyword_info == NULL){print_error_cr("node is not keyword type or null");return false;}
	if(_node->keyword_info->id != KEYWORD_TYPE::FOR_KEYWORD){print_error_cr("node is not FOR keyword type");return false;}
	if(_node->children.size()!=4) {print_error_cr("node FOR has not valid number of nodes");return false;}
	if(!(_node->children[0]->node_type==PRE_FOR_NODE && _node->children[1]->node_type==CONDITIONAL_NODE &&
	_node->children[2]->node_type==POST_FOR_NODE && _node->children[3]->node_type==BODY_NODE)) {print_error_cr("node FOR has not valid TYPE nodes");return false;}
	tInfoAsmOp *asm_op;
	//CScopeInfo * _currentScope = _node ->scope_info_ptr;

	// 1. compile var init ...
	if(!ast2asm_Recursive(_node->children[0],_node->scope_info_ptr)){ return false;}

	// 2. compile conditional
	if(!ast2asm_Recursive(_node->children[1],_node->scope_info_ptr)){ return false;}
	// get current index statment in order to jmp from end body for.
	int index_statment_conditional_for_= getCurrentStatmentIndex();

	// insert conditional jmp (if not true go to the end)
	asm_op = insert_JNT_Instruction();


	// 3. compile body
	if(!gacBody(_node->children[3],_node->children[3]->scope_info_ptr)){ return false;}


	// 4. compile post oper
	if(!ast2asm_Recursive(_node->children[2],_node->scope_info_ptr)){ return false;}

	// 5. jmp to the conditional index ...
	insert_JMP_Instruction(index_statment_conditional_for_);

	// save jmp value...
	asm_op->index_op1=getCurrentStatmentIndex()+1;

	return true;
}

bool CCompiler::gacWhile(PASTNode _node, CScopeInfo * _lc){
	if(_node == NULL) {print_error_cr("NULL node");return false;}
	if(_node->node_type != KEYWORD_NODE || _node->keyword_info == NULL){print_error_cr("node is not keyword type or null");return false;}
	if(_node->keyword_info->id != KEYWORD_TYPE::WHILE_KEYWORD){print_error_cr("node is not WHILE keyword type");return false;}
	if(_node->children.size()!=2) {print_error_cr("node WHILE has not valid number of nodes");return false;}
	if(!(_node->children[0]->node_type==CONDITIONAL_NODE && _node->children[1]->node_type==BODY_NODE )) {print_error_cr("node WHILE has not valid TYPE nodes");return false;}
	tInfoAsmOp *asm_op_jmp_end;
	int index_ini_while;


	// compile conditional expression...
	if(!ast2asm_Recursive(_node->children[0],_lc)){ return false;}
	index_ini_while = getCurrentStatmentIndex();
	asm_op_jmp_end = insert_JNT_Instruction(); // goto end  ...

	// compile if-body ...
	if(!gacBody(_node->children[1],_lc)){ return false;}
	insert_JMP_Instruction(index_ini_while); // goto end  ...

	// save jmp value ...
	asm_op_jmp_end->index_op1= getCurrentStatmentIndex()+1;
	return true;
}

bool CCompiler::gacReturn(PASTNode _node, CScopeInfo * _lc){

	if(_node == NULL) {print_error_cr("NULL node");return false;}
	if(_node->node_type != KEYWORD_NODE || _node->keyword_info == NULL){print_error_cr("node is not keyword type or null");return false;}
	if(_node->keyword_info->id != KEYWORD_TYPE::RETURN_KEYWORD){print_error_cr("node is not RETURN keyword type");return false;}
	if(_node->children.size() != 1){print_error_cr("node RETURN has not 1 child");return false;}

	if(gacExpression(_node->children[0], _lc)){

		// finally we put mov to Value ...
		insertRet(_node,getCurrentInstructionIndex());
	}
	else {
		return false;
	}

	return true;
}

bool CCompiler::gacFunction(PASTNode _node, CScopeInfo * _lc, tInfoRegisteredFunctionSymbol *irfs){

	if(_node == NULL) {print_error_cr("NULL node");return false;}
	//if(!(_node->node_type == KEYWORD_NODE && _node->keyword_info != NULL) && !(_node->node_type != FUNCTION_OBJECT_NODE))>{print_error_cr("node is not keyword type or null");return false;}

	if((_node->keyword_info != NULL)){
		if(_node->keyword_info->id != KEYWORD_TYPE::FUNCTION_KEYWORD) {print_error_cr("node is not FUNCTION keyword type");return false;}
	}else{
		if((_node->node_type != FUNCTION_OBJECT_NODE))
		{
			print_error_cr("node is not FUNCTION OBJECT NODE type or null");
			return false;
		}
	}

	//if(!(_node->keyword_info->id == KEYWORD_TYPE::FUNCTION_KEYWORD) && !(_node->node_type != FUNCTION_OBJECT_NODE)){print_error_cr("node is not FUNCTION keyword type");return false;}
	if(_node->children.size() != 2){print_error_cr("node FUNCTION has not 2 child");return false;}
	if(_node->children[0]->node_type != NODE_TYPE::ARGS_DECL_NODE){print_error_cr("node FUNCTION has not ARGS node");return false;}
	if(_node->children[1]->node_type != NODE_TYPE::BODY_NODE){print_error_cr("node FUNCTION has not BODY node");return false;}


	//CScopeInfo * scope_body = ast_body_node->scope_info_ptr;

	//int local_function_idx=-1;
	// 1. Get the registered symbol.
	/*tInfoScopeVar * irv=_lc->getInfoRegisteredSymbol(_node->value_symbol,false);
	if(irv == NULL){
		print_error_cr("Cannot get registered function %s",_node->value_symbol.c_str());
		return false;
	}*/


	// 2. Processing args ...

	for(unsigned i = 0; i < _node->children[0]->children.size(); i++){
		irfs->m_arg.push_back(_node->children[0]->children[i]->value_symbol);
	}

	// 2. Compiles the function ...


	return compile(_node->children[1], irfs);
}

bool CCompiler::gacIf(PASTNode _node, CScopeInfo * _lc){
	if(_node == NULL) {print_error_cr("NULL node");return false;}
	if(_node->node_type != KEYWORD_NODE || _node->keyword_info == NULL){print_error_cr("node is not keyword type or null");return false;}
	if(_node->keyword_info->id != KEYWORD_TYPE::IF_KEYWORD){print_error_cr("node is not IF keyword type");return false;}
	if(_node->children.size()<2) {print_error_cr("node IF has not valid number of nodes");return false;}
	if(!(_node->children[0]->node_type==CONDITIONAL_NODE && _node->children[1]->node_type==BODY_NODE )) {print_error_cr("node IF has not valid TYPE nodes");return false;}
	tInfoAsmOp *asm_op_jmp_else_if,*asm_op_jmp_end;

	// compile conditional expression...
	if(!ast2asm_Recursive(_node->children[0],_lc)){ return false;}
	asm_op_jmp_else_if = insert_JNT_Instruction(); // goto else body ...

	// compile if-body ...
	if(!gacBody(_node->children[1],_lc)){ return false;}


	// if there's else body, compile-it
	if(_node->children.size()==3){
		asm_op_jmp_end = insert_JMP_Instruction(); // goto end
		asm_op_jmp_else_if->index_op1 = getCurrentStatmentIndex()+1;
		if(!gacBody(_node->children[2],_lc)){ return false;}

		asm_op_jmp_end->index_op1 = getCurrentStatmentIndex()+1;

	}
	else{
		asm_op_jmp_else_if->index_op1 = getCurrentStatmentIndex()+1;
	}
	return true;
}

int CCompiler::gacInlineIf(PASTNode _node, CScopeInfo * _lc, int & instruction){

	if(_node == NULL) {print_error_cr("NULL node");return -1;}
	if(_node->node_type != PUNCTUATOR_NODE || _node->operator_info == NULL){print_error_cr("node is not punctuator type or null");return -1;}
	if(_node->operator_info->id != TERNARY_IF_PUNCTUATOR){print_error_cr("node is not INLINE-IF PUNCTUATOR type");return -1;}
	if(_node->children.size()!=2) {print_error_cr("node INLINE-IF has not 2 nodes");return -1;}
	if(!(_node->children[1]->node_type==PUNCTUATOR_NODE && _node->children[1]->operator_info->id==TERNARY_ELSE_PUNCTUATOR )) {print_error_cr("node INLINE-ELSE has not found");return -1;}
	if(_node->children[1]->children.size() != 2) {print_error_cr("node INLINE-ELSE has not 2 nodes");return -1;}
	tInfoAsmOp *asm_op_jmp_else_if,*asm_op_jmp_end;


	int r=instruction;

	// compile conditional expression...
	if((r=gacExpression_Recursive(_node->children[0],_lc,r))==-1){ return -1;}
	asm_op_jmp_else_if = insert_JNT_Instruction(); // goto else body ...

	// compile if-body ...
	r+=2; // in order to execute recursive expression we have to advance r pointer jnt (+2)
	if((r=gacExpression_Recursive(_node->children[1]->children[0],_lc,r))==-1){ return -1;}

	insert_Save_CurrentInstruction();


	// compile else-body ...
	asm_op_jmp_end = insert_JMP_Instruction(); // goto end+


	asm_op_jmp_else_if->index_op1 = getCurrentStatmentIndex();
	asm_op_jmp_else_if->index_op2 = getCurrentInstructionIndex()+1;

	r+=3;

	if((r=gacExpression_Recursive(_node->children[1]->children[1],_lc,r))==-1){ return -1;}

	insert_Save_CurrentInstruction();
	insert_Load_CurrentInstruction();

	r+=2; // add +2 load +save ...

	asm_op_jmp_end->index_op1 = getCurrentStatmentIndex();
	asm_op_jmp_end->index_op2 = getCurrentInstructionIndex();

	instruction = r+1;

	return r;
}

bool CCompiler::gacSwitch(PASTNode _node, CScopeInfo * _lc){

	if(_node == NULL) {print_error_cr("NULL node");return false;}
	if(_node->node_type != KEYWORD_NODE || _node->keyword_info == NULL){print_error_cr("node is not keyword type or null");return false;}
	if(_node->keyword_info->id != SWITCH_KEYWORD){print_error_cr("node is not SWITCH keyword type");return false;}
	bool has_default = false;
	PASTNode switch_node;
	PASTNode group_cases;
	PASTNode case_value;
	PASTNode case_body;

	tInfoAsmOp * asm_op;

	string error_str;
	string detected_type_str;
	CScopeInfo *_scope = this->m_currentFunctionInfo->object_info.symbol_info.ast->scope_info_ptr->getCurrentScopePointer();

	// create new statment ...
	CCompiler::getInstance()->newStatment();


	if(insertLoadValueInstruction(_node,_scope)){ // insert condition value ...

		// get current instruction value to take as ref for compare within value cases...
		int switch_value_index  = getCurrentInstructionIndex();

		// the stratege is first evaluate all cases and then their bodies...
		for(unsigned s=0; s < 3; s++){
			for(unsigned i = 0; i < _node->children.size(); i++){ // expect node type group cases ...

				switch_node = _node->children[i];

				if(switch_node != NULL){ // the rules are the following children[0]:group_cases and children[1]:body_case

					if(switch_node->children.size() == 2){
						group_cases = switch_node->children[0];
						case_body = switch_node->children[1];

						switch(s){

						case 0: // GENERATE ASM FOR CONDITIONAL CASES ...

							if(group_cases->node_type == GROUP_CASES_NODE){

								//int condition_index  = getCurrentInstructionIndex();

								for(unsigned j = 0; j < group_cases->children.size(); j++){ // generate condition case ...
									case_value = group_cases->children[j];

									if(case_value->node_type == KEYWORD_NODE && case_value->keyword_info != NULL){

										switch(case_value->keyword_info->id){
										default:
											print_error_cr("Unexpected %s keyword node in SWITCH node",case_value->keyword_info->str);
											break;
										case DEFAULT_KEYWORD:

											if(!has_default){
												has_default = true;
												// insert jmp instruction and save its information to store where to jmp when we know the total code size of cases + body...
												case_value->aux_value = insert_JMP_Instruction();
											}else{
												print_error_cr("case already defined!");
												return false;
											}
											break;
										case CASE_KEYWORD:

											// load case X:
											insertLoadValueInstruction(case_value,_scope);

											// is equal ? ==
											if(!insertOperatorInstruction(&CAst::defined_operator_punctuator[LOGIC_EQUAL_PUNCTUATOR],0, error_str, switch_value_index ,getCurrentInstructionIndex())){
													print_error_cr("%s",error_str.c_str());
													return false;
											}

											// insert jmp instruction and save its information to store where to jmp when we know the total code size of cases...
											case_value->aux_value = insert_JT_Instruction();

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

						case 1: // GENERATE ASM FOR BODY AND WRITE initial JMP

							if(gacBody(case_body,_lc)){
								for(unsigned i = 0; i < group_cases->children.size(); i++){
									case_value = group_cases->children[i];
									asm_op = (tInfoAsmOp *)case_value->aux_value; // load jt instruction and set current instruction before write asm code.
									asm_op->index_op1 = getCurrentStatmentIndex();
								}

								if(i < (_node->children.size()-1))
									case_body->aux_value = insert_JMP_Instruction();
							}else{
								return false;
							}
							break;

						case 2: // FINALLY, WRITE JMP's to end statment

							if((asm_op = (tInfoAsmOp *)case_body->aux_value) != NULL){
								asm_op->index_op1 = getCurrentStatmentIndex()+1;
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
	}

	return true;
}

bool CCompiler::gacVar(PASTNode _node, CScopeInfo * _lc){
	if(_node == NULL) {print_error_cr("NULL node");return false;}
	if(_node->node_type != KEYWORD_NODE || _node->keyword_info == NULL){print_error_cr("node is not keyword type or null");return false;}
	if(_node->keyword_info->id != VAR_KEYWORD){print_error_cr("node is not VAR keyword type");return false;}

	int local_variable_idx;

	for(unsigned i = 0; i < _node->children.size(); i++){ // for all vars ...

		if(localVarSymbolExists(_node->children[i]->value_symbol, _node)){
			print_error_cr("Variable \"%s\" already defined !",_node->value_symbol.c_str());
			return false;
		}

		if((local_variable_idx=addLocalVarSymbol(_node->children[i]->value_symbol,_node->children[i])) == -1){
			return false;
		}

		//for(unsigned i = 0 ; i < _node->children.size(); i++){ // init all requested vars...
		if(_node->children[i]->children.size()==1){
			if(!gacExpression(_node->children[i]->children[0], _lc)){
				return false;
			}
		}
		//}


	}



	return true;
}

bool CCompiler::gacKeyword(PASTNode _node, CScopeInfo * _lc){

	int idx=0;
	if(_node == NULL) {print_error_cr("NULL node");return false;}
	if(_node->node_type != KEYWORD_NODE || _node->keyword_info == NULL){print_error_cr("node is not keyword type or null");return false;}

	switch(_node->keyword_info->id){
	default:
		print_error_cr("Keyword [ %s ] not implemented yet!",_node->keyword_info->str);
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
	case KEYWORD_TYPE::IF_KEYWORD:
		return gacIf(_node, _lc);
		break;
	case KEYWORD_TYPE::VAR_KEYWORD:
		return gacVar(_node, _lc);
		break;
	case KEYWORD_TYPE::FUNCTION_KEYWORD: // don't compile function. It will compiled later, after main body

		if(functionSymbolExists(_node->value_symbol, _node)){
				print_error_cr("Function \"%s\" already defined !",_node->value_symbol.c_str());
				return false;
		}

		if((idx=addLocalFunctionSymbol(_node->value_symbol,_node)) == -1){
			return false;
		}

		return gacFunction(_node, _lc,&this->m_currentFunctionInfo->object_info.local_symbols.m_registeredFunction[idx]);

		break;
	case KEYWORD_TYPE::RETURN_KEYWORD:
		return gacReturn(_node, _lc);
		break;
	case KEYWORD_TYPE::NEW_KEYWORD:
		return gacNew(_node, _lc);
		break;

	}

	return false;
}

bool CCompiler::gacBody(PASTNode _node, CScopeInfo * _lc){
	if(_node == NULL) {print_error_cr("NULL node");return false;}
	if(_node->node_type != BODY_NODE ){print_error_cr("node is not BODY type or null");return false;}

	if(_node->children.size() > 0){ // body has code ...

		for(unsigned i = 0; i < _node->children.size(); i++){
			if(!ast2asm_Recursive(_node->children[i], _lc))
				return false;
		}

		return true;
	}else{ // insert one statment at least ..
		newStatment();
	}

	return true;
}

bool CCompiler::gacExpression(PASTNode _node, CScopeInfo *_lc,int index_instruction){

	if(index_instruction == -1){ // create new statment
		//int index_instruction=0;
		tInfoStatementOp i_stat;
		(*m_currentListStatements).push_back(i_stat);
		index_instruction = 0; // set as 0
	}

	if(_node == NULL) {print_error_cr("NULL node");return false;}
	if(_node->node_type != EXPRESSION_NODE){print_error_cr("node is not Expression");return false;}

	return gacExpression_Recursive(_node->children[0], _lc,index_instruction) != -1;
}

bool CCompiler::ast2asm_Recursive(PASTNode _node, CScopeInfo *_lc){

	if(_node != NULL){
		switch(_node->node_type){
			default:
			case UNKNOWN_NODE:
				print_info_cr("UNKNOWN_NODE (%i)",_node->node_type);
				return false;
				break;
			break;
			break;
			case MAIN_NODE:print_info_cr("MAIN_NODE");
			break;
			case EXPRESSION_NODE: // in fact is EXPRESSION NODE
				print_info_cr("EXPRESSION_NODE");
				return gacExpression(_node, _lc);
				break;
			case GROUP_CASES_NODE:
				print_info_cr("GROUP_CASES_NODE");
				break;
			case KEYWORD_NODE:
				print_info_cr("KEYWORD_NODE %s",_node->keyword_info->str);
				return gacKeyword(_node, _lc);
				break;

			case BODY_NODE:
				print_info_cr("BODY_NODE");
				return gacBody(_node, _node->scope_info_ptr); // we pass scope node
				break;
			case POST_FOR_NODE:
			case CONDITIONAL_NODE:
				print_info_cr("%s",_node->node_type == CONDITIONAL_NODE ? "CONDITIONAL_NODE":"POST_FOR_NODE");
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
				print_info_cr("FUNCTION_OBJECT");break;
				break;
			case CLASS_VAR_COLLECTION_NODE:print_info_cr("CLASS_VAR_COLLECTION_NODE");break;
			case CLASS_FUNCTION_COLLECTION_NODE:print_info_cr("CLASS_FUNCTION_COLLECTION_NODE");break;
			case BASE_CLASS_NODE:print_info_cr("BASE_CLASS_NODE");break;
			case CALLING_OBJECT_NODE:print_info_cr("CALLING_OBJECT_NODE");break;
		}
	}else{
		print_error_cr("Node is null!");
	}


	return false;
}

void CCompiler::pushFunction(PASTNode _node,tInfoRegisteredFunctionSymbol *sf){
	stk_scriptFunction.push_back(m_currentFunctionInfo);

	this->m_currentFunctionInfo = sf;
	this->m_currentListStatements = &sf->object_info.statment_op;
	this->m_treescope = _node->scope_info_ptr;

}

void CCompiler::popFunction(){

	tInfoRegisteredFunctionSymbol *aux_sf = stk_scriptFunction[stk_scriptFunction.size()-1];
	stk_scriptFunction.pop_back();
	m_currentFunctionInfo = aux_sf;


	if(m_currentFunctionInfo != NULL){
		this->m_currentListStatements = &m_currentFunctionInfo->object_info.statment_op;
		this->m_treescope = m_currentFunctionInfo->object_info.symbol_info.ast->scope_info_ptr;
	}
}

bool CCompiler::compile(PASTNode _node, tInfoRegisteredFunctionSymbol *sf){

	if(_node == NULL){
		print_error_cr("NULL node!");
		return false;
	}


	if(_node->node_type == NODE_TYPE::BODY_NODE ){
		pushFunction(_node,sf);


		// reset current pointer ...
		//m_treescope->resetScopePointer();

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
		print_error_cr("Main node expected");
	}

	return false;
}
/*
bool CCompiler::compile(const string & s, tInfoRegisteredFunctionSymbol * sf){

	// generate whole AST

	if(CAst::generateAST(s.c_str(),sf, &sf->)){

		if(ast2asm(sf->symbol_info.ast,sf)){
			// print generated asm ...
			CCompiler::printGeneratedCode(sf);
			return true;
		}
		// then you have all information -> compile into asm!
		//generateAsmCode(root);
	}

	return false;
}
*/
CCompiler::~CCompiler(){

}
