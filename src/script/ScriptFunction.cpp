/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
#include "zetscript.h"

#define FORMAT_PRINT_INSTRUCTION "%04i|%2i|%02i"

#define GET_ILOAD_ACCESS_TYPE_STR(properties) \
((properties) & INSTRUCTION_PROPERTY_ILOAD_R_ACCESS_THIS_VAR) ? "This"\
:"Local"\

#define GET_ILOAD_R_STR(properties,value) \
	((properties) & INSTRUCTION_PROPERTY_ILOAD_R_ACCESS_THIS_VAR) ? ((Symbol *)sc->scope_script_type->symbol_variables->items[value])->name.c_str()\
	:((Symbol *)sfo->local_variables->items[value])->name.c_str()\

namespace zetscript{

	ScriptFunction::ScriptFunction(
			ZetScript * _zs
			,int _idx_script_function
			,int _idx_script_type_owner
			,int _idx_position
			,const zs_string & _function_name
			, ScriptFunctionParam **_params
			, int _params_len
			,int _idx_return_type
			, zs_int _ref_native_function_ptr
			,uint16_t _properties
		) {

		// function data...
		idx_script_type_owner=_idx_script_type_owner;
		name_script_function=_function_name;
		idx_script_function=_idx_script_function;
		idx_position = _idx_position;
		idx_script_type_return = _idx_return_type;
		ref_native_function_ptr=_ref_native_function_ptr;
		properties = _properties;
		scope_script_function = NULL;
		instructions=NULL;
		instructions_len = 0;


		// local symbols for type or function...
		local_variables=new zs_vector();
		params = NULL;//new zs_vector();
		params_len = 0;

		updateParams(_params,_params_len);

		// factories
		zs = _zs;
		scope_factory = zs->getScopeFactory();
		script_function_factory= zs->getScriptFunctionFactory();
		script_type_factory=zs->getScriptTypeFactory();

		min_code_stack_needed=0;

	}

	InstructionSourceInfo * ScriptFunction::getInstructionInfo(Instruction *instruction){
		short idx= (instruction-this->instructions);///sizeof(Instruction *);
		//if(instruction_source_info.items[idx]==1){
		return (InstructionSourceInfo *)instruction_source_info.items[idx];
	}

	 void ScriptFunction::printGeneratedCode(ScriptFunction *sfo,ScriptType *sc){

		// PRE: it should printed after compile and updateReferences.
		// first print functions  ...
		ZetScript *zs=sfo->zs;
		int sum_stk_load_stk=0;
		int max_acc_stk_load=0;
		int req_stk;


		if(sfo->properties & FUNCTION_PROPERTY_C_OBJECT_REF){ // c functions has no script instructions
			return;
		}

		if(sfo->instructions == NULL){
			return;
		}

		if(sfo->instructions->byte_code== BYTE_CODE_END_FUNCTION){
			return;
		}

		zs_string symbol_ref="????";
		zs_string class_str="";
		zs_string symbol_value="";
		zs_string iload_info="";

		if(sc==NULL){ // no type is a function on a global scope
			symbol_ref=sfo->name_script_function;
		}else{ // is a type
			symbol_ref=sfo->name_script_function;//+zs_string("::")+zs_string("????");
			class_str=sc->str_script_type+"::";
		}



		printf("______________________________________________________________\n\n");
		printf(" Function: '%s%s'                                             \n",class_str.c_str(),symbol_ref.c_str());
		printf(" Stack code: %i                                        		  \n",sfo->min_code_stack_needed);
		printf(" Stack local vars: %i                                         \n",sfo->local_variables->count);
		printf(" Total stack required: %i                                     \n\n",sfo->local_variables->count+sfo->min_code_stack_needed);
		printf(" Scopes: %i                                                   \n",sfo->scope_script_function->scopes->count);

		printf(" NUM |RS|AS|               INSTRUCTION                        \n");
		printf("-----+--+--+--------------------------------------------------\n");

		for(Instruction * instruction=sfo->instructions; instruction->byte_code!= BYTE_CODE_END_FUNCTION; instruction++){

			int n_ops=0;
			unsigned char value_op1 = instruction->value_op1;
			int value_op2 = instruction->value_op2;
			symbol_value=SFI_GET_SYMBOL_NAME(sfo,instruction);

			//Instruction *instruction_aux=NULL;
			iload_info="";
			unsigned idx_instruction=instruction-sfo->instructions;

			if((char)value_op1 != ZS_IDX_UNDEFINED){
				n_ops++;
			}

			 if(value_op2 != ZS_IDX_UNDEFINED){
				 n_ops++;
			 }

			 if((
					   instruction->byte_code==ByteCode::BYTE_CODE_LOAD_THIS_VARIABLE
					|| instruction->byte_code==ByteCode::BYTE_CODE_LOAD_THIS_FUNCTION
					|| instruction->byte_code==ByteCode::BYTE_CODE_THIS_CALL
					|| instruction->byte_code==ByteCode::BYTE_CODE_PUSH_STK_THIS_VARIABLE
			)){

				// ... we create new instruction
				 symbol_value="this."+symbol_value;
			 }else if(   instruction->byte_code==ByteCode:: BYTE_CODE_INDIRECT_GLOBAL_CALL
					 || instruction->byte_code== BYTE_CODE_INDIRECT_LOCAL_CALL

			 ){
				 symbol_value="@"+symbol_value;
			 }else if(instruction->byte_code== BYTE_CODE_INDIRECT_THIS_CALL){
				 symbol_value="@this."+symbol_value;
			 }


			 req_stk=instruction_num_required_stack(instruction);
			 sum_stk_load_stk+=req_stk;
			 max_acc_stk_load=ZS_MAX(max_acc_stk_load,sum_stk_load_stk);
			 if(instruction->byte_code == BYTE_CODE_RESET_STACK
				|| instruction->byte_code == BYTE_CODE_RET
				|| (instruction->properties & INSTRUCTION_PROPERTY_RESET_STACK)
			 ){
				// <-- reset stack
				sum_stk_load_stk=0; // and reset stack
			 }

			switch(instruction->properties & INSTRUCTION_PROPERTY_ILOAD){
			default:
				break;
			case INSTRUCTION_PROPERTY_ILOAD_K: /* only perfom with one constant*/\
				 iload_info=zs_strutils::format("%s", instruction->getConstantValueOp2ToString().c_str());
				 break;
			case INSTRUCTION_PROPERTY_ILOAD_R: /* only perfom with one Register */\
				 iload_info=zs_strutils::format("%s['%s']"
					 ,GET_ILOAD_ACCESS_TYPE_STR(instruction->properties)
					 ,GET_ILOAD_R_STR(instruction->properties,instruction->value_op1)
				 );
				 break;
			case INSTRUCTION_PROPERTY_ILOAD_KR: /* perfom Konstant-Register*/\
			 	 iload_info=zs_strutils::format("%s,%s['%s']"
					 ,instruction->getConstantValueOp2ToString().c_str()
					 ,GET_ILOAD_ACCESS_TYPE_STR(instruction->properties)
					 ,GET_ILOAD_R_STR(instruction->properties,instruction->value_op1)
				 );
				break;
			case INSTRUCTION_PROPERTY_ILOAD_RK: /* perfom Register-Konstant */\
				 iload_info=zs_strutils::format("%s['%s'],%s"
					 ,GET_ILOAD_ACCESS_TYPE_STR(instruction->properties)
					 ,GET_ILOAD_R_STR(instruction->properties,instruction->value_op1)
					 ,instruction->getConstantValueOp2ToString().c_str()
				 );
				break;
		   case INSTRUCTION_PROPERTY_ILOAD_RR: /* perfom Register-Register*/ \
		   	   iload_info=zs_strutils::format(
		   			 "%s['%s'],%s['%s']"
		  			 ,GET_ILOAD_ACCESS_TYPE_STR(instruction->properties)
		  			 ,GET_ILOAD_R_STR(instruction->properties,instruction->value_op1)
		  			 ,GET_ILOAD_ACCESS_TYPE_STR(instruction->value_op2)
					 ,GET_ILOAD_R_STR(instruction->value_op2,(instruction->value_op2 & 0xff0000) >> 16)
		  		);
				break;
			}

			switch(instruction->byte_code){

			case  BYTE_CODE_NEW_OBJECT_BY_TYPE:
				printf("[" FORMAT_PRINT_INSTRUCTION "]\t%s\t%s\n"
					,idx_instruction
					,req_stk
					,sum_stk_load_stk
					,byte_code_to_str(instruction->byte_code)
					,(char)instruction->value_op1!=ZS_IDX_UNDEFINED?GET_SCRIPT_TYPE_NAME(sfo->script_type_factory,instruction->value_op1):"???"
				);
				break;
			case BYTE_CODE_LOAD_BOOL:
				printf("[" FORMAT_PRINT_INSTRUCTION "]\tLOAD_BOOL\t\t%s\n"
						,idx_instruction
						,req_stk
						,sum_stk_load_stk
						,instruction->value_op2==0?"false":"true");
				break;
			case BYTE_CODE_LOAD_ZS_FLOAT:
				printf("[" FORMAT_PRINT_INSTRUCTION "]\tLOAD_FLT\t\t%f\n"
						,idx_instruction
						,req_stk
						,sum_stk_load_stk
						,*((zs_float *)&instruction->value_op2));
				break;
			case BYTE_CODE_LOAD_ZS_INT:
				printf("[" FORMAT_PRINT_INSTRUCTION "]\tLOAD_INT\t\t%i\n"
						,idx_instruction
						,req_stk
						,sum_stk_load_stk
						,(int)(instruction->value_op2));
				break;
			case BYTE_CODE_LOAD_STRING:
				printf("[" FORMAT_PRINT_INSTRUCTION "]\tLOAD_STRING\t\t%s\n"
						,idx_instruction
						,req_stk
						,sum_stk_load_stk
						,instruction->getConstantValueOp2ToString().c_str());
				break;
			case BYTE_CODE_NEW_STRING:
				printf("[" FORMAT_PRINT_INSTRUCTION "]\tNEW_STRING\t\t%s\n"
						,idx_instruction
						,req_stk
						,sum_stk_load_stk
						,instruction->getConstantValueOp2ToString().c_str());
				break;

			case BYTE_CODE_PUSH_STK_GLOBAL:
			case BYTE_CODE_PUSH_STK_LOCAL:
			//case BYTE_CODE_PUSH_STK_REF:
			case BYTE_CODE_PUSH_STK_THIS:
			case BYTE_CODE_PUSH_STK_MEMBER_VAR:
			case BYTE_CODE_LOAD_THIS_FUNCTION:
			case BYTE_CODE_LOAD_CONSTRUCTOR_FUNCT:
			case BYTE_CODE_LOAD_FUNCTION:
			case BYTE_CODE_FIND_VARIABLE:
			case BYTE_CODE_LOAD_REF:
			case BYTE_CODE_LOAD_LOCAL:
			case BYTE_CODE_LOAD_THIS:
			case BYTE_CODE_LOAD_GLOBAL:
				printf("[" FORMAT_PRINT_INSTRUCTION "]\t%s%s%s\n"
					,idx_instruction
					,req_stk
					,sum_stk_load_stk
					,byte_code_to_str(instruction->byte_code)
					, instruction->byte_code == BYTE_CODE_PUSH_STK_MEMBER_VAR
					|| instruction->byte_code == BYTE_CODE_LOAD_CONSTRUCTOR_FUNCT
					? "\t" : "\t\t"
					,symbol_value.c_str()
				);
				break;

			case BYTE_CODE_LOAD_VECTOR_ITEM:
				printf("[" FORMAT_PRINT_INSTRUCTION "]\t%s\t\t%s {vector}\n"
					,idx_instruction
					,req_stk
					,sum_stk_load_stk
					,byte_code_to_str(instruction->byte_code)
					,symbol_value.c_str()
				);
				break;
			case BYTE_CODE_LOAD_OBJECT_ITEM:
			case BYTE_CODE_LOAD_THIS_VARIABLE:
			case BYTE_CODE_PUSH_STK_THIS_VARIABLE:
			case BYTE_CODE_PUSH_STK_OBJECT_ITEM:
			case BYTE_CODE_PUSH_STK_VECTOR_ITEM:
				//instruction_aux=instruction;

				while((instruction+1)->byte_code == BYTE_CODE_LOAD_OBJECT_ITEM){
					instruction++;
					symbol_value+=zs_string(".")+SFI_GET_SYMBOL_NAME(sfo,instruction);
				}

				printf("[" FORMAT_PRINT_INSTRUCTION "]\t%s%s%s %s\n"
					,idx_instruction
					,req_stk
					,sum_stk_load_stk
					,byte_code_to_str(instruction->byte_code)
					,instruction->byte_code==BYTE_CODE_LOAD_THIS_VARIABLE
					|| instruction->byte_code == BYTE_CODE_LOAD_OBJECT_ITEM
					?"\t\t":"\t"
					,symbol_value.c_str()
					,instruction->properties & INSTRUCTION_PROPERTY_OBJ_ITEM_TO_STORE? "[SLOT]":""
				);
				break;
			case BYTE_CODE_JNT:
			case BYTE_CODE_JT:
			case BYTE_CODE_JMP:
			case BYTE_CODE_JE_CASE:
			case BYTE_CODE_JMP_CASE:
				printf("[" FORMAT_PRINT_INSTRUCTION "]\t%s\t\t\t%03i (ins%s%i) %s\n"
					,idx_instruction
					,req_stk
					,sum_stk_load_stk
					,byte_code_to_str(instruction->byte_code)
					,(int)((instruction-sfo->instructions)+instruction->value_op2)
					,(int)(instruction->value_op2)>=0?"+":""
					,(int)(instruction->value_op2)
					,instruction->properties & INSTRUCTION_PROPERTY_RESET_STACK? "[RST]":""

				);
				break;
			// just the name of op
			case BYTE_CODE_PUSH_SCOPE:
			case BYTE_CODE_POP_SCOPE:
				printf("[" FORMAT_PRINT_INSTRUCTION "]\t%s\n"
					,idx_instruction
					,req_stk
					,sum_stk_load_stk
					,byte_code_to_str(instruction->byte_code)
				);
				break;
			case BYTE_CODE_INSTANCEOF:
				printf("[" FORMAT_PRINT_INSTRUCTION "]\t%s\n"
					,idx_instruction
					,req_stk
					,sum_stk_load_stk
					,byte_code_to_str(instruction->byte_code)
				);
				break;
			case BYTE_CODE_CALL:
			case BYTE_CODE_INDIRECT_GLOBAL_CALL:
			case BYTE_CODE_INDIRECT_LOCAL_CALL:
			case BYTE_CODE_THIS_CALL:
			case BYTE_CODE_INDIRECT_THIS_CALL:
			case BYTE_CODE_SUPER_CALL:
			case BYTE_CODE_UNRESOLVED_CALL:
			case BYTE_CODE_STACK_CALL:
				printf("[" FORMAT_PRINT_INSTRUCTION "]\t%s%s%s%s\targ:%i ret:%i %s\n"
					,idx_instruction
					,req_stk
					,sum_stk_load_stk
					,byte_code_to_str(instruction->byte_code)
					,instruction->byte_code==BYTE_CODE_STACK_CALL?"\t\t":"\t\t\t"
					,instruction->byte_code==BYTE_CODE_THIS_CALL && instruction->value_op2== ZS_IDX_UNDEFINED?"??":""
					,symbol_value.c_str()
					,INSTRUCTION_GET_PARAMETER_COUNT(instruction)
					,INSTRUCTION_GET_RETURN_COUNT(instruction)
					,instruction->properties & INSTRUCTION_PROPERTY_RESET_STACK? "[RST]":""
				);
				break;
			case BYTE_CODE_MEMBER_CALL:
			case BYTE_CODE_CONSTRUCTOR_CALL:
				printf("[" FORMAT_PRINT_INSTRUCTION "]\t%s%sarg:%i ret:%i %s\n"
					,idx_instruction
					,req_stk
					,sum_stk_load_stk
					,byte_code_to_str(instruction->byte_code)
					,instruction->byte_code==BYTE_CODE_MEMBER_CALL?"\t\t":"\t"
					,INSTRUCTION_GET_PARAMETER_COUNT(instruction)
					,INSTRUCTION_GET_RETURN_COUNT(instruction)
					,instruction->properties & INSTRUCTION_PROPERTY_RESET_STACK? "[RST]":""
				);
				break;
			case BYTE_CODE_LOAD_TYPE:
				printf("[" FORMAT_PRINT_INSTRUCTION "]\tLOAD_TYPE\t%s\n"
						,idx_instruction
						,req_stk
						,sum_stk_load_stk
						,zs->getScriptTypeFactory()->getScriptTypeName(instruction->value_op2));
				break;
			case BYTE_CODE_STORE:
			case BYTE_CODE_STORE_CONST:
				printf("[" FORMAT_PRINT_INSTRUCTION "]\t%s%sn:%i %s\n"
					,idx_instruction
					,req_stk
					,sum_stk_load_stk
					,byte_code_to_str(instruction->byte_code)
					,instruction->byte_code==BYTE_CODE_STORE_CONST
					?"\t\t":"\t\t\t"
					,(int)instruction->value_op1
					,instruction->properties & INSTRUCTION_PROPERTY_RESET_STACK? "[RST]":""
				);
				break;
			case BYTE_CODE_ADD_STORE:
			case BYTE_CODE_SUB_STORE:
			case BYTE_CODE_MUL_STORE:
			case BYTE_CODE_DIV_STORE:
			case BYTE_CODE_MOD_STORE:
			case BYTE_CODE_BITWISE_AND_STORE:
			case BYTE_CODE_BITWISE_OR_STORE:
			case BYTE_CODE_BITWISE_XOR_STORE:
			case BYTE_CODE_SHL_STORE:
			case BYTE_CODE_SHR_STORE:
			case BYTE_CODE_PRE_INC:
			case BYTE_CODE_PRE_DEC:
			case BYTE_CODE_POST_INC:
			case BYTE_CODE_NEG_POST_INC:
			case BYTE_CODE_POST_DEC:
			case BYTE_CODE_NEG_POST_DEC:
				printf("[" FORMAT_PRINT_INSTRUCTION "]\t%s%s%s\n"
					,idx_instruction
					,req_stk
					,sum_stk_load_stk
					,byte_code_to_str(instruction->byte_code)
					,"\t\t"
					,instruction->properties & INSTRUCTION_PROPERTY_RESET_STACK? "[RST]":""
				);
				break;
			case BYTE_CODE_IT_INIT:
				printf("[" FORMAT_PRINT_INSTRUCTION "]\t%s\t\t\t[RST]\n"
					,idx_instruction
					,req_stk
					,sum_stk_load_stk
					,byte_code_to_str(instruction->byte_code)
				);
				break;
			default:

				if(iload_info != ""){
					printf("[" FORMAT_PRINT_INSTRUCTION "]\t%s\t\t\t%s\n", // VGET CAN HAVE PRE/POST INCREMENTS
						idx_instruction
						,req_stk
						,sum_stk_load_stk
						,byte_code_to_str(instruction->byte_code)
						,iload_info.c_str()
					);
				}else{
					if(n_ops==0){
						printf("[" FORMAT_PRINT_INSTRUCTION "]\t%s\n", // VGET CAN HAVE PRE/POST INCREMENTS
							idx_instruction
							,req_stk
							,sum_stk_load_stk
							,byte_code_to_str(instruction->byte_code)
						);
					}else if(n_ops==1){
						printf("[" FORMAT_PRINT_INSTRUCTION "]\t%s\t\t\t%i\n"
							,idx_instruction
							,req_stk
							,sum_stk_load_stk
							,byte_code_to_str(instruction->byte_code)
							,instruction->value_op1
						);
					}else{ //2 ops
						printf("[" FORMAT_PRINT_INSTRUCTION "]\t%s\t\t%i,%i\n"
							,idx_instruction
							,req_stk
							,sum_stk_load_stk
							,byte_code_to_str(instruction->byte_code)
							,instruction->value_op1
							,(int)instruction->value_op2
						);
					}
				}
				break;
			}
		}

		printf("\n");

	 }

	void ScriptFunction::checkNativeFunctionParams(Scope *_scope,int _idx_return_type,const zs_string & _function_name,ScriptFunctionParam *_params,int _params_len){

		ZS_UNUSUED_PARAM(_idx_return_type);

		for(int i=0; i < _scope->symbol_functions->count;i++){
			bool same_signature=true;
			Symbol *symbol_function_memeber= (Symbol *)_scope->symbol_functions->items[i];
			ScriptFunction *function_member=(ScriptFunction *)symbol_function_memeber->ref_ptr;
			if(symbol_function_memeber->name==_function_name && function_member->params_len==_params_len){
				for(int j=0; j < _params_len && same_signature;j++){
					same_signature&=function_member->params[j].name==(_params)[j].name;
				}

				if(same_signature){
					THROW_SCRIPT_ERROR_FILE_LINE(NULL,-1,"Function '%s' already binded"
						,function_member->scope_script_function!=NULL?
								zs_strutils::format("%s::%s"
										,function_member->scope_script_function->script_type_owner->str_script_type.c_str()
										,_function_name.c_str()).c_str()
								:_function_name.c_str()

						//,zs_path::get_filename(_file).c_str()
						//,_line
					);
				}
			}
		}
	}

	short	 ScriptFunction::getInstructionLine(Instruction * ins){
		InstructionSourceInfo *info=getInstructionInfo(ins);
		if(info!=NULL){
			return info->line;
		}
		return -1;
	}

	const char *  ScriptFunction::getInstructionSymbolName(Instruction * ins){
		InstructionSourceInfo *info=getInstructionInfo(ins);

		if(info!=NULL && info->ptr_str_symbol_name!=NULL){
			return info->ptr_str_symbol_name->c_str();
		}
		return NULL;
	}

	const char *ScriptFunction::getInstructionSourceFile(Instruction * ins){
		InstructionSourceInfo *info=getInstructionInfo(ins);

		if(info!=NULL){
			return info->file;
		}
		return "unknown_file";
	}

	Symbol * ScriptFunction::registerLocalArgument(
			 Scope * _scope_block
			, const char *_file
			, short _line
			, const zs_string & _symbol_name
			, uint16_t _properties
	){
		Symbol * symbol=NULL;
		short idx_position_aux=(short)local_variables->count;

		if((symbol=_scope_block->registerSymbolVariable(
				_file
				,_line
				, _symbol_name
				, REGISTER_SCOPE_CHECK_REPEATED_SYMBOLS_CURRENT_LEVEL
		))==NULL){
				return NULL;
		}

		symbol->properties=_properties;
		symbol->idx_position = idx_position_aux;

		local_variables->push_back((zs_int)symbol);

		return symbol;
	}

	Symbol * ScriptFunction::registerLocalVariable(
			 Scope * _scope_block
			, const char * _file
			, short _line
			, const zs_string & _symbol_name
			, const zs_string & _str_native_type
			, zs_int _ref_ptr
			, unsigned short _properties
	){
		//ScopeSymbolInfo *irs=new ScopeSymbolInfo;

		Symbol * symbol=NULL;
		short idx_position_aux=(short)local_variables->count;

		if((symbol=_scope_block->registerSymbolVariable(_file,_line, _symbol_name ))==NULL){
				return NULL;
		}

		symbol->ref_ptr =_ref_ptr;
		//scope_symbol->symbol=scope_symbol;
		symbol->str_native_type = _str_native_type;
		symbol->properties = _properties;
		symbol->idx_position = idx_position_aux;

		local_variables->push_back((zs_int)symbol);

		return symbol;
	}

	Symbol * ScriptFunction::registerLocalFunction(
			 Scope * _scope_block
			,const char *_file
			, short _line
			, const zs_string & _function_name
			, ScriptFunctionParam **_params
			,char _params_len
			, int _idx_return_type
			,zs_int _ref_ptr
			, unsigned short _function_properties
	){
		Symbol *symbol_repeat=_scope_block->getSymbol(_function_name, NO_PARAMS_SYMBOL_ONLY,REGISTER_SCOPE_CHECK_REPEATED_SYMBOLS_DOWN),*symbol=NULL;
		zs_string current_file_line=ZS_CONST_STR_IS_EMPTY(_file)?
							zs_strutils::format("[line %i]",_line):
							zs_strutils::format("[%s:%i]",zs_path::get_filename(_file).c_str(),_line);

		if(symbol_repeat != NULL){ // symbol found

			ScriptFunction *sf_repeat=NULL;

			zs_string symbol_file_line=ZS_CONST_STR_IS_EMPTY(symbol_repeat->file)?
					zs_strutils::format("[line %i]",_line):
					zs_strutils::format("[%s:%i]",zs_path::get_filename(symbol_repeat->file).c_str(),_line);


			if(symbol_repeat->properties & SYMBOL_PROPERTY_FUNCTION){
				sf_repeat=(ScriptFunction *)symbol_repeat->ref_ptr;
			}

			if((sf_repeat == NULL) // repeat symbol is variable
							||
			// .. or both are not script neither c ref
			(sf_repeat->properties & FUNCTION_PROPERTY_C_OBJECT_REF) != (_function_properties & FUNCTION_PROPERTY_C_OBJECT_REF)
							||
			// ... or both are script but not in the same scope
			((((sf_repeat->properties | _function_properties) & FUNCTION_PROPERTY_C_OBJECT_REF) == 0) && 	(symbol_repeat->scope != _scope_block) )

			) // repeat symbol are not both c or script funtions
			{
				// if exist override, but should be in the same scope
				THROW_RUNTIME_ERROR("Symbol '%s' defined at %s is already defined at %s"
					,name_script_function.c_str()
					,current_file_line.c_str()
					,_line
					,symbol_file_line.c_str()
					,symbol_repeat->line
				);
			}

			if(((sf_repeat->properties | _function_properties) & FUNCTION_PROPERTY_C_OBJECT_REF) == 0){
				// override script function
				ScriptFunction *sf = (ScriptFunction *)symbol_repeat->ref_ptr;
				sf->clear();
				sf->updateParams(_params,_params_len);
				symbol_repeat->n_params=(char)_params_len;
				return symbol_repeat;
			}else{ // check all params equal
				ScriptFunction::checkNativeFunctionParams(_scope_block,_idx_return_type,_function_name,*_params,_params_len);
			}

			// from here... function should be both as c ref, mark function property to deduce at runtme
			((ScriptFunction *)symbol_repeat->ref_ptr)->properties|=FUNCTION_PROPERTY_DEDUCE_AT_RUNTIME; //
		}

		// register new slot
		symbol =  script_function_factory->newScriptFunction(
				//---- Register data
				 _scope_block
				,_file
				,_line
				//---- Function data
				,idx_script_type_owner 				// idx type is the same which this function belongs to...
				,_function_name
				,_params
				,_params_len
				,_idx_return_type
				,_ref_ptr
				,_function_properties
		);

		// register num symbols only for c symbols...
		if(((_function_properties & FUNCTION_PROPERTY_C_OBJECT_REF) != 0) && (symbol_repeat!=NULL)){
			((ScriptFunction *)symbol->ref_ptr)->properties|=FUNCTION_PROPERTY_DEDUCE_AT_RUNTIME;
		}

		return symbol;
	}

	void ScriptFunction::clearParams(){

		if(params != NULL){
			delete [] params;
			params_len=0;
			params=NULL;
		}

	}


	void ScriptFunction::updateParams(
			ScriptFunctionParam **_params
			,size_t _params_len
	){
		if(_params == NULL){
			return;
		}

		if(_params_len>=FUNCTION_PARAMETER_COUNT_MAX){
			THROW_EXCEPTION(zs_strutils::format("Reached max parameter count (count:%i max:%i)"
				,_params_len
				,FUNCTION_PARAMETER_COUNT_MAX
			));
		}

		// delete existing args...
		clearParams();
		params=*_params;
		params_len = (int)_params_len;

		// mark ptr that was assigned. Now ScriptFunction is in charge to deallocate this variable
		*_params=NULL;
	}

	void ScriptFunction::clear(){
		unresolved_symbols.clear();
		// delete symbols refs from scope...
		//symbol_registered_functions->clear();
		local_variables->clear();

		// delete arg info variables...
		clearParams();

		// delete arg info variables...
		if(instructions != NULL){
			free(instructions);
		}

		instructions=NULL;
	}

	void ScriptFunction::removeUnusuedScopes(){

		// remove children scopes type block
		scope_script_function->removeChildrenBlockTypes();

		// count number vars at the top most
		int n_var_scope=scope_script_function->countVariables(true);

		// remove last symbols
		local_variables->resize(n_var_scope);


	}

	bool ScriptFunction::linkUnresolvedSymbols(){
		ScriptType *sc_found=NULL;
		if(unresolved_symbols.count > 0){

			const char *str_aux=NULL;
			int i=0;
			while(i < unresolved_symbols.count){
				Instruction *unresolved_instruction=&instructions[unresolved_symbols.items[i]];
				const char *ptr_str_symbol_to_find=SFI_GET_SYMBOL_NAME(this,unresolved_instruction);
				const char *instruction_file=SFI_GET_FILE(this,unresolved_instruction);
				int instruction_line=SFI_GET_LINE(this,unresolved_instruction);
				Symbol *symbol_found=NULL;
				short idx_sc_found=ZS_IDX_UNDEFINED;


				if(ptr_str_symbol_to_find==NULL){
					THROW_SCRIPT_ERROR_FILE_LINE(
						instruction_file
						,instruction_line
						,"Cannot find symbol name at instruction %i"
						,unresolved_symbols.items[i]
					);
				}


				if((idx_sc_found= script_type_factory->getIdxScriptType(ptr_str_symbol_to_find))!= ZS_IDX_UNDEFINED){ // check if type
					unresolved_instruction->byte_code=BYTE_CODE_LOAD_TYPE;
					unresolved_instruction->value_op2=idx_sc_found;
				 }else if((str_aux=strstr(ptr_str_symbol_to_find,"::")) != NULL){ // static
					 zs_string static_error;
					char copy_aux[512]={0};

					// get type
					strncpy(copy_aux,ptr_str_symbol_to_find,str_aux-ptr_str_symbol_to_find);

					sc_found=zs->getScriptTypeFactory()->getScriptType(copy_aux);

					if(sc_found!=NULL){
						// advance ::
						str_aux+=2;

						//get member
						strcpy(copy_aux,str_aux);

						symbol_found=sc_found->getSymbol(copy_aux); // ... and member as well we can define the instruction here
					}
				}else{ // global scope
					symbol_found = MAIN_SCOPE(this)->getSymbol(ptr_str_symbol_to_find,NO_PARAMS_SYMBOL_ONLY,REGISTER_SCOPE_CHECK_REPEATED_SYMBOLS_DOWN);
				}


				if(symbol_found !=NULL){ // symbol found

					if(symbol_found->properties & SYMBOL_PROPERTY_FUNCTION){
						unresolved_instruction->byte_code=BYTE_CODE_CALL;
						unresolved_instruction->value_op2=(zs_int)symbol_found; // store script function
					}else{ // global variable
						if(unresolved_instruction->byte_code == BYTE_CODE_UNRESOLVED_CALL){
							unresolved_instruction->byte_code=BYTE_CODE_INDIRECT_GLOBAL_CALL;
						}else{

							if(unresolved_instruction->properties & INSTRUCTION_PROPERTY_USE_PUSH_STK){
								unresolved_instruction->byte_code=BYTE_CODE_PUSH_STK_GLOBAL;
							}else{
								unresolved_instruction->byte_code=BYTE_CODE_LOAD_GLOBAL;
							}
						}

						unresolved_instruction->value_op2=symbol_found->idx_position;
					}

					// unstack unresolved symbol
					unresolved_symbols.erase(i);

				}
				else{ // next
					i++;
				}

				// deallocate item
			}
		}

		return unresolved_symbols.count==0;
	}

	void ScriptFunction::addUnresolvedSymbol(zs_int instruction){
		unresolved_symbols.push_back(instruction);
	}

	ZetScript *ScriptFunction::getZetScript(){
		return zs;
	}

	ScriptFunction::~ScriptFunction(){
		clear();

		for(int i=0; i < instruction_source_info.count; i++){
			InstructionSourceInfo *isi=(InstructionSourceInfo *)instruction_source_info.items[i];
			if(isi != NULL){
				delete isi;
			}
		}

		delete local_variables;
		local_variables=NULL;
	}

}
