/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
#include "zetscript.h"

#define ZS_HEADER_FORMAT_INSTRUCTION "%04i|%2i|%02i"

#define ZS_GET_ILOAD_ACCESS_TYPE_STR(properties) \
((properties) & INSTRUCTION_PROPERTY_ILOAD_R_ACCESS_THIS_VAR) ? "This"\
:"Local"\

#define ZS_GET_ILOAD_R_STR(properties,value) \
	((properties) & INSTRUCTION_PROPERTY_ILOAD_R_ACCESS_THIS_VAR) ? ((Symbol *)sc->scope->symbol_variables->get(value))->name.toConstChar()\
	:((Symbol *)sfo->local_variables->get(value))->name.toConstChar()\

namespace zetscript{

	Function::Function(
			ZetScript * _zs
			,int _idx_script_function
			,TypeId _owner_type_id
			,int _idx_position
			,const String & _function_name
			, FunctionParam **_params
			, int _params_len
			,TypeId _return_type_id
			, zs_int _ref_native_function_ptr
			,uint16_t _properties
		) {

		// function data...
		owner_type_id=_owner_type_id;
		name=_function_name;
		id=_idx_script_function;
		idx_position = _idx_position;
		return_type_id = _return_type_id;
		ref_native_function_ptr=_ref_native_function_ptr;
		properties = _properties;
		scope = NULL;
		instructions=NULL;
		instructions_len = 0;


		// local symbols for type or function...
		local_variables=new Vector<Symbol *>();
		params = NULL;//new Vector();
		params_len = 0;

		updateParams(_params,_params_len);

		// factories
		zs = _zs;
		scope_factory = zs->getScopeFactory();
		script_function_factory= zs->getFunctionFactory();
		type_factory=zs->getTypeFactory();

		min_code_stack_needed=0;

	}

	InstructionSourceInfo * Function::getInstructionInfo(Instruction *instruction){
		short idx= (instruction-this->instructions);///sizeof(Instruction *);
		//if(instruction_source_info.items[idx]==1){
		return (InstructionSourceInfo *)instruction_source_infos.get(idx);
	}

	 void Function::printGeneratedCode(Function *sfo,Type *sc){

		// PRE: it should printed after compile and updateReferences.
		// first print functions  ...
		ZetScript *zs=sfo->zs;
		int sum_stk_load_stk=0;
		int max_acc_stk_load=0;
		int req_stk;


		if(sfo->properties & FUNCTION_PROPERTY_NATIVE_OBJECT_REF){ // c functions has no script instructions
			return;
		}

		if(sfo->instructions == NULL){
			return;
		}

		if(sfo->instructions->byte_code== BYTE_CODE_END_FUNCTION){
			return;
		}

		String symbol_ref="????";
		String class_str="";
		String symbol_value="";
		String iload_info="";

		if(sc==NULL){ // no type is a function on a global scope
			symbol_ref=sfo->name;
		}else{ // is a type
			symbol_ref=sfo->name;//+String("::")+String("????");
			class_str=String(sc->name)+"::";
		}



		printf("______________________________________________________________\n\n");
		printf(" Function: '%s%s'                                             \n",class_str.toConstChar(),symbol_ref.toConstChar());
		printf(" Stack code: %i                                       		  \n",sfo->min_code_stack_needed);
		printf(" Stack local vars: %i                                         \n",sfo->local_variables->size());
		printf(" Total stack required: %i                                     \n",sfo->local_variables->size()+sfo->min_code_stack_needed);
		printf(" Scopes: %i                                                   \n\n",sfo->scope->getScopes()->size());

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

			if((int8_t)value_op1 != ZS_UNDEFINED_IDX){
				n_ops++;
			}

			 if(value_op2 != ZS_UNDEFINED_IDX){
				 n_ops++;
			 }

			 if((
					   instruction->byte_code==BYTE_CODE_LOAD_THIS_VARIABLE
					|| instruction->byte_code==BYTE_CODE_LOAD_THIS_FUNCTION
					|| instruction->byte_code==BYTE_CODE_THIS_CALL
					|| instruction->byte_code==BYTE_CODE_PUSH_STK_THIS_VARIABLE
			)){

				// ... we create new instruction
				 symbol_value="this."+symbol_value;
			 }else if(   instruction->byte_code==BYTE_CODE_INDIRECT_GLOBAL_CALL
					 || instruction->byte_code== BYTE_CODE_INDIRECT_LOCAL_CALL

			 ){
				 symbol_value="@"+symbol_value;
			 }else if(instruction->byte_code== BYTE_CODE_INDIRECT_THIS_CALL){
				 symbol_value="@this."+symbol_value;
			 }

			 req_stk=instruction_num_required_stack(instruction);

			if(req_stk==ZS_NUM_REQUIRED_BYTE_CODE_ID_NOT_MANAGED){
				ZS_THROW_RUNTIME_ERROR("byte_code_num_required_stack: byte_code '%i' not managed", instruction->byte_code);
			}

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
				 iload_info=StringUtils::format("%s", instruction->getConstantValueOp2ToString().toConstChar());
				 break;
			case INSTRUCTION_PROPERTY_ILOAD_R: /* only perfom with one Register */\
				 iload_info=StringUtils::format("%s['%s']"
					 ,ZS_GET_ILOAD_ACCESS_TYPE_STR(instruction->properties)
					 ,ZS_GET_ILOAD_R_STR(instruction->properties,instruction->value_op1)
				 );
				 break;
			case INSTRUCTION_PROPERTY_ILOAD_KR: /* perfom Konstant-Register*/\
			 	 iload_info=StringUtils::format("%s,%s['%s']"
					 ,instruction->getConstantValueOp2ToString().toConstChar()
					 ,ZS_GET_ILOAD_ACCESS_TYPE_STR(instruction->properties)
					 ,ZS_GET_ILOAD_R_STR(instruction->properties,instruction->value_op1)
				 );
				break;
			case INSTRUCTION_PROPERTY_ILOAD_RK: /* perfom Register-Konstant */\
				 iload_info=StringUtils::format("%s['%s'],%s"
					 ,ZS_GET_ILOAD_ACCESS_TYPE_STR(instruction->properties)
					 ,ZS_GET_ILOAD_R_STR(instruction->properties,instruction->value_op1)
					 ,instruction->getConstantValueOp2ToString().toConstChar()
				 );
				break;
		   case INSTRUCTION_PROPERTY_ILOAD_RR: /* perfom Register-Register*/ \
		   	   iload_info=StringUtils::format(
		   			 "%s['%s'],%s['%s']"
		  			 ,ZS_GET_ILOAD_ACCESS_TYPE_STR(instruction->properties)
		  			 ,ZS_GET_ILOAD_R_STR(instruction->properties,instruction->value_op1)
		  			 ,ZS_GET_ILOAD_ACCESS_TYPE_STR(instruction->value_op2)
					 ,ZS_GET_ILOAD_R_STR(instruction->value_op2,(instruction->value_op2 & 0xff0000) >> 16)
		  		);
				break;
			}

			switch(instruction->byte_code){

			case  BYTE_CODE_NEW_OBJECT_BY_TYPE:
				printf("[" ZS_HEADER_FORMAT_INSTRUCTION "]\t%s\t%s\n"
					,idx_instruction
					,req_stk
					,sum_stk_load_stk
					,ByteCodeHelper::getByteCodeName(instruction->byte_code)
					,(int8_t)instruction->value_op1!=ZS_UNDEFINED_IDX?ZS_GET_TYPE_NAME(sfo->type_factory,instruction->value_op1):"???"
				);
				break;
			case BYTE_CODE_LOAD_BOOL:
				printf("[" ZS_HEADER_FORMAT_INSTRUCTION "]\tLOAD_BOOL\t\t%s\n"
						,idx_instruction
						,req_stk
						,sum_stk_load_stk
						,instruction->value_op2==0?"false":"true");
				break;
			case BYTE_CODE_LOAD_FLOAT:
				printf("[" ZS_HEADER_FORMAT_INSTRUCTION "]\tLOAD_FLT\t\t%f\n"
						,idx_instruction
						,req_stk
						,sum_stk_load_stk
						,ZS_READ_INTPTR_FLOAT(instruction->value_op2));
				break;
			case BYTE_CODE_LOAD_INT:
				printf("[" ZS_HEADER_FORMAT_INSTRUCTION "]\tLOAD_INT\t\t%i\n"
						,idx_instruction
						,req_stk
						,sum_stk_load_stk
						,(int)(instruction->value_op2));
				break;
			case BYTE_CODE_LOAD_STRING:
				printf("[" ZS_HEADER_FORMAT_INSTRUCTION "]\tLOAD_STRING\t\t%s\n"
						,idx_instruction
						,req_stk
						,sum_stk_load_stk
						,instruction->getConstantValueOp2ToString().toConstChar());
				break;
			case BYTE_CODE_NEW_STRING:
				printf("[" ZS_HEADER_FORMAT_INSTRUCTION "]\tNEW_STRING\t\t%s\n"
						,idx_instruction
						,req_stk
						,sum_stk_load_stk
						,instruction->getConstantValueOp2ToString().toConstChar());
				break;

			case BYTE_CODE_PUSH_STK_GLOBAL:
			case BYTE_CODE_PUSH_STK_GLOBAL_IRGO:
			case BYTE_CODE_PUSH_STK_LOCAL:
			//case BYTE_CODE_PUSH_STK_REF:
			case BYTE_CODE_PUSH_STK_THIS:
			case BYTE_CODE_LOAD_THIS_FUNCTION:
			case BYTE_CODE_LOAD_CONSTRUCTOR_FUNCT:
			case BYTE_CODE_LOAD_FUNCTION:
			case BYTE_CODE_FIND_VARIABLE:
			case BYTE_CODE_LOAD_REF:
			case BYTE_CODE_LOAD_LOCAL:
			case BYTE_CODE_LOAD_THIS:
			case BYTE_CODE_LOAD_GLOBAL:
				printf("[" ZS_HEADER_FORMAT_INSTRUCTION "]\t%s%s%s\n"
					,idx_instruction
					,req_stk
					,sum_stk_load_stk
					,ByteCodeHelper::getByteCodeName(instruction->byte_code)
					,instruction->byte_code == BYTE_CODE_LOAD_CONSTRUCTOR_FUNCT
					? "\t" : "\t\t"
					,symbol_value.toConstChar()
				);
				break;

			case BYTE_CODE_LOAD_ARRAY_ITEM:
				printf("[" ZS_HEADER_FORMAT_INSTRUCTION "]\t%s\t\t%s {vector}\n"
					,idx_instruction
					,req_stk
					,sum_stk_load_stk
					,ByteCodeHelper::getByteCodeName(instruction->byte_code)
					,symbol_value.toConstChar()
				);
				break;
			case BYTE_CODE_LOAD_OBJECT_ITEM:
			case BYTE_CODE_LOAD_THIS_VARIABLE:
			case BYTE_CODE_PUSH_STK_THIS_VARIABLE:
			case BYTE_CODE_PUSH_STK_OBJECT_ITEM:
			case BYTE_CODE_PUSH_STK_ARRAY_ITEM:
				//instruction_aux=instruction;

				while((instruction+1)->byte_code == BYTE_CODE_LOAD_OBJECT_ITEM){
					instruction++;
					symbol_value+=String(".")+SFI_GET_SYMBOL_NAME(sfo,instruction);
				}

				printf("[" ZS_HEADER_FORMAT_INSTRUCTION "]\t%s%s%s %s\n"
					,idx_instruction
					,req_stk
					,sum_stk_load_stk
					,ByteCodeHelper::getByteCodeName(instruction->byte_code)
					,instruction->byte_code==BYTE_CODE_LOAD_THIS_VARIABLE
					|| instruction->byte_code == BYTE_CODE_LOAD_OBJECT_ITEM
					?"\t\t":"\t"
					,symbol_value.toConstChar()
					,instruction->properties & INSTRUCTION_PROPERTY_CONTAINER_SLOT_ASSIGMENT? "[SLOT]":
					instruction->properties & INSTRUCTION_PROPERTY_FOR_ASSIGN? "[STORE]":
					instruction->properties & INSTRUCTION_PROPERTY_CALLING_FUNCTION?	"[CALL]":""
				);
				break;
			case BYTE_CODE_JNT:
			case BYTE_CODE_JT:
			case BYTE_CODE_JMP:
			case BYTE_CODE_JE_CASE:
			case BYTE_CODE_JMP_CASE:
				printf("[" ZS_HEADER_FORMAT_INSTRUCTION "]\t%s\t\t\t%03i (ins%s%i) %s\n"
					,idx_instruction
					,req_stk
					,sum_stk_load_stk
					,ByteCodeHelper::getByteCodeName(instruction->byte_code)
					,(int)((instruction-sfo->instructions)+instruction->value_op2)
					,(int)(instruction->value_op2)>=0?"+":""
					,(int)(instruction->value_op2)
					,instruction->properties & INSTRUCTION_PROPERTY_RESET_STACK? "[RST]":""

				);
				break;
			// just the name of op
			case BYTE_CODE_PUSH_SCOPE:
			case BYTE_CODE_POP_SCOPE:
			case BYTE_CODE_NEW_OBJECT_BY_VALUE:
				printf("[" ZS_HEADER_FORMAT_INSTRUCTION "]\t%s\n"
					,idx_instruction
					,req_stk
					,sum_stk_load_stk
					,ByteCodeHelper::getByteCodeName(instruction->byte_code)
				);
				break;
			case BYTE_CODE_INSTANCEOF:
				printf("[" ZS_HEADER_FORMAT_INSTRUCTION "]\t%s\t\t%s\n"
					,idx_instruction
					,req_stk
					,sum_stk_load_stk
					,ByteCodeHelper::getByteCodeName(instruction->byte_code)
					,symbol_value.toConstChar()
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
				printf("[" ZS_HEADER_FORMAT_INSTRUCTION "]\t%s%s%s%s\targ:%i ret:%i %s\n"
					,idx_instruction
					,req_stk
					,sum_stk_load_stk
					,ByteCodeHelper::getByteCodeName(instruction->byte_code)
					,instruction->byte_code==BYTE_CODE_STACK_CALL?"\t\t":"\t\t\t"
					,instruction->byte_code==BYTE_CODE_THIS_CALL && instruction->value_op2== ZS_UNDEFINED_IDX?"??":""
					,symbol_value.toConstChar()
					,INSTRUCTION_GET_PARAMETER_COUNT(instruction)
					,INSTRUCTION_GET_RETURN_COUNT(instruction)
					,instruction->properties & INSTRUCTION_PROPERTY_RESET_STACK? "[RST]":""
				);
				break;
			case BYTE_CODE_MEMBER_CALL:
			case BYTE_CODE_CONSTRUCTOR_CALL:
				printf("[" ZS_HEADER_FORMAT_INSTRUCTION "]\t%s%sarg:%i ret:%i %s\n"
					,idx_instruction
					,req_stk
					,sum_stk_load_stk
					,ByteCodeHelper::getByteCodeName(instruction->byte_code)
					,instruction->byte_code==BYTE_CODE_MEMBER_CALL?"\t\t":"\t"
					,INSTRUCTION_GET_PARAMETER_COUNT(instruction)
					,INSTRUCTION_GET_RETURN_COUNT(instruction)
					,instruction->properties & INSTRUCTION_PROPERTY_RESET_STACK? "[RST]":""
				);
				break;
			case BYTE_CODE_LOAD_TYPE:
				printf("[" ZS_HEADER_FORMAT_INSTRUCTION "]\tLOAD_TYPE\t\t%s\n"
						,idx_instruction
						,req_stk
						,sum_stk_load_stk
						,zs->getTypeFactory()->getTypeName(instruction->value_op2));
				break;
			case BYTE_CODE_STORE:
			case BYTE_CODE_STORE_CONST:
				printf("[" ZS_HEADER_FORMAT_INSTRUCTION "]\t%s%sn:%i %s\n"
					,idx_instruction
					,req_stk
					,sum_stk_load_stk
					,ByteCodeHelper::getByteCodeName(instruction->byte_code)
					,instruction->byte_code==BYTE_CODE_STORE_CONST
					?"\t\t":"\t\t\t"
					,(int)instruction->value_op1
					,instruction->properties & INSTRUCTION_PROPERTY_RESET_STACK? "[RST]":""
				);
				break;
			case BYTE_CODE_ADD_ASSIGN:
			case BYTE_CODE_SUB_ASSIGN:
			case BYTE_CODE_MUL_ASSIGN:
			case BYTE_CODE_DIV_ASSIGN:
			case BYTE_CODE_MOD_ASSIGN:
			case BYTE_CODE_BITWISE_AND_ASSIGN:
			case BYTE_CODE_BITWISE_OR_ASSIGN:
			case BYTE_CODE_BITWISE_XOR_ASSIGN:
			case BYTE_CODE_SHL_ASSIGN:
			case BYTE_CODE_SHR_ASSIGN:
			case BYTE_CODE_PRE_INC:
			case BYTE_CODE_PRE_DEC:
			case BYTE_CODE_POST_INC:
			//case BYTE_CODE_NEG_POST_INC:
			//case BYTE_CODE_BWC_POST_INC:
			case BYTE_CODE_POST_DEC:
			//case BYTE_CODE_NEG_POST_DEC:
			//case BYTE_CODE_BWC_POST_DEC:
				printf("[" ZS_HEADER_FORMAT_INSTRUCTION "]\t%s%s%s\n"
					,idx_instruction
					,req_stk
					,sum_stk_load_stk
					,ByteCodeHelper::getByteCodeName(instruction->byte_code)
					,"\t\t"
					,instruction->properties & INSTRUCTION_PROPERTY_RESET_STACK? "[RST]":""
				);
				break;
			case BYTE_CODE_IT_INIT:
				printf("[" ZS_HEADER_FORMAT_INSTRUCTION "]\t%s\t\t\t[RST]\n"
					,idx_instruction
					,req_stk
					,sum_stk_load_stk
					,ByteCodeHelper::getByteCodeName(instruction->byte_code)
				);
				break;
			default:

				if(iload_info != ""){
					printf("[" ZS_HEADER_FORMAT_INSTRUCTION "]\t%s\t\t\t%s\n", // VGET CAN HAVE PRE/POST INCREMENTS
						idx_instruction
						,req_stk
						,sum_stk_load_stk
						,ByteCodeHelper::getByteCodeName(instruction->byte_code)
						,iload_info.toConstChar()
					);
				}else{
					if(n_ops==0){
						printf("[" ZS_HEADER_FORMAT_INSTRUCTION "]\t%s\n", // VGET CAN HAVE PRE/POST INCREMENTS
							idx_instruction
							,req_stk
							,sum_stk_load_stk
							,ByteCodeHelper::getByteCodeName(instruction->byte_code)
						);
					}else if(n_ops==1){
						printf("[" ZS_HEADER_FORMAT_INSTRUCTION "]\t%s\t\t\t%i\n"
							,idx_instruction
							,req_stk
							,sum_stk_load_stk
							,ByteCodeHelper::getByteCodeName(instruction->byte_code)
							,instruction->value_op1
						);
					}else{ //2 ops
						printf("[" ZS_HEADER_FORMAT_INSTRUCTION "]\t%s\t\t%i,%i\n"
							,idx_instruction
							,req_stk
							,sum_stk_load_stk
							,ByteCodeHelper::getByteCodeName(instruction->byte_code)
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

	void Function::checkNativeFunctionParams(Scope *_scope,int _idx_return_type,const String & _function_name,FunctionParam *_params,int _params_len){

		ZS_UNUSUED_PARAM(_idx_return_type);

		for(int i=0; i < _scope->symbol_functions->size();i++){
			bool same_signature=true;
			Symbol *symbol_function_memeber= (Symbol *)_scope->symbol_functions->get(i);
			Function *function_member=(Function *)symbol_function_memeber->ref_ptr;
			if(symbol_function_memeber->name==_function_name && function_member->params_len==_params_len){
				for(int j=0; j < _params_len && same_signature;j++){
					same_signature&=function_member->params[j].name==(_params)[j].name;
				}

				if(same_signature){
					ZS_THROW_EXCEPTION_FILE_LINE(NULL,-1,"Function '%s' with same signature already binded"
						,function_member->scope!=NULL?
								StringUtils::format("%s::%s"
										,function_member->scope->owner_type->name.toConstChar()
										,_function_name.toConstChar()).toConstChar()
								:_function_name.toConstChar()

						//,Path::getFilename(_file).toConstChar()
						//,_line
					);
				}
			}
		}
	}

	short	 Function::getInstructionLine(Instruction * ins){
		InstructionSourceInfo *info=getInstructionInfo(ins);
		if(info!=NULL){
			return info->line;
		}
		return -1;
	}

	const char *  Function::getInstructionSymbolName(Instruction * ins){
		InstructionSourceInfo *info=getInstructionInfo(ins);

		if(info!=NULL && info->ptr_str_symbol_name!=NULL){
			return info->ptr_str_symbol_name;
		}
		return NULL;
	}

	const char *Function::getInstructionSourceFile(Instruction * ins){
		InstructionSourceInfo *info=getInstructionInfo(ins);

		if(info!=NULL){
			return info->file;
		}
		return "unknown_file";
	}

	Function	*	Function::getInstructionFunctionLastCall(Instruction * _instruction){
		short idx= (_instruction-this->instructions);///sizeof(Instruction *);
		Function	*sf=NULL;
		bool exists=false;
		sf=(Function	*)this->instruction_last_native_call.get(idx,&exists);
		return sf;
	}


	void				Function::clearFunctionLastCalls(){
		this->instruction_last_native_call.clear();
	}

	void				Function::setInstructionFunctionLastCall(Instruction * _instruction,Function *_sf){
		short idx= (_instruction-this->instructions);///sizeof(Instruction *);
		this->instruction_last_native_call.set(idx,(zs_int)_sf);
	}

	Symbol * Function::registerLocalArgument(
			 Scope * _scope_block
			, const char *_file
			, short _line
			, const String & _symbol_name
			, uint16_t _properties
	){
		Symbol * symbol=NULL;
		short idx_position_aux=(short)local_variables->size();

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

		local_variables->append(symbol);

		return symbol;
	}

	Symbol * Function::registerLocalVariable(
			 Scope * _scope_block
			, const char * _file
			, short _line
			, const String & _symbol_name
			, unsigned short _properties
	){
		//ScopeSymbolInfo *irs=new ScopeSymbolInfo;

		Symbol * symbol=NULL;
		short idx_position_aux=(short)local_variables->size();

		if((symbol=_scope_block->registerSymbolVariable(_file,_line, _symbol_name ))==NULL){
				return NULL;
		}


		symbol->properties = _properties;
		symbol->idx_position = idx_position_aux;

		if(this==ZS_MAIN_FUNCTION(this)){
			// if function is main, we must initialize stack element at position local_variables->size()
			// to prevent collect garbage information from previous evaluations where the stack element
			// was used for temporal operations
			vm_set_stack_element_at(zs->getVirtualMachine(),local_variables->size(),k_stk_undefined);
		}

		local_variables->append(symbol);

		return symbol;
	}

	Symbol * Function::registerLocalFunction(
			 Scope * _scope_block
			,const char *_file
			, short _line
			, const String & _function_name
			, FunctionParam **_params
			,int8_t _params_len
			, TypeId _return_type_id
			,zs_int _ref_ptr
			, unsigned short _function_properties
	){
		Symbol *symbol_repeat=_scope_block->getSymbol(_function_name, ZS_NO_PARAMS_SYMBOL_ONLY,REGISTER_SCOPE_CHECK_REPEATED_SYMBOLS_DOWN),*symbol=NULL;
		String current_file_line=ZS_STR_CONST_IS_EMPTY(_file)?
							StringUtils::format(ZS_FORMAT_LINE,_line):
							StringUtils::format(ZS_FORMAT_FILE_LINE,Path::getFilename(_file).toConstChar(),_line);

		if(symbol_repeat != NULL){ // symbol found

			Function *sf_repeat=NULL;

			String symbol_file_line=ZS_STR_CONST_IS_EMPTY(symbol_repeat->file)?
					StringUtils::format(ZS_FORMAT_LINE,_line):
					StringUtils::format(ZS_FORMAT_FILE_LINE,Path::getFilename(symbol_repeat->file).toConstChar(),_line);


			if(symbol_repeat->properties & SYMBOL_PROPERTY_FUNCTION){
				sf_repeat=(Function *)symbol_repeat->ref_ptr;
			}

			if((sf_repeat == NULL) // repeat symbol is variable
							||
			// .. or both are not script neither c ref
			(sf_repeat->properties & FUNCTION_PROPERTY_NATIVE_OBJECT_REF) != (_function_properties & FUNCTION_PROPERTY_NATIVE_OBJECT_REF)
							||
			// ... or both are script but not in the same scope
			((((sf_repeat->properties | _function_properties) & FUNCTION_PROPERTY_NATIVE_OBJECT_REF) == 0) && 	(symbol_repeat->scope != _scope_block) )

			) // repeat symbol are not both c or script funtions
			{
				// if exist override, but should be in the same scope
				ZS_THROW_RUNTIME_ERROR("Symbol '%s' defined at %s is already defined at %s"
					,name.toConstChar()
					,current_file_line.toConstChar()
					,_line
					,symbol_file_line.toConstChar()
					,symbol_repeat->line
				);
			}

			if(((sf_repeat->properties | _function_properties) & FUNCTION_PROPERTY_NATIVE_OBJECT_REF) == 0){
				// override script function
				Function *sf = (Function *)symbol_repeat->ref_ptr;
				sf->clear();
				sf->updateParams(_params,_params_len);
				symbol_repeat->n_params=(int8_t)_params_len;
				return symbol_repeat;
			}else{ // check all params equal
				Function::checkNativeFunctionParams(_scope_block,_return_type_id,_function_name,*_params,_params_len);
			}

			// from here... function should be both as c ref, mark function property to deduce at runtme
			((Function *)symbol_repeat->ref_ptr)->properties|=FUNCTION_PROPERTY_DEDUCE_AT_RUNTIME; //
		}

		// register new slot
		symbol =  script_function_factory->newFunction(
				//---- Register data
				 _scope_block
				,_file
				,_line
				//---- Function data
				,owner_type_id 				// idx type is the same which this function belongs to...
				,_function_name
				,_params
				,_params_len
				,_return_type_id
				,_ref_ptr
				,_function_properties
		);

		// register num symbols only for c symbols...
		if(((_function_properties & FUNCTION_PROPERTY_NATIVE_OBJECT_REF) != 0) && (symbol_repeat!=NULL)){
			((Function *)symbol->ref_ptr)->properties|=FUNCTION_PROPERTY_DEDUCE_AT_RUNTIME;
		}

		return symbol;
	}

	void Function::clearParams(){

		if(params != NULL){
			delete [] params;
			params_len=0;
			params=NULL;
		}

	}


	void Function::updateParams(
			FunctionParam **_params
			,int _params_len
	){
		if(_params == NULL){
			return;
		}

		if(_params_len>=FUNCTION_PARAMETER_COUNT_MAX){
			ZS_THROW_EXCEPTION("Reached max parameter count (count:%i max:%i)"
				,_params_len
				,FUNCTION_PARAMETER_COUNT_MAX
			);
		}

		// delete existing args...
		clearParams();
		params=*_params;
		params_len = _params_len;

		// mark ptr that was assigned. Now Function is in charge to deallocate this variable
		*_params=NULL;
	}

	void Function::clearUnresolvedSymbols(){
		unresolved_symbols.clear();
	}

	void Function::clear(){
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

	void Function::removeUnusuedScopes(){

		// remove children scopes type block
		scope->removeChildrenBlockTypes();

		// count number vars at the top most
		int n_var_scope=scope->countVariables(true);

		// remove last symbols
		local_variables->resize(n_var_scope);


	}

	bool Function::linkUnresolvedSymbols(){
		Type *sc_found=NULL;
		if(unresolved_symbols.size() > 0){

			const char *str_aux=NULL;
			int i=0;
			while(i < unresolved_symbols.size()){
				Instruction *unresolved_instruction=&instructions[unresolved_symbols.get(i)];
				const char *ptr_str_symbol_to_find=SFI_GET_SYMBOL_NAME(this,unresolved_instruction);
				const char *instruction_file=SFI_GET_FILE(this,unresolved_instruction);
				int instruction_line=SFI_GET_LINE(this,unresolved_instruction);
				Symbol *symbol_found=NULL;
				short idx_sc_found=ZS_UNDEFINED_IDX;


				if(ptr_str_symbol_to_find==NULL){
					ZS_THROW_EXCEPTION_FILE_LINE(
						instruction_file
						,instruction_line
						,"Cannot find symbol name at instruction %i"
						,unresolved_symbols.get(i)
					);
				}


				if((idx_sc_found= type_factory->getTypeId(ptr_str_symbol_to_find))!= ZS_UNDEFINED_IDX){ // check if type

					// set idx type found
					unresolved_instruction->value_op2=idx_sc_found;

					switch(unresolved_instruction->byte_code){
					case BYTE_CODE_INSTANCEOF:
						break;
					default:
						// cheange type
						unresolved_instruction->byte_code=BYTE_CODE_LOAD_TYPE;
						break;
					}
				 }else if((str_aux=strstr(ptr_str_symbol_to_find,"::")) != NULL){ // static
					 String static_error;
					char copy_aux[512]={0};

					// get type
					strncpy(copy_aux,ptr_str_symbol_to_find,str_aux-ptr_str_symbol_to_find);

					sc_found=zs->getTypeFactory()->getType(copy_aux);

					if(sc_found!=NULL){
						// advance ::
						str_aux+=2;

						//get member
						strcpy(copy_aux,str_aux);

						symbol_found=sc_found->getSymbol(copy_aux); // ... and member as well we can define the instruction here
					}
				}else{
					if(unresolved_instruction->byte_code==BYTE_CODE_THIS_CALL){

						Type *sc_sf=this->scope->getOwnerType();
						for(int j = 0; j < sc_sf->scope->symbol_functions->size(); j++){
							Symbol *sv=(Symbol *)sc_sf->scope->symbol_functions->get(j);
							if(
								   sv->name==ptr_str_symbol_to_find
							){
								symbol_found=sv;
								break;
							}
						}
					}else{
						// global scope
						symbol_found = ZS_MAIN_SCOPE(this)->getSymbol(ptr_str_symbol_to_find,ZS_NO_PARAMS_SYMBOL_ONLY,REGISTER_SCOPE_CHECK_REPEATED_SYMBOLS_DOWN);
					}
				}


				if(symbol_found !=NULL){ // symbol found

					if(
							(symbol_found->properties & SYMBOL_PROPERTY_FUNCTION)
					){
						// if symbol is function, then there's two possible actions:
						// 1. call function
						// 2. load function
						if(unresolved_instruction->byte_code!=BYTE_CODE_THIS_CALL){

							if(unresolved_instruction->value_op1==(uint8_t)ZS_UNDEFINED_IDX){
								// if byte code has not defined number of parameters, it loads the function
								unresolved_instruction->byte_code=BYTE_CODE_LOAD_FUNCTION;
							}else{
								unresolved_instruction->byte_code=BYTE_CODE_CALL;
							}
						}
						unresolved_instruction->value_op2=(zs_int)symbol_found; // store script function
					}else{ // global variable
						if(unresolved_instruction->byte_code == BYTE_CODE_UNRESOLVED_CALL){
							unresolved_instruction->byte_code=BYTE_CODE_INDIRECT_GLOBAL_CALL;
						}else{
							// global
							if(unresolved_instruction->value_op1==INSTRUCTION_VALUE_OP1_PUSH_STK_GLOBAL_IRGO){
								unresolved_instruction->byte_code=BYTE_CODE_PUSH_STK_GLOBAL_IRGO;
							}else{
								if(unresolved_instruction->properties & INSTRUCTION_PROPERTY_USE_PUSH_STK){
									unresolved_instruction->byte_code=BYTE_CODE_PUSH_STK_GLOBAL;
								}else{
									unresolved_instruction->byte_code=BYTE_CODE_LOAD_GLOBAL;
								}
							}
						}

						if(symbol_found->properties & SYMBOL_PROPERTY_CONST){
							unresolved_instruction->value_op2=symbol_found->ref_ptr;
						}else{
							unresolved_instruction->value_op2=symbol_found->idx_position;
						}
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

		return unresolved_symbols.size()==0;
	}

	void Function::addUnresolvedSymbol(zs_int instruction){
		unresolved_symbols.append(instruction);
	}

	ZetScript *Function::getZetScript(){
		return zs;
	}

	Function::~Function(){
		clear();

		for(int i=0; i < instruction_source_infos.size(); i++){
			InstructionSourceInfo *isi=(InstructionSourceInfo *)instruction_source_infos.get(i);
			if(isi != NULL){
				delete isi;
			}
		}

		delete local_variables;
		local_variables=NULL;
	}

}
