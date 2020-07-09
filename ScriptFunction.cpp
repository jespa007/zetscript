/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
#include "ZetScript.h"

namespace zetscript{

	 void ScriptFunction::PrintGeneratedCode(ScriptFunction *sfo){

		// PRE: it should printed after compile and updateReferences.
		std::string pre="";
		std::string post="";

		unsigned idx_instruction=0;
		for(Instruction * instruction=sfo->instruction; instruction->byte_code_op!= BYTE_CODE_END_FUNCTION; instruction++,idx_instruction++){

			int n_ops=0;
			int op1_value = instruction->op1_value;
			int op2_value = instruction->op2_value;

			if(op1_value != -1)
				n_ops++;

			 if(op2_value != -1)
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
					if(instruction->byte_code_op==ByteCode::BYTE_CODE_LOAD && instruction->op1_value==LOAD_TYPE_CONSTANT){
						ConstantValueInfo *icv = (((ConstantValueInfo *)instruction->op2_value));
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
			switch(instruction->byte_code_op){

			case  BYTE_CODE_NEW:
				printf("[" FORMAT_PRINT_INSTRUCTION "]\t%s\t%s\n",idx_instruction,Instruction::ByteCodeOpToStr(instruction->byte_code_op),instruction->op1_value!=ZS_INVALID_CLASS?GET_SCRIPT_CLASS_NAME(instruction->op1_value):"???");
				break;
			case  BYTE_CODE_LOAD:
				printf("[" FORMAT_PRINT_INSTRUCTION "]\t%s\t%s%s%s\n"
						,idx_instruction,
						Instruction::ByteCodeOpToStr(instruction->byte_code_op),
						pre.c_str(),
						getStrTypeLoadValue(sfo,sfo->instruction,idx_instruction).c_str(),
						post.c_str());
				break;
			case BYTE_CODE_JNT:
			case BYTE_CODE_JT:
			case BYTE_CODE_JMP:
				printf("[" FORMAT_PRINT_INSTRUCTION "]\t%s\t%03i\n"
						,idx_instruction
						,Instruction::ByteCodeOpToStr(instruction->byte_code_op)
						,(int)instruction->op2_value);
				break;
			case BYTE_CODE_PUSH_SCOPE:
				printf("[" FORMAT_PRINT_INSTRUCTION "]\t%s%c%s%s%s%c\n"
						,idx_instruction
						,Instruction::ByteCodeOpToStr(instruction->byte_code_op)
						,instruction->op1_value!=0?'(':' '
						,instruction->op1_value&ScopeProperty::BREAK?"BREAK":""
						,instruction->op1_value&ScopeProperty::CONTINUE?" CONTINUE":""
						,instruction->op1_value&ScopeProperty::FOR_IN?" FOR_IN":""
						,instruction->op1_value!=0?')':' '
						);
				break;
			case BYTE_CODE_POP_SCOPE:
				printf("[" FORMAT_PRINT_INSTRUCTION "]\t%s%c%s%s%s%c\n"
						,idx_instruction
						,Instruction::ByteCodeOpToStr(instruction->byte_code_op)
						,instruction->op1_value!=0?'(':' '
						,instruction->op1_value&ScopeProperty::BREAK?"BREAK":""
						,instruction->op1_value&ScopeProperty::CONTINUE?" CONTINUE":""
						,instruction->op1_value&ScopeProperty::FOR_IN?" FOR_IN":""
						,instruction->op1_value!=0?')':' '
						);
				break;
			default:

				if(n_ops==0){
					printf("[" FORMAT_PRINT_INSTRUCTION "]\t%s\n",idx_instruction,Instruction::ByteCodeOpToStr(instruction->byte_code_op));
				}else if(n_ops==1){
					printf("[" FORMAT_PRINT_INSTRUCTION "]\t%s%s\n"
							,idx_instruction
							,Instruction::ByteCodeOpToStr(instruction->byte_code_op)
							,(instruction->properties & MSK_STACK_ELEMENT_PROPERTY_POP_ONE)?"_CS":""
							);
				}else{
					printf("[" FORMAT_PRINT_INSTRUCTION "]\t%s\n"
							,idx_instruction
							,Instruction::ByteCodeOpToStr(instruction->byte_code_op)
							);
				}
				break;
			}
		}


		// and then print its functions ...
		std::vector<ScriptFunction *> * m_vf = &sfo->local_function;

		for(unsigned j =0; j < m_vf->size(); j++){

			ScriptFunction *local_irfs = (*m_vf)[j];

			if(( local_irfs->symbol_info.symbol_info_properties & SYMBOL_INFO_PROPERTY_C_OBJECT_REF) != SYMBOL_INFO_PROPERTY_C_OBJECT_REF){
				std::string symbol_ref="????";


				//strcpy(symbol_ref,AST_SYMBOL_VALUEZJ_CONST_CHAR(local_irfs->symbol_info.idxAstNode));

				if(local_irfs->idx_class!=ZS_INVALID_CLASS){
					ScriptClass *sc = GET_SCRIPT_CLASS(local_irfs->idx_class);
					if(sc->idx_class == IDX_BUILTIN_TYPE_CLASS_MAIN){
						symbol_ref="Main";
					}else{
						symbol_ref=sfo->symbol_info.symbol->name+std::string("::")+std::string("????");
					}
				}

				printf("-------------------------------------------------------\n");
				printf("\nCode for function \"%s\"\n\n",symbol_ref.c_str());
				PrintGeneratedCode(m_vf->at(j));
			}
		}
	 }


	void ScriptFunction::BuildLutScopeSymbols(){

		if(symbol_info.symbol->idx_scope < 0){ // it could be undefined or C
			return;
		}

		if(local_variable.size() == 0){ // no elements...
			return;
		}

		if(lut_scope_symbol != NULL){ // free if already allocated.
			free(lut_scope_symbol);
			lut_scope_symbol=NULL;
			n_lut_scope_symbols=0;
		}

		/// PRE: base_class_irfs must be info of root class.
		 //bool is_main_function = symbol_info.symbol->idx_scope == IDX_GLOBAL_SCOPE;


		 struct tInfoVarScopeBlockRegister{
			 int idx_scope;
			 std::vector<int> var_index;
		 };

		 //std::vector<Scope *> *list = scope_factory->getVectorScopeNode();
		 std::vector<tInfoVarScopeBlockRegister> vec_ivsb;
		 std::map<short,tInfoVarScopeBlockRegister> map_scope_register;

		 for(unsigned idx_var = 0;idx_var < local_variable.size(); idx_var++){ // register index var per scope ...

			map_scope_register[local_variable[idx_var].symbol->idx_scope].idx_scope=local_variable[idx_var].symbol->idx_scope;
			map_scope_register[local_variable[idx_var].symbol->idx_scope].var_index.push_back(idx_var);
		 }


		 lut_scope_symbol = (ScopeVarInnerBlockInfo*)malloc(map_scope_register.size()*sizeof(ScopeVarInnerBlockInfo));
		 n_lut_scope_symbols =map_scope_register.size();

		 int i=0;
		 for(std::map<short,tInfoVarScopeBlockRegister>::iterator e = map_scope_register.begin(); e != map_scope_register.end(); e++){

			 tInfoVarScopeBlockRegister ivs = map_scope_register[e->first];

			 lut_scope_symbol[i].idx_scope = ivs.idx_scope;
			 lut_scope_symbol[i].n_var_index = (char)ivs.var_index.size();
			 lut_scope_symbol[i].var_index = (int *)malloc(sizeof(int)*ivs.var_index.size());
			 for(unsigned j = 0; j < ivs.var_index.size(); j++){
				 lut_scope_symbol[i].var_index[j] = ivs.var_index[j];
			 }
			 i++;
		 }

	}


	ScriptFunction::ScriptFunction(ZetScript * _zs,unsigned char _idxClass):ScriptClassBase(_zs,_idxClass){
		idx_return_type = ZS_UNDEFINED_IDX;
		idx_script_function = ZS_UNDEFINED_IDX;
		instruction=NULL;
		lut_scope_symbol=NULL;
		n_lut_scope_symbols=0;
		idx_local_function=ZS_UNDEFINED_IDX;

	}


	short 		 ScriptFunction::GetInstructionLine(Instruction * ins){
		OpCodeInstructionSourceInfo *info=GetInstructionInfo(ins);
		if(info!=NULL){
			return info->line;
		}
		return -1;
	}

	const char *  ScriptFunction::GetInstructionSymbolName(Instruction * ins){
		OpCodeInstructionSourceInfo *info=GetInstructionInfo(ins);

		if(info!=NULL){
			return info->str_symbol->c_str();
		}
		return "unknown";
	}

	const char * ScriptFunction::GetInstructionSourceFile(Instruction * ins){

		OpCodeInstructionSourceInfo *info=GetInstructionInfo(ins);

		if(info!=NULL){
			return info->file;
		}
		return "unknown";
	}


	SymbolInfo *	ScriptFunction::RegisterVariable(const std::string & file, short line, const std::string & variable_name, const std::string & c_type, intptr_t ref_ptr, unsigned short symbol_info_properties)
	{
		SymbolInfo *vsi=ScriptClassBase::RegisterVariable(file,line,this->symbol_info.symbol->idx_scope,  variable_name,  c_type,  ref_ptr,   symbol_info_properties);

		StackElement se = {0,0,MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_UNDEFINED};

		if(symbol_info_properties &  SYMBOL_INFO_PROPERTY_C_OBJECT_REF) // convert c ref var into stack element. This should be consistent in the whole execution.
			se=zs->C_REF_InfoVariable_2_StackElement(
										 	vsi,
											(void *)vsi->ref_ptr);

		virtual_machine->AddGlobalVar(se);

		return vsi;

	}

	ScriptFunction::~ScriptFunction(){

	}

}
