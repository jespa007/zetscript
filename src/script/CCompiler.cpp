#include "zg_script.h"

#ifdef __DEBUG__ // incoment __VERBOSE_MESSAGE__ to print all messages (wrning is going to be slow because of the prints)
//#define __VERBOSE_MESSAGE__
#endif



#ifdef  __VERBOSE_MESSAGE__

#define print_com_cr print_ast_cr
#else
#define print_com_cr(s,...)
#endif


CCompiler::tDefOperator CCompiler::def_operator[MAX_OPERATORS];
map<string, CObject *> * CCompiler::constant_pool=NULL;
char CCompiler::print_aux_load_value[512];


CCompiler *CCompiler::m_compiler = NULL;


CObject *CCompiler::getConstant(const string & const_name){

	if((*constant_pool).count(const_name) == 1){
		return (*constant_pool)[const_name];
	}
	return NULL;
}

void CCompiler::addConstant(const string & const_name, CObject *obj){
	if(getConstant(const_name) == NULL){
		(*constant_pool)[const_name]=obj;
	}else{
		print_error_cr("constant %s already exist",const_name.c_str());
	}
}

CCompiler *CCompiler::getInstance(){
	if(m_compiler == NULL){
		constant_pool = new map<string,CObject *>;
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

const char * CCompiler::getStrTypeLoadValue(CCompiler::tInfoAsmOp * iao){

	if(iao->operator_type != LOAD){
		return "ERROR";
	}


	string value_symbol="Unknown";

	if(iao->ast_node != NULL){
		value_symbol = iao->ast_node->value_symbol;
	}


	sprintf(print_aux_load_value,"UNDEFINED");
	switch(iao->index_op1){
	case LOAD_TYPE::LOAD_TYPE_CONSTANT:

		sprintf(print_aux_load_value,"CONST(%s)",value_symbol.c_str());
		break;
	case LOAD_TYPE::LOAD_TYPE_VARIABLE:

		sprintf(print_aux_load_value,"VAR(%s)",value_symbol.c_str());
		break;
	case LOAD_TYPE::LOAD_TYPE_ARGUMENT:
		sprintf(print_aux_load_value,"ARG(%s)",value_symbol.c_str());
		break;
	default:
		break;

	}

	return print_aux_load_value;
}

const char * CCompiler::getStrMovVar(CCompiler::tInfoAsmOp * iao){

	if(iao->operator_type != MOV){
		return "ERROR";
	}

	string value_symbol="Unknown";

	if(iao->ast_node != NULL){
		value_symbol = iao->ast_node->value_symbol;
	}

	sprintf(print_aux_load_value,"VAR(%s)",value_symbol.c_str());

	return print_aux_load_value;
}

void CCompiler::printGeneratedCode_Recursive(CScriptFunction *fs){

	vector<tInfoStatementOp> * m_listStatements = fs->getCompiledCode();
	string pre="";
	string post="";

	for(unsigned s = 0; s < (*m_listStatements).size();s++){
		vector<tInfoAsmOp *> * asm_op_statment = &(*m_listStatements)[s].asm_op;

		printf("\n[%s]\n\n","file.zs");

		for(unsigned i = 0; i  <  asm_op_statment->size(); i++){

			int n_ops=0;
			int index_op1 = (*asm_op_statment)[i]->index_op1;
			int index_op2 = (*asm_op_statment)[i]->index_op2;

			if(index_op1 != -1)
				n_ops++;

			 if(index_op2 != -1)
				 n_ops++;

			 pre="";
			 post="";

				switch((*asm_op_statment)[i]->pre_post_operator_type){
				case ASM_PRE_POST_OPERATORS::PRE_INC:
					pre="++";
					break;
				case ASM_PRE_POST_OPERATORS::PRE_DEC:
					pre="--";
					break;
				case ASM_PRE_POST_OPERATORS::POST_INC:
					post="++";
					break;
				case ASM_PRE_POST_OPERATORS::POST_DEC:
					post="--";
					break;
				default:
					break;

				}
			switch((*asm_op_statment)[i]->operator_type){
			case  LOAD:
				printf("[%02i:%02i]\t%s\t%s%s%s\n",s,i,def_operator[(*asm_op_statment)[i]->operator_type].op_str,pre.c_str(),getStrTypeLoadValue((*asm_op_statment)[i]),post.c_str());
				break;
			//case  MOV:
			//	printf("[%02i:%02i]\t%s\t%s,[%02i:%02i]\n",s,i,def_operator[(*asm_op_statment)[i]->operator_type].op_str,getStrMovVar((*asm_op_statment)[i]),s,index_op2);
			//	break;
			case JNT:
			case JT:
			case JMP:
				printf("[%02i:%02i]\t%s\t[%04i]\n",s,i,def_operator[(*asm_op_statment)[i]->operator_type].op_str,(*asm_op_statment)[i]->index_op1);
				break;
			/*case PRE_INC:
			case POST_INC:
			case PRE_DEC:
			case POST_DEC:
				printf("[%02i:%02i]\t%s\n",s,i,def_operator[(*asm_op_statment)[i]->operator_type].op_str);
				break;*/
			case VGET:
			case VPUSH:
				printf("[%02i:%02i]\t%s\t%s[%02i:%02i]%s,[%02i:%02i]\n",s,i,def_operator[(*asm_op_statment)[i]->operator_type].op_str,pre.c_str(),s,index_op1,post.c_str(),s,index_op2);
				break;
			default:

				if(n_ops==0){
					printf("[%02i:%02i]\t%s\n",s,i,def_operator[(*asm_op_statment)[i]->operator_type].op_str);
				}else if(n_ops==1){
					printf("[%02i:%02i]\t%s\t[%02i:%02i]\n",s,i,def_operator[(*asm_op_statment)[i]->operator_type].op_str,s,index_op1);
				}else{
					printf("[%02i:%02i]\t%s\t[%02i:%02i],[%02i:%02i]\n",s,i,def_operator[(*asm_op_statment)[i]->operator_type].op_str,s,index_op1,s,index_op2);
				}
				break;
			}
		}
	}
	// and then print its functions ...
	vector<CScriptFunction *> * m_vf = fs->getVectorFunction();

	for(unsigned j =0; j < m_vf->size(); j++){

		if(m_vf->at(j)->getType() == CScriptFunction::TYPE::SCRIPT_FUNCTION_TYPE){

			print_info_cr("-------------------------------------------------------");
			print_info_cr("");
			print_info_cr("Code for %s",(*m_vf)[j]->getName().c_str());
			print_info_cr("");
			printGeneratedCode_Recursive((*m_vf)[j]);
		}
	}
}

void CCompiler::printGeneratedCode(CScriptFunction *fs){
	printGeneratedCode_Recursive(fs);
}

//------------------------------------------------------------------------------------------------------------------
//
// CONSTRUCTOR
//

CCompiler::CCompiler(){
	m_currentListStatements = NULL;
	m_currentScriptFunction = NULL;
	m_treescope = NULL;

	//		VAR  			|	STR   | ID | NUM OP
	//----------------------+---------+----+-------
	def_operator[NOP]         ={"NOP" ,NOP ,0      };
	def_operator[MOV]         ={"MOV" ,MOV ,2}; // mov expression to var
	def_operator[LOAD]        ={"LOAD",LOAD ,1}; // primitive value like number/string or boolean...
	def_operator[EQU]         ={"EQU" ,EQU ,2};  // ==
	def_operator[LT]          ={"LT"  ,LT ,2};  // <
	def_operator[LTE]         ={"LTE" ,LTE ,2};  // <=
	def_operator[NOT]         ={"NOT" ,NOT,1}; // !
	def_operator[GT]          ={"GT"  ,GT,2};  // >
	def_operator[GTE]         ={"GTE" ,GTE,2}; // >=
	//def_operator[NEG]         ={"NEG",NEG,1}; // !
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
	def_operator[JMP]         ={"JMP",JMP,1};
	def_operator[JNT]         ={"JNT",JNT,1}; // goto if not true ... goes end to conditional.
	def_operator[JT]          ={"JT",JT,1}; // goto if true ... goes end to conditional.

	def_operator[CALL]={"CALL",CALL,1}; // calling function after all of args are processed...
	def_operator[PUSH]={"PUSH",PUSH,1};
	def_operator[CLR]={"CLR",CLR,0};
	def_operator[VGET]={"VGET",VGET,1}; // vector access after each index is processed...

	def_operator[VEC]={"VEC",VEC,1}; // Vector object (CREATE)

	def_operator[VPUSH]={"VPUSH",VPUSH,1}; // Value push for vector
	def_operator[VPOP]={"VPOP",VPOP,1}; // Value pop for vector
	def_operator[RET]={"RET",RET,1}; // Value pop for vector

}

//------------------------------------------------------------------------------------------------------------------
//
// COMPILE COMPILER MANAGEMENT
//
int CCompiler::getCurrentInstructionIndex(){
	CCompiler::tInfoStatementOp *ptr_current_statement_op = &(*m_currentListStatements)[m_currentListStatements->size()-1];
	return ptr_current_statement_op->asm_op.size()-1;
}

int CCompiler::getCurrentStatmentIndex(){
	return (int)(m_currentListStatements->size()-1);
}

CCompiler::tInfoStatementOp * CCompiler::newStatment(){
	CCompiler::tInfoStatementOp st;

	m_currentListStatements->push_back(st);

	return  &(*m_currentListStatements)[m_currentListStatements->size()-1];
}

bool CCompiler::insertLoadValueInstruction(PASTNode _node, CScope * _lc){

	string v = _node->value_symbol;
	int m_var_at_line = _node->definedValueline;
	ASM_PRE_POST_OPERATORS pre_post_operator_type =ASM_PRE_POST_OPERATORS::UNKNOW_PRE_POST_OPERATOR;

	CCompiler::tInfoStatementOp *ptr_current_statement_op = &(*m_currentListStatements)[m_currentListStatements->size()-1];
	CObject *obj, *get_obj;
	CVariable::VAR_TYPE type=CVariable::OBJECT;
	CCompiler::LOAD_TYPE load_type=CCompiler::LOAD_TYPE_NOT_DEFINED;

	if(_node->pre_post_operator_info != NULL){

		pre_post_operator_type=getNumberOperatorId_OneOp(_node->pre_post_operator_info->id);
	}
	// try parse value...
	if((obj=CInteger::Parse(v))!=NULL){
			type=CVariable::INTEGER;
			load_type=LOAD_TYPE_CONSTANT;
			print_com_cr("%s detected as int\n",v.c_str());
			if((get_obj = getConstant(v))!=NULL){
				delete obj;
				obj = get_obj;
			}else{
				addConstant(v,obj);
			}
	}
	else if((obj=CNumber::Parse(v))!=NULL){
		type=CVariable::NUMBER;
		load_type=LOAD_TYPE_CONSTANT;
		print_com_cr("%s detected as float\n",v.c_str());

		if((get_obj = getConstant(v))!=NULL){
			delete obj;
			obj = get_obj;
		}else{
			addConstant(v,obj);
		}
	}
	else if(v[0]=='\"' && v[v.size()-1]=='\"'){
		type=CVariable::STRING;
		load_type=LOAD_TYPE_CONSTANT;
		print_com_cr("%s detected as string\n",v.c_str());

		if((get_obj = getConstant(v))!=NULL){
			obj = get_obj;
		}else{
			string s=v.substr(1,v.size()-2);
			CString *os=new CString();
			os->m_value = s;
			obj = os;
			addConstant(v,obj);
		}

	}
	else if((obj=CBoolean::Parse(v))!=NULL){
		type=CVariable::BOOLEAN;
		load_type=LOAD_TYPE_CONSTANT;
		print_com_cr("%s detected as boolean\n",v.c_str());

		if((get_obj = getConstant(v))!=NULL){
			delete obj;
			obj = get_obj;
		}else{
			addConstant(v,obj);
		}
	}else{
		CScope::tInfoRegisteredVar * info_var=_lc->getInfoRegisteredSymbol(v,false);
		type=CVariable::OBJECT;
		load_type=LOAD_TYPE_VARIABLE;


		if(info_var==NULL){
			print_error_cr("symbol %s at line %i is not declared ", v.c_str(),m_var_at_line);
			return false;
		}

		// if object check whether has pre/post inc/dec

		obj = info_var->m_obj;
	}

	if((pre_post_operator_type ==ASM_PRE_POST_OPERATORS::POST_DEC ||
		pre_post_operator_type ==ASM_PRE_POST_OPERATORS::POST_INC ||
		pre_post_operator_type ==ASM_PRE_POST_OPERATORS::PRE_DEC ||
		pre_post_operator_type ==ASM_PRE_POST_OPERATORS::PRE_INC
		) && type!=CVariable::OBJECT){
		print_error_cr("line %i: operation \"%s\" not allowed for constants ",_node->definedValueline,_node->pre_post_operator_info->str);
		return false;

	}

	CCompiler::tInfoAsmOp *asm_op = new CCompiler::tInfoAsmOp();

	asm_op->variable_type=type;
	asm_op->index_op1=load_type;
	asm_op->index_op2=(int)obj;
	asm_op->ast_node=_node;
	asm_op->pre_post_operator_type=pre_post_operator_type;

	asm_op->operator_type=CCompiler::ASM_OPERATOR::LOAD;
	ptr_current_statement_op->asm_op.push_back(asm_op);

	return true;
}

bool CCompiler::insertMovVarInstruction(int left_index, int right_index){

	CCompiler::tInfoStatementOp *ptr_current_statement_op = &(*m_currentListStatements)[m_currentListStatements->size()-1];

	CCompiler::tInfoAsmOp * left_asm_op = ptr_current_statement_op->asm_op[left_index];

	// check whether left operant is object...
	if(left_asm_op->variable_type != CVariable::OBJECT){
		int line = -1;

		if(left_asm_op->ast_node!=NULL)
			line=left_asm_op->ast_node->definedValueline;
		print_error_cr("line %i. left operand must be l-value for '=' operator",line);
		return false;
	}

	CCompiler::tInfoAsmOp *asm_op = new CCompiler::tInfoAsmOp();
	asm_op->index_op1 = left_index;//&((*m_currentListStatements)[dest_statment]);
	asm_op->index_op2 =  right_index;
	//asm_op->symbol_name="";
	asm_op->operator_type=CCompiler::ASM_OPERATOR::MOV;

	ptr_current_statement_op->asm_op.push_back(asm_op);

	return true;
}

CCompiler::tInfoAsmOp * CCompiler::insert_JMP_Instruction(int jmp_statement){

	CCompiler::tInfoStatementOp *ptr_current_statement_op = &(*m_currentListStatements)[m_currentListStatements->size()-1];
	CCompiler::tInfoAsmOp *asm_op = new CCompiler::tInfoAsmOp();
	asm_op->index_op1 = jmp_statement;//&((*m_currentListStatements)[dest_statment]);
	asm_op->operator_type=CCompiler::ASM_OPERATOR::JMP;
	ptr_current_statement_op->asm_op.push_back(asm_op);
	//printf("[%02i:%02i]\tJMP\t[??]\n",m_currentListStatements->size(),ptr_current_statement_op->asm_op.size());

	return asm_op;
}

CCompiler::tInfoAsmOp * CCompiler::insert_JNT_Instruction(int jmp_statement){

	CCompiler::tInfoStatementOp *ptr_current_statement_op = &(*m_currentListStatements)[m_currentListStatements->size()-1];
	CCompiler::tInfoAsmOp *asm_op = new CCompiler::tInfoAsmOp();
	asm_op->index_op1 = jmp_statement;//&((*m_currentListStatements)[dest_statment]);
	asm_op->operator_type=CCompiler::ASM_OPERATOR::JNT;

	//printf("[%02i:%02i]\tJNT\t[%02i:%02i],[??]\n",m_currentListStatements->size(),ptr_current_statement_op->asm_op.size(),m_currentListStatements->size(),ptr_current_statement_op->asm_op.size()-1);
	ptr_current_statement_op->asm_op.push_back(asm_op);

	return asm_op;
}

CCompiler::tInfoAsmOp * CCompiler::insert_JT_Instruction(int jmp_statement){

	CCompiler::tInfoStatementOp *ptr_current_statement_op = &(*m_currentListStatements)[m_currentListStatements->size()-1];
	CCompiler::tInfoAsmOp *asm_op = new CCompiler::tInfoAsmOp();
	asm_op->index_op1 = jmp_statement;//&((*m_currentListStatements)[dest_statment]);
	asm_op->operator_type=CCompiler::ASM_OPERATOR::JT;
	//printf("[%02i:%02i]\tJT \t[%02i:%02i],[??]\n",m_currentListStatements->size(),ptr_current_statement_op->asm_op.size(),m_currentListStatements->size(),ptr_current_statement_op->asm_op.size()-1);
	ptr_current_statement_op->asm_op.push_back(asm_op);
	return asm_op;
}

void CCompiler::insert_NOP_Instruction(){
	CCompiler::tInfoStatementOp *ptr_current_statement_op = &(*m_currentListStatements)[m_currentListStatements->size()-1];
	CCompiler::tInfoAsmOp *asm_op = new CCompiler::tInfoAsmOp();
	asm_op->index_op1 = 0;//&((*m_currentListStatements)[dest_statment]);
	asm_op->operator_type=CCompiler::ASM_OPERATOR::NOP;
	//printf("[%02i:%02i]\tNOP\n",m_currentListStatements->size(),ptr_current_statement_op->asm_op.size());
	ptr_current_statement_op->asm_op.push_back(asm_op);

}

void CCompiler::insert_CreateArrayObject_Instruction(){
	CCompiler::tInfoStatementOp *ptr_current_statement_op = &(*m_currentListStatements)[m_currentListStatements->size()-1];
	CCompiler::tInfoAsmOp *asm_op = new CCompiler::tInfoAsmOp();

	asm_op->operator_type=CCompiler::ASM_OPERATOR::VEC;
	asm_op->variable_type = CVariable::OBJECT;
	//printf("[%02i:%02i]\tJT \t[%02i:%02i],[??]\n",m_currentListStatements->size(),ptr_current_statement_op->asm_op.size(),m_currentListStatements->size(),ptr_current_statement_op->asm_op.size()-1);
	ptr_current_statement_op->asm_op.push_back(asm_op);

}

void CCompiler::insert_ArrayAccess_Instruction(int vec_object, int index_instrucction, tASTNode *_ast){
	CCompiler::tInfoStatementOp *ptr_current_statement_op = &(*m_currentListStatements)[m_currentListStatements->size()-1];
	CCompiler::tInfoAsmOp *asm_op = new CCompiler::tInfoAsmOp();
	asm_op->index_op1 = vec_object;//&((*m_currentListStatements)[dest_statment]);
	asm_op->index_op2 = index_instrucction;//&((*m_currentListStatements)[dest_statment]);
	asm_op->operator_type=CCompiler::ASM_OPERATOR::VGET;
	asm_op->ast_node = _ast;
	asm_op->variable_type = CVariable::OBJECT;
	//printf("[%02i:%02i]\tJT \t[%02i:%02i],[??]\n",m_currentListStatements->size(),ptr_current_statement_op->asm_op.size(),m_currentListStatements->size(),ptr_current_statement_op->asm_op.size()-1);
	ptr_current_statement_op->asm_op.push_back(asm_op);

}


void CCompiler::insert_ClearArgumentStack_Instruction(){
	CCompiler::tInfoStatementOp *ptr_current_statement_op = &(*m_currentListStatements)[m_currentListStatements->size()-1];
	CCompiler::tInfoAsmOp *asm_op = new CCompiler::tInfoAsmOp();
	asm_op->operator_type=CCompiler::ASM_OPERATOR::CLR;
	//printf("[%02i:%02i]\tNOP\n",m_currentListStatements->size(),ptr_current_statement_op->asm_op.size());
	ptr_current_statement_op->asm_op.push_back(asm_op);
}


void CCompiler::insert_PushArgument_Instruction(){
	CCompiler::tInfoStatementOp *ptr_current_statement_op = &(*m_currentListStatements)[m_currentListStatements->size()-1];
	CCompiler::tInfoAsmOp *asm_op = new CCompiler::tInfoAsmOp();
	asm_op->index_op1 = CCompiler::getCurrentInstructionIndex();//&((*m_currentListStatements)[dest_statment]);
	asm_op->operator_type=CCompiler::ASM_OPERATOR::PUSH;
	//printf("[%02i:%02i]\tJT \t[%02i:%02i],[??]\n",m_currentListStatements->size(),ptr_current_statement_op->asm_op.size(),m_currentListStatements->size(),ptr_current_statement_op->asm_op.size()-1);
	ptr_current_statement_op->asm_op.push_back(asm_op);

}

void CCompiler::insert_CallFunction_Instruction(int  index_call){
	CCompiler::tInfoStatementOp *ptr_current_statement_op = &(*m_currentListStatements)[m_currentListStatements->size()-1];
	CCompiler::tInfoAsmOp *asm_op = new CCompiler::tInfoAsmOp();
	asm_op->index_op1 = index_call;//&((*m_currentListStatements)[dest_statment]);
	asm_op->operator_type=CCompiler::ASM_OPERATOR::CALL;
	//printf("[%02i:%02i]\tJT \t[%02i:%02i],[??]\n",m_currentListStatements->size(),ptr_current_statement_op->asm_op.size(),m_currentListStatements->size(),ptr_current_statement_op->asm_op.size()-1);
	ptr_current_statement_op->asm_op.push_back(asm_op);
}

void CCompiler::insertRet(int index){
	CCompiler::tInfoStatementOp *ptr_current_statement_op = &(*m_currentListStatements)[m_currentListStatements->size()-1];
	CCompiler::tInfoAsmOp *asm_op = new CCompiler::tInfoAsmOp();
	asm_op->index_op1 = index;//&((*m_currentListStatements)[dest_statment]);
	//asm_op->symbol_name="";
	asm_op->operator_type=CCompiler::ASM_OPERATOR::RET;

	ptr_current_statement_op->asm_op.push_back(asm_op);
}


void CCompiler::insert_ArrayObject_PushValueInstruction(int ref_vec_object_index, int index_instruciont_to_push){
	CCompiler::tInfoStatementOp *ptr_current_statement_op = &(*m_currentListStatements)[m_currentListStatements->size()-1];
	CCompiler::tInfoAsmOp *asm_op = new CCompiler::tInfoAsmOp();
	asm_op->index_op1=ref_vec_object_index;
	asm_op->index_op2=index_instruciont_to_push;
	if(index_instruciont_to_push == -1){
		asm_op->index_op2=CCompiler::getCurrentInstructionIndex();
	}
	asm_op->operator_type=CCompiler::ASM_OPERATOR::VPUSH;
	ptr_current_statement_op->asm_op.push_back(asm_op);
}
//--------------------------------------------------------------------------------------------------------------------------------------------------------------------

CCompiler::ASM_OPERATOR CCompiler::getIntegerOperatorId_TwoOps(PUNCTUATOR_TYPE op, CVariable::VAR_TYPE & variable_type){

	variable_type = CVariable::INTEGER;

	switch(op){
	default:
		return CCompiler::ASM_OPERATOR::INVALID_OP;
	case PUNCTUATOR_TYPE::ADD_PUNCTUATOR:
		return CCompiler::ADD;
	case PUNCTUATOR_TYPE::DIV_PUNCTUATOR:
		return CCompiler::DIV;
	case PUNCTUATOR_TYPE::MOD_PUNCTUATOR:
		return CCompiler::MOD;
	case PUNCTUATOR_TYPE::MUL_PUNCTUATOR:
		return CCompiler::MUL;
	case PUNCTUATOR_TYPE::BINARY_AND_PUNCTUATOR:
		return CCompiler::AND;
	case PUNCTUATOR_TYPE::BINARY_OR_PUNCTUATOR:
		return CCompiler::OR;
	case PUNCTUATOR_TYPE::BINARY_XOR_PUNCTUATOR:
		return CCompiler::XOR;
	case PUNCTUATOR_TYPE::SHIFT_LEFT_PUNCTUATOR:
		return CCompiler::SHL;
	case PUNCTUATOR_TYPE::SHIFT_RIGHT_PUNCTUATOR:
		return CCompiler::SHR;
	case PUNCTUATOR_TYPE::LOGIC_GTE_PUNCTUATOR:
		variable_type = CVariable::BOOLEAN;
		return CCompiler::GTE;
	case PUNCTUATOR_TYPE::LOGIC_GT_PUNCTUATOR:
		variable_type = CVariable::BOOLEAN;
		return CCompiler::GT;
	case PUNCTUATOR_TYPE::LOGIC_LT_PUNCTUATOR:
		variable_type = CVariable::BOOLEAN;
		return CCompiler::LT;
	case PUNCTUATOR_TYPE::LOGIC_LTE_PUNCTUATOR:
		variable_type = CVariable::BOOLEAN;
		return CCompiler::LTE;
	case PUNCTUATOR_TYPE::LOGIC_EQUAL_PUNCTUATOR:
		variable_type = CVariable::BOOLEAN;
		return CCompiler::EQU;

	}
	return CCompiler::ASM_OPERATOR::INVALID_OP;
}

CCompiler::ASM_OPERATOR CCompiler::getNumberOperatorId_TwoOps(PUNCTUATOR_TYPE op, CVariable::VAR_TYPE & variable_type){

	variable_type = CVariable::NUMBER;

	switch(op){
	default:
		return CCompiler::ASM_OPERATOR::INVALID_OP;
	case PUNCTUATOR_TYPE::ADD_PUNCTUATOR:
		return CCompiler::ADD;
	case PUNCTUATOR_TYPE::DIV_PUNCTUATOR:
		return CCompiler::DIV;
	case PUNCTUATOR_TYPE::MOD_PUNCTUATOR:
		return CCompiler::MOD;
	case PUNCTUATOR_TYPE::MUL_PUNCTUATOR:
		return CCompiler::MUL;
	case PUNCTUATOR_TYPE::LOGIC_GTE_PUNCTUATOR:
		variable_type = CVariable::BOOLEAN;
		return CCompiler::GTE;
	case PUNCTUATOR_TYPE::LOGIC_GT_PUNCTUATOR:
		variable_type = CVariable::BOOLEAN;
		return CCompiler::GT;
	case PUNCTUATOR_TYPE::LOGIC_LT_PUNCTUATOR:
		variable_type = CVariable::BOOLEAN;
		return CCompiler::LT;
	case PUNCTUATOR_TYPE::LOGIC_LTE_PUNCTUATOR:
		variable_type = CVariable::BOOLEAN;
		return CCompiler::LTE;
	case PUNCTUATOR_TYPE::LOGIC_EQUAL_PUNCTUATOR:
		variable_type = CVariable::BOOLEAN;
		return CCompiler::EQU;
	}
	return CCompiler::ASM_OPERATOR::INVALID_OP;
}

CCompiler::ASM_PRE_POST_OPERATORS CCompiler::getNumberOperatorId_OneOp(PUNCTUATOR_TYPE op){

	switch(op){
	default:
		return CCompiler::ASM_PRE_POST_OPERATORS::UNKNOW_PRE_POST_OPERATOR;
	case PUNCTUATOR_TYPE::SUB_PUNCTUATOR:
		return CCompiler::NEG;
	case PUNCTUATOR_TYPE::PRE_INC_PUNCTUATOR:
		return CCompiler::PRE_INC;
	case PUNCTUATOR_TYPE::POST_INC_PUNCTUATOR:
		return CCompiler::POST_INC;
	case PUNCTUATOR_TYPE::PRE_DEC_PUNCTUATOR:
		return CCompiler::PRE_DEC;
	case PUNCTUATOR_TYPE::POST_DEC_PUNCTUATOR:
		return CCompiler::POST_DEC;
	}

	return CCompiler::ASM_PRE_POST_OPERATORS::UNKNOW_PRE_POST_OPERATOR;
}

CCompiler::ASM_OPERATOR CCompiler::getBoleanOperatorId_TwoOps(PUNCTUATOR_TYPE op, CVariable::VAR_TYPE & variable_type){

	variable_type = CVariable::BOOLEAN;

	switch(op){
	default:
		return CCompiler::ASM_OPERATOR::INVALID_OP;
	case PUNCTUATOR_TYPE::LOGIC_AND_PUNCTUATOR:
		return CCompiler::LOGIC_AND;
	case PUNCTUATOR_TYPE::LOGIC_OR_PUNCTUATOR:
		return CCompiler::LOGIC_OR;
	}

	return CCompiler::ASM_OPERATOR::INVALID_OP;
}

CCompiler::ASM_OPERATOR CCompiler::getBoleanOperatorId_OneOp(PUNCTUATOR_TYPE op){
	switch(op){
	default:
		return CCompiler::ASM_OPERATOR::INVALID_OP;
	case PUNCTUATOR_TYPE::LOGIC_NOT_PUNCTUATOR:
		return CCompiler::NOT;
	}
	return CCompiler::ASM_OPERATOR::INVALID_OP;
}

CCompiler::ASM_OPERATOR CCompiler::puntuator2asmop(tInfoPunctuator * op){

	switch(op->id){
	default:
		print_error_cr("%s Not implemented", op->str);
		break;
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
	}

	return INVALID_OP;
}

bool CCompiler::insertOperatorInstruction(tInfoPunctuator * op, PASTNode _node, string & error_str, int op_index_left, int op_index_right){
	CCompiler::tInfoStatementOp *ptr_current_statement_op = &(*m_currentListStatements)[m_currentListStatements->size()-1];
	CCompiler::tInfoAsmOp * left_asm_op = ptr_current_statement_op->asm_op[op_index_left];

	if(op->id == ASSIGN_PUNCTUATOR && left_asm_op->variable_type != CVariable::OBJECT){

			error_str = "left operand must be l-value for '=' operator";
			return false;
	}


	ASM_OPERATOR asm_op;
	if((asm_op= puntuator2asmop(op))!=INVALID_OP){


		CCompiler::tInfoAsmOp *iao = new CCompiler::tInfoAsmOp();
		//asm_op->type_op=OPERATOR;
		iao->operator_type = asm_op;
		iao->index_op1 = op_index_left;
		iao->index_op2 = op_index_right;

		iao->ast_node=_node;


		ptr_current_statement_op->asm_op.push_back(iao);

		return true;
	}
	return true;
}

//------------------------------------------------------------------------------------------------------------------
//
// COMPILE EXPRESSIONS AND GENERATE ITS ASM
//
int CCompiler::gacExpression_ArrayAccess(PASTNode _node, CScope *_lc)
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
				if((gacExpression(array_acces->children [k]->children[0], _lc,CCompiler::getCurrentInstructionIndex()+1)) == -1){
					return -1;
				}

				// insert vector access instruction ...
				insert_ArrayAccess_Instruction(vec,CCompiler::getCurrentInstructionIndex(),array_acces->children [k]);
				vec = CCompiler::getCurrentInstructionIndex();

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
		CCompiler::tInfoStatementOp *ptr_current_statement_op = &(*m_currentListStatements)[m_currentListStatements->size()-1];
		CCompiler::tInfoAsmOp *asm_op = ptr_current_statement_op->asm_op[ptr_current_statement_op->asm_op.size()-1];
		asm_op->pre_post_operator_type=getNumberOperatorId_OneOp(_node->pre_post_operator_info->id);

	}

	// return last instruction where was modified
	return CCompiler::getCurrentInstructionIndex();
}

int CCompiler::gacExpression_ArrayObject_Recursive(PASTNode _node, CScope *_lc){

	return 0;
}

int CCompiler::gacExpression_ArrayObject(PASTNode _node, CScope *_lc)
{
	if(_node == NULL) {print_error_cr("NULL node");return -1;}
	if(_node->node_type != ARRAY_OBJECT_NODE ){print_error_cr("node is not ARRAY_OBJECT_NODE type or null");return -1;}

	int r=0;

	// 1. create object ...
	insert_CreateArrayObject_Instruction();
	int index_created_vec = CCompiler::getCurrentInstructionIndex();

	bool children_is_vector_object;

	// 2. evaluate expressions if any
	for(unsigned j=0; j < _node->children.size(); j++){

		int ini_instruction = -1;

		if(_node->children[j]->node_type == EXPRESSION_NODE){
			children_is_vector_object = _node->children[j]->children[0]->node_type == ARRAY_OBJECT_NODE;
			ini_instruction = getCurrentInstructionIndex()+1;
		}



		// check whether is expression node...
		if((r=gacExpression(_node->children[j], _lc,ini_instruction)) == -1){
			return -1;
		}

		if(index_created_vec==0){
			int yy=0;
			yy++;

		}

		insert_ArrayObject_PushValueInstruction(index_created_vec,ini_instruction);
	}
	return index_created_vec;//CCompiler::getCurrentInstructionIndex();
}

int CCompiler::gacExpression_FunctionObject(PASTNode _node, CScope *_lc)
{
	if(_node == NULL) {print_error_cr("NULL node");return -1;}
	if(_node->node_type != FUNCTION_OBJECT_NODE ){print_error_cr("node is not FUNCTION_OBJECT_NODE type or null");return -1;}
	if(_node->children.size()!=2) {print_error_cr("Array access should have 2 children");return -1;}


	// 1. insert load reference created object ...

	if(!insertLoadValueInstruction(_node, _lc)){
		return -1;
	}

	return CCompiler::getCurrentInstructionIndex();
}

int CCompiler::gacExpression_FunctionAccess(PASTNode _node, CScope *_lc)
{
	if(_node == NULL) {print_error_cr("NULL node");return -1;}
	if(_node->node_type != CALLING_OBJECT_NODE ){print_error_cr("node is not CALLING_OBJECT_NODE type or null");return -1;}
	if(_node->children.size()!=2) {print_error_cr("Array access should have 2 children");return -1;}
	if(_node->children[0]->node_type != FUNCTION_REF_NODE && _node->children[0]->node_type != FUNCTION_OBJECT_NODE){print_error_cr("Node is not FUNCTION_OBJECT_NODE type"); return -1;}
	if(_node->children[1]->node_type != ARGS_PASS_NODE){print_error_cr("Function has no index nodes "); return -1;}
	int r=0;

	insertLoadValueInstruction(_node->children[0],_lc);
	int call_index = getCurrentInstructionIndex();

	// 1. insert push to pass values to all args ...
	PASTNode function_args = _node->children[1];

	// insert clear push arguments stack
	insert_ClearArgumentStack_Instruction();

	for(unsigned k = 0; k < function_args->children.size(); k++){

		// check whether is expression node...
		if((gacExpression(function_args->children[k], _lc,getCurrentInstructionIndex()+1)) == -1){
			return -1;
		}

		// insert vector access instruction ...
		insert_PushArgument_Instruction();
	}

	// 2. insert call instruction itself.
	insert_CallFunction_Instruction(call_index);

	return CCompiler::getCurrentInstructionIndex();
}

int CCompiler::gacExpression_Recursive(PASTNode _node, CScope *_lc, int & index_instruction){

	//CScope * _lc = m_currentScriptFunction->getScope();
	int r=index_instruction;
	bool inline_if_else=false;
	string error_str;
	if(_node==NULL){
		return -1;
	}



	bool special_node =	 _node->node_type == ARRAY_OBJECT_NODE ||
						_node->node_type == FUNCTION_OBJECT_NODE ||
						_node->node_type == CALLING_OBJECT_NODE;

	// TERMINAL SYMBOLS
	if(_node->children.size()==0 ||
			special_node
		)
	{
		bool function_access = false;
		bool array_access = false;

		PASTNode eval_node_sp = _node;
		if(_node->node_type == CALLING_OBJECT_NODE ){
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
						if((r=gacExpression_FunctionObject(_node, _lc)) == -1){
							return -1;
						}
						break;

					default:
						print_error_cr("Unexpected node type %i",eval_node_sp->node_type);
						return -1;
						break;

					}
				}
		}
		else{

			bool var_assign=false;
			if(_node->parent != NULL){
				if(_node->parent->node_type == PUNCTUATOR_NODE){
					if(_node->parent->operator_info->id == ASSIGN_PUNCTUATOR && _node->parent->children[0] == _node){
						var_assign=true;
					}
				}
			}

			//printf("CONST \tE[%i],%s\n",index_instruction,op->value.c_str());
			//if(!var_assign){
				if(!insertLoadValueInstruction(_node, _lc)){
					return -1;

				}
			//}
		}
		index_instruction=r;
		//r=index_instruction;

	}else{

		if(_node->children.size()==3){

			if(_node->operator_info->id == INLINE_IF_PUNCTUATOR && _node->children[2]->operator_info->id == INLINE_ELSE_PUNCTUATOR){
				// node children[0]: conditional.
				// node children[1]: body-if
				// node children[2]: body-else
				inline_if_else = true;
			}

		}

		// check if there's inline-if-else
		int right=0, left=0;

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

		if(left !=-1 && right!=-1){ // 2 ops

			// particular case if operator is =
				//printf("%s\tE[%i],E[%i],E[%i]\n",op->token.c_str(),index_instruction,left,right);
				if(!insertOperatorInstruction(_node->operator_info,_node,error_str,left,right)){
					print_error_cr("%s at line %i",error_str.c_str(),_node->definedValueline);
					return -1;
				}
			//}

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
	index_instruction++;

	return r;
}

bool CCompiler::gacFor(PASTNode _node, CScope * _lc){
	if(_node == NULL) {print_error_cr("NULL node");return false;}
	if(_node->node_type != KEYWORD_NODE || _node->keyword_info == NULL){print_error_cr("node is not keyword type or null");return false;}
	if(_node->keyword_info->id != KEYWORD_TYPE::FOR_KEYWORD){print_error_cr("node is not FOR keyword type");return false;}
	if(_node->children.size()!=4) {print_error_cr("node FOR has not valid number of nodes");return false;}
	if(!(_node->children[0]->node_type==PRE_FOR_NODE && _node->children[1]->node_type==CONDITIONAL_NODE &&
	_node->children[2]->node_type==POST_FOR_NODE && _node->children[3]->node_type==BODY_NODE)) {print_error_cr("node FOR has not valid TYPE nodes");return false;}
	CCompiler::tInfoAsmOp *asm_op;
	//CScope * _currentScope = _node ->scope_ptr;

	// 1. compile var init ...
	if(!ast2asm_Recursive(_node->children[0],_node->scope_ptr)){ return false;}

	// 2. compile conditional

	if(!ast2asm_Recursive(_node->children[1],_node->scope_ptr)){ return false;}
	// get current index statment in order to jmp from end body for.
	int index_statment_conditional_for_= getCurrentStatmentIndex();

	// insert conditional jmp (if not true go to the end)
	asm_op = insert_JNT_Instruction();


	// 3. compile body
	if(!gacBody(_node->children[3],_node->scope_ptr)){ return false;}


	// 4. compile post oper
	if(!ast2asm_Recursive(_node->children[2],_node->scope_ptr)){ return false;}

	// 5. jmp to the conditional index ...
	insert_JMP_Instruction(index_statment_conditional_for_);

	// save jmp value...
	asm_op->index_op1=getCurrentStatmentIndex()+1;

	return true;
}

bool CCompiler::gacWhile(PASTNode _node, CScope * _lc){
	if(_node == NULL) {print_error_cr("NULL node");return false;}
	if(_node->node_type != KEYWORD_NODE || _node->keyword_info == NULL){print_error_cr("node is not keyword type or null");return false;}
	if(_node->keyword_info->id != KEYWORD_TYPE::WHILE_KEYWORD){print_error_cr("node is not WHILE keyword type");return false;}
	if(_node->children.size()!=2) {print_error_cr("node WHILE has not valid number of nodes");return false;}
	if(!(_node->children[0]->node_type==CONDITIONAL_NODE && _node->children[1]->node_type==BODY_NODE )) {print_error_cr("node WHILE has not valid TYPE nodes");return false;}
	CCompiler::tInfoAsmOp *asm_op_jmp_end;
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

bool CCompiler::gacReturn(PASTNode _node, CScope * _lc){

	if(_node == NULL) {print_error_cr("NULL node");return false;}
	if(_node->node_type != KEYWORD_NODE || _node->keyword_info == NULL){print_error_cr("node is not keyword type or null");return false;}
	if(_node->keyword_info->id != KEYWORD_TYPE::RETURN_KEYWORD){print_error_cr("node is not RETURN keyword type");return false;}
	if(_node->children.size() != 1){print_error_cr("node RETURN has not 1 child");return false;}

	if(gacExpression(_node->children[0], _lc)){

		// finally we put mov to Value ...
		insertRet(getCurrentInstructionIndex());
	}
	else {
		return false;
	}

	return true;
}

bool CCompiler::gacFunction(PASTNode _node, CScope * _lc){

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


	// 1. Get the registered symbol.
	CScope::tInfoRegisteredVar * irv=_lc->getInfoRegisteredSymbol(_node->value_symbol,false);
	if(irv == NULL){
		print_error_cr("Cannot get registered function %s",_node->value_symbol.c_str());
		return false;
	}

	// 2. Processing args ...

	// 3. Processing body ...

	// 2. Compiles the function ...
	return ast2asm(_node->children[1], (CScriptFunction *)irv->m_obj);
}

bool CCompiler::gacIf(PASTNode _node, CScope * _lc){
	if(_node == NULL) {print_error_cr("NULL node");return false;}
	if(_node->node_type != KEYWORD_NODE || _node->keyword_info == NULL){print_error_cr("node is not keyword type or null");return false;}
	if(_node->keyword_info->id != KEYWORD_TYPE::IF_KEYWORD){print_error_cr("node is not IF keyword type");return false;}
	if(_node->children.size()<2) {print_error_cr("node IF has not valid number of nodes");return false;}
	if(!(_node->children[0]->node_type==CONDITIONAL_NODE && _node->children[1]->node_type==BODY_NODE )) {print_error_cr("node IF has not valid TYPE nodes");return false;}
	CCompiler::tInfoAsmOp *asm_op_jmp_else_if,*asm_op_jmp_end;

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

bool CCompiler::gacSwitch(PASTNode _node, CScope * _lc){

	if(_node == NULL) {print_error_cr("NULL node");return false;}
	if(_node->node_type != KEYWORD_NODE || _node->keyword_info == NULL){print_error_cr("node is not keyword type or null");return false;}
	if(_node->keyword_info->id != SWITCH_KEYWORD){print_error_cr("node is not SWITCH keyword type");return false;}
	bool has_default = false;
	PASTNode switch_node;
	PASTNode group_cases;
	PASTNode case_value;
	PASTNode case_body;

	CCompiler::tInfoAsmOp * asm_op;

	string error_str;
	string detected_type_str;
	CScope *_scope = this->m_currentScriptFunction->getScope()->getCurrentScopePointer();

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
									asm_op = (CCompiler::tInfoAsmOp *)case_value->aux_value; // load jt instruction and set current instruction before write asm code.
									asm_op->index_op1 = getCurrentStatmentIndex();
								}

								if(i < (_node->children.size()-1))
									case_body->aux_value = insert_JMP_Instruction();
							}else{
								return false;
							}
							break;

						case 2: // FINALLY, WRITE JMP's to end statment

							if((asm_op = (CCompiler::tInfoAsmOp *)case_body->aux_value) != NULL){
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

bool CCompiler::gacVar(PASTNode _node, CScope * _lc){
	if(_node == NULL) {print_error_cr("NULL node");return false;}
	if(_node->node_type != KEYWORD_NODE || _node->keyword_info == NULL){print_error_cr("node is not keyword type or null");return false;}
	if(_node->keyword_info->id != VAR_KEYWORD){print_error_cr("node is not VAR keyword type");return false;}

	for(unsigned i = 0 ; i < _node->children.size(); i++){ // init all requested vars...
		if(!gacExpression(_node->children[i], _lc))
			return false;
	}

	return true;
}

bool CCompiler::gacKeyword(PASTNode _node, CScope * _lc){

	if(_node == NULL) {print_error_cr("NULL node");return false;}
	if(_node->node_type != KEYWORD_NODE || _node->keyword_info == NULL){print_error_cr("node is not keyword type or null");return false;}

	switch(_node->keyword_info->id){
	default:
		print_error_cr("Keyword [ %s ] not implemented yet!",_node->keyword_info->str);
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
	//	return gacFunction(_node, _lc);
		return true;
		break;
	case KEYWORD_TYPE::RETURN_KEYWORD:
		return gacReturn(_node, _lc);
		break;

	}

	return false;
}

bool CCompiler::gacBody(PASTNode _node, CScope * _lc){
	if(_node == NULL) {print_error_cr("NULL node");return false;}
	if(_node->node_type != BODY_NODE ){print_error_cr("node is not BODY type or null");return false;}

	if(_node->children.size() == 1){ // body has code ...

		bool ok = ast2asm_Recursive(_node->children[0], _lc);

		return ok;
	}else{ // insert one statment at least ..
		newStatment();
	}

	return true;
}

bool CCompiler::gacExpression(PASTNode _node, CScope *_lc,int index_instruction){

	if(index_instruction == -1){ // create new statment
		//int index_instruction=0;
		CCompiler::tInfoStatementOp i_stat;
		(*m_currentListStatements).push_back(i_stat);
		index_instruction = 0; // set as 0
	}

	if(_node == NULL) {print_error_cr("NULL node");return false;}
	if(_node->node_type != EXPRESSION_NODE){print_error_cr("node is not Expression");return false;}

	return gacExpression_Recursive(_node->children[0], _lc,index_instruction) != -1;
}

bool CCompiler::ast2asm_Recursive(PASTNode _node, CScope *_lc){

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
				return gacBody(_node, _lc);
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

bool CCompiler::ast2asm(PASTNode _node, CScriptFunction *sf){

	if(_node == NULL){
		print_error_cr("NULL node!");
		return false;
	}


	if(_node->node_type == NODE_TYPE::BODY_NODE ){
		CScriptFunction *aux_sf = m_currentScriptFunction;
		stk_scriptFunction.push_back(m_currentScriptFunction);

		this->m_currentScriptFunction = sf;
		this->m_currentListStatements = sf->getCompiledCode();
		this->m_treescope = sf->getScope();

		// reset current pointer ...
		m_treescope->resetScopePointer();

		{ // main node ?
			for(unsigned i = 0; i < _node->children.size(); i++){

				if(!ast2asm_Recursive(_node->children[i], m_treescope->getCurrentScopePointer())){
					return false;
				}
			}
		}

		stk_scriptFunction.pop_back();
		m_currentScriptFunction = aux_sf;

		if(m_currentScriptFunction != NULL){
			this->m_currentListStatements = m_currentScriptFunction->getCompiledCode();
			this->m_treescope = m_currentScriptFunction->getScope();
		}

		// ok parse all function
		// and then print its functions ...
		vector<CScriptFunction *> * m_vf = sf->getVectorFunction();

		for(unsigned j =0; j < m_vf->size(); j++){

			if(m_vf->at(j)->getType() == CScriptFunction::TYPE::SCRIPT_FUNCTION_TYPE){

				if(!gacFunction(m_vf->at(j)->getRootAst(),sf->getScope())){
					return false;
				}
			}
		}

		return true;
	}
	else{
		print_error_cr("Main node expected");
	}

	return false;
}

bool CCompiler::compile(const string & s, CScriptFunction * sf){

	// generate whole AST

	if(CAst::generateAST(s.c_str(),sf, sf->getRootAstPtr())){

		if(ast2asm(sf->getRootAst(),sf)){
			// print generated asm ...
			CCompiler::printGeneratedCode(sf);
			return true;
		}
		// then you have all information -> compile into asm!
		//generateAsmCode(root);
	}

	return false;
}

CCompiler::~CCompiler(){

}
