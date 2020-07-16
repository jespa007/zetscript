/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
#include "zetscript.h"

#define FORMAT_PRINT_INSTRUCTION "%04i"

namespace zetscript{

	std::string ScriptFunction::formatInstructionLoadType(ScriptFunction *current_function,PtrInstruction list_statements, int current_instruction){

		 char print_aux_load_value[512] = {0};
		 char object_access[512] = "";

		 Instruction * instruction =&list_statements[current_instruction];
		 ConstantValue *icv;
		 std::string symbol_value=SFI_GET_SYMBOL_NAME(current_function,instruction);
		 if(instruction->byte_code != BYTE_CODE_LOAD){
			 return "ERROR";
		 }

		 sprintf(print_aux_load_value,"UNDEFINED");

		 if(instruction->properties & MSK_INSTRUCTION_PROPERTY_SCOPE_TYPE_ACCESS){

			 sprintf(object_access,
					"[" FORMAT_PRINT_INSTRUCTION "]."

					,(int)instruction->value_op2);
		 }
		 else if(instruction->properties & MSK_INSTRUCTION_PROPERTY_SCOPE_TYPE_THIS){
			sprintf(object_access,"this.");
		 }

		 switch(instruction->value_op1){

			case LoadType::LOAD_TYPE_CONSTANT:
				icv=(ConstantValue *)instruction->value_op2;
				switch(icv->properties){
				case MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_BOOLEAN:
				case MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_INTEGER:
					sprintf(print_aux_load_value,"CONST %i",(int)((intptr_t)icv->stk_value));
					break;
				case MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_FLOAT:
					sprintf(print_aux_load_value,"CONST %f",*((float *)&icv->stk_value));
					break;
				case MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_STRING:
					sprintf(print_aux_load_value,"CONST \"%s\"",((const char *)icv->stk_value));
					break;

				}
				break;

			case LoadType::LOAD_TYPE_VARIABLE:
				sprintf(print_aux_load_value,"%sVAR   %s",object_access,symbol_value.c_str());
				break;
			case LoadType::LOAD_TYPE_FUNCTION:

				sprintf(print_aux_load_value,"%sFUN   %s",object_access,symbol_value.c_str());
				break;

			case LoadType::LOAD_TYPE_ARGUMENT:
				sprintf(print_aux_load_value,"ARG   %s",symbol_value.c_str());
				break;
			default:

				break;
		}
		return print_aux_load_value;
	 }

	 void ScriptFunction::printGeneratedCode(ScriptFunction *sfo){

		// PRE: it should printed after compile and updateReferences.
		std::string pre="";
		std::string post="";

		// first print functions  ...
		std::vector<ScriptFunction *> * m_vf = &sfo->local_function;

		for(unsigned j =0; j < m_vf->size(); j++){

			ScriptFunction *local_irfs = (*m_vf)[j];

			if(( local_irfs->symbol_info.symbol_info_properties & SYMBOL_INFO_PROPERTY_C_OBJECT_REF) != SYMBOL_INFO_PROPERTY_C_OBJECT_REF){
				printGeneratedCode(m_vf->at(j));
			}
		}

		if(sfo->symbol_info.symbol_info_properties & SYMBOL_INFO_PROPERTY_C_OBJECT_REF){ // c functions has no script instructions
			return;
		}

		std::string symbol_ref="????";

		//strcpy(symbol_ref,AST_SYMBOL_VALUEZJ_CONST_CHAR(local_irfs->symbol_info.idxAstNode));

		if(sfo->idx_class!=ZS_INVALID_CLASS){
			ScriptClass *sc = GET_SCRIPT_CLASS(sfo,sfo->idx_class);
			if(sc->idx_class == IDX_BUILTIN_TYPE_CLASS_MAIN){
				symbol_ref=sfo->symbol_info.symbol->name;
			}else{
				symbol_ref=sfo->symbol_info.symbol->name+std::string("::")+std::string("????");
			}
		}


		printf("-------------------------------------------------------\n");
		printf("\nCode for function \"%s\"\n\n",symbol_ref.c_str());


		/*if(sfo->instruction==NULL){
			ZS_PRINT_WARNING("No instructions for function \"%s\"",sfo->symbol_info.symbol->name);
			return;
		}*/

		unsigned idx_instruction=0;
		for(Instruction * instruction=sfo->instruction; instruction->byte_code!= BYTE_CODE_END_FUNCTION; instruction++,idx_instruction++){

			int n_ops=0;
			int value_op1 = instruction->value_op1;
			int value_op2 = instruction->value_op2;

			if(value_op1 != -1)
				n_ops++;

			 if(value_op2 != -1)
				 n_ops++;

			 pre="";
			 post="";

				switch(GET_MSK_INSTRUCTION_PROPERTY_PRE_POST_OP(instruction->properties)){
				case MSK_INSTRUCTION_PROPERTY_PRE_NOT:
					pre="-";
					break;
				case MSK_INSTRUCTION_PROPERTY_PRE_INC:
					pre="++";
					break;
				case MSK_INSTRUCTION_PROPERTY_PRE_DEC:
					pre="--";
					break;
				case MSK_INSTRUCTION_PROPERTY_POST_INC:
					post="++";
					break;
				case MSK_INSTRUCTION_PROPERTY_POST_DEC:
					post="--";
					break;
				default:
					// check whether is constant and numeric
					if(instruction->byte_code==ByteCode::BYTE_CODE_LOAD && instruction->value_op1==LOAD_TYPE_CONSTANT){
						ConstantValue *icv = (((ConstantValue *)instruction->value_op2));
						float n;

						// change the sign
						switch(icv->properties){
						default:
							break;
						case MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_INTEGER:
							if(((intptr_t)icv->stk_value)<0){
								pre="-";
							}
							break;
						case MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_FLOAT:
							memcpy(&n,&icv->stk_value,sizeof(float));
							if(n<0){
								pre="-";
							}
							break;
						}
					}
					break;

				}
			switch(instruction->byte_code){

			case  BYTE_CODE_NEW:
				printf("[" FORMAT_PRINT_INSTRUCTION "]\t%s\t%s\n"
						,idx_instruction
						,ByteCodeToStr(instruction->byte_code)
						,instruction->value_op1!=ZS_INVALID_CLASS?GET_SCRIPT_CLASS_NAME(sfo,instruction->value_op1):"???");
				break;
			case  BYTE_CODE_LOAD:
				printf("[" FORMAT_PRINT_INSTRUCTION "]\t%s\t%s%s%s\n"
						,idx_instruction,
						ByteCodeToStr(instruction->byte_code),
						pre.c_str(),
						formatInstructionLoadType(sfo,sfo->instruction,idx_instruction).c_str(),
						post.c_str());
				break;
			case BYTE_CODE_JNT:
			case BYTE_CODE_JT:
			case BYTE_CODE_JMP:
				printf("[" FORMAT_PRINT_INSTRUCTION "]\t%s\t%03i\n"
						,idx_instruction
						,ByteCodeToStr(instruction->byte_code)
						,(int)instruction->value_op2);
				break;
			case BYTE_CODE_PUSH_SCOPE:
				printf("[" FORMAT_PRINT_INSTRUCTION "]\t%s%c%s%s%s%c\n"
						,idx_instruction
						,ByteCodeToStr(instruction->byte_code)
						,instruction->value_op1!=0?'(':' '
						,instruction->value_op1&ScopeProperty::SCOPE_PROPERTY_BREAK?"BREAK":""
						,instruction->value_op1&ScopeProperty::SCOPE_PROPERTY_CONTINUE?" CONTINUE":""
						,instruction->value_op1&ScopeProperty::SCOPE_PROPERTY_FOR_IN?" FOR_IN":""
						,instruction->value_op1!=0?')':' '
						);
				break;
			case BYTE_CODE_POP_SCOPE:
				printf("[" FORMAT_PRINT_INSTRUCTION "]\t%s%c%s%s%s%c\n"
						,idx_instruction
						,ByteCodeToStr(instruction->byte_code)
						,instruction->value_op1!=0?'(':' '
						,instruction->value_op1&ScopeProperty::SCOPE_PROPERTY_BREAK?"BREAK":""
						,instruction->value_op1&ScopeProperty::SCOPE_PROPERTY_CONTINUE?" CONTINUE":""
						,instruction->value_op1&ScopeProperty::SCOPE_PROPERTY_FOR_IN?" FOR_IN":""
						,instruction->value_op1!=0?')':' '
						);
				break;
			default:

				if(n_ops==0){
					printf("[" FORMAT_PRINT_INSTRUCTION "]\t%s\n",idx_instruction,ByteCodeToStr(instruction->byte_code));
				}else if(n_ops==1){
					printf("[" FORMAT_PRINT_INSTRUCTION "]\t%s%s\n"
							,idx_instruction
							,ByteCodeToStr(instruction->byte_code)
							,(instruction->properties & MSK_STACK_ELEMENT_PROPERTY_POP_ONE)?"_CS":""
							);
				}else{
					printf("[" FORMAT_PRINT_INSTRUCTION "]\t%s\n"
							,idx_instruction
							,ByteCodeToStr(instruction->byte_code)
							);
				}
				break;
			}
		}



	 }


	void ScriptFunction::linkScopeBlockVars(){

		 struct ScopeBlockVarsRegisterHelper{
			 int idx_scope;
			 std::vector<int> idx_local_var;
		 };

		if(symbol_info.symbol->idx_scope < 0){ // just ignore. it could be undefined or C
			return;
		}

		if(local_variable.size() == 0){ // no elements, return...
			return;
		}

		if(scope_block_vars != NULL){ // free if already allocated.
			free(scope_block_vars);
			scope_block_vars=NULL;
			n_scope_block_vars=0;
		}

		/// PRE: base_class_irfs must be info of root class.
		 //bool is_main_function = symbol_info.symbol->idx_scope == IDX_GLOBAL_SCOPE;

		 //std::vector<Scope *> *list = scope_factory->getScopes();
		 std::vector<ScopeBlockVarsRegisterHelper> vec_ivsb;
		 std::map<short,ScopeBlockVarsRegisterHelper> map_scope_register;

		 for(unsigned idx_local_var = 0;idx_local_var < local_variable.size(); idx_local_var++){ // register index var per scope ...

			map_scope_register[local_variable[idx_local_var].symbol->idx_scope].idx_scope=local_variable[idx_local_var].symbol->idx_scope;
			map_scope_register[local_variable[idx_local_var].symbol->idx_scope].idx_local_var.push_back(idx_local_var);
		 }


		 scope_block_vars = (ScopeBlockVars*)malloc(map_scope_register.size()*sizeof(ScopeBlockVars));
		 memset(scope_block_vars,0,map_scope_register.size()*sizeof(ScopeBlockVars));
		 n_scope_block_vars =map_scope_register.size();

		 int i=0;
		 for(std::map<short,ScopeBlockVarsRegisterHelper>::iterator e = map_scope_register.begin(); e != map_scope_register.end(); e++){

			 ScopeBlockVarsRegisterHelper ivs = map_scope_register[e->first];

			 scope_block_vars[i].idx_scope = ivs.idx_scope;
			 scope_block_vars[i].n_local_vars = (char)ivs.idx_local_var.size();
			 scope_block_vars[i].idx_local_var = (int *)malloc(sizeof(int)*ivs.idx_local_var.size());
			 for(unsigned j = 0; j < ivs.idx_local_var.size(); j++){
				 scope_block_vars[i].idx_local_var[j] = ivs.idx_local_var[j];
			 }
			 i++;
		 }

	}


	ScriptFunction::ScriptFunction(ZetScript * _zs,unsigned char _idxClass):ScriptContext(_zs,_idxClass){
		idx_return_type = ZS_IDX_UNDEFINED;
		idx_script_function = ZS_IDX_UNDEFINED;
		instruction=NULL;
		scope_block_vars=NULL;
		n_scope_block_vars=0;
		//idx_local_function=ZS_IDX_UNDEFINED;

	}


	short 		 ScriptFunction::getInstructionLine(Instruction * ins){
		InstructionSourceInfo *info=getInstructionInfo(ins);
		if(info!=NULL){
			return info->line;
		}
		return -1;
	}

	const char *  ScriptFunction::getInstructionSymbolName(Instruction * ins){
		InstructionSourceInfo *info=getInstructionInfo(ins);

		if(info!=NULL){
			return info->str_symbol->c_str();
		}
		return "unknown";
	}

	const char * ScriptFunction::getInstructionSourceFile(Instruction * ins){

		InstructionSourceInfo *info=getInstructionInfo(ins);

		if(info!=NULL){
			return info->file;
		}
		return "unknown";
	}

	int ScriptFunction::existArgumentName(const std::string & arg_name){
		int idx_arg=ZS_IDX_UNDEFINED;

		for(unsigned i = 0; i < this->arg_info.size() && idx_arg == ZS_IDX_UNDEFINED; i++){

			if(this->arg_info[i].arg_name == arg_name){
				idx_arg=i;
			}
		}

		return idx_arg;

	}


	SymbolInfo *	ScriptFunction::registerVariable(const std::string & file, short line, const std::string & variable_name, const std::string & c_type, intptr_t ref_ptr, unsigned short symbol_info_properties)
	{
		SymbolInfo *vsi=ScriptContext::registerVariable(file,line,this->symbol_info.symbol->idx_scope,  variable_name,  c_type,  ref_ptr,   symbol_info_properties);

		StackElement se = {0,0,MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_UNDEFINED};

		if(symbol_info_properties &  SYMBOL_INFO_PROPERTY_C_OBJECT_REF) // convert c ref var into stack element. This should be consistent in the whole execution.
			se=symbolInfoToStackElement(
				this->zs,
			 	vsi,
				(void *)vsi->ref_ptr
			);

		virtual_machine->addGlobalVar(se);

		return vsi;

	}

	ScriptFunction::~ScriptFunction(){

	}

}
