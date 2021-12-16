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
	((properties) & INSTRUCTION_PROPERTY_ILOAD_R_ACCESS_THIS_VAR) ? ((Symbol *)sc->class_scope->symbol_variables->items[value])->name.c_str()\
	:((Symbol *)sfo->local_variables->items[value])->name.c_str()\

namespace zetscript{

	ScriptFunction::ScriptFunction(
			ZetScript * _zs
			,Scope *_scope_function
			,int _idx_script_function
			,int _idx_class
			,int _idx_position
			,const zs_string & _function_name
			, ScriptFunctionParam **_params
			, size_t _params_len
			,int _idx_return_type
			, zs_int _ref_native_function_ptr
			,uint16_t _properties
		) {
		// function data...
		idx_class=_idx_class;
		function_name=_function_name;
		idx_script_function=_idx_script_function;
		idx_position = _idx_position;
		idx_return_type = _idx_return_type;
		ref_native_function_ptr=_ref_native_function_ptr;
		properties = _properties;

		instructions=NULL;
		instructions_len = 0;


		// local symbols for class or function...
		local_variables=new zs_vector();
		params = NULL;//new zs_vector();
		params_len = 0;

		updateParams(_params,_params_len);

		// factories
		zs = _zs;
		scope_factory = zs->getScopeFactory();
		script_function_factory= zs->getScriptFunctionFactory();
		script_class_factory=zs->getScriptClassFactory();

		min_stack_needed=0;

	}

	InstructionSourceInfo * ScriptFunction::getInstructionInfo(Instruction *instruction){
		short idx= (instruction-this->instructions);///sizeof(Instruction *);
		//if(instruction_source_info.items[idx]==1){
		return (InstructionSourceInfo *)instruction_source_info.items[idx];
	}

	 void ScriptFunction::printGeneratedCode(ScriptFunction *sfo,ScriptClass *sc){

		// PRE: it should printed after compile and updateReferences.
		// first print functions  ...
		zs_vector * m_vf = sfo->local_variables;
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

		if(sc==NULL){ // no class is a function on a global scope
			symbol_ref=sfo->function_name;
		}else{ // is a class
			symbol_ref=sfo->function_name;//+zs_string("::")+zs_string("????");
			class_str=sc->class_name+"::";
		}



		printf("______________________________________________________________\n\n");
		printf(" Function: '%s%s' (Required stack: %i)				  		  \n\n",class_str.c_str(),symbol_ref.c_str(),sfo->min_stack_needed);
		printf(" NUM |RS|AS|          INSTRUCTION                             \n");
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
					|| instruction->byte_code==ByteCode::BYTE_CODE_UNRESOLVED_THIS_CALL
					|| instruction->byte_code==ByteCode::BYTE_CODE_THIS_MEMBER_CALL
					|| instruction->byte_code==ByteCode::BYTE_CODE_THIS_CALL
					|| instruction->byte_code==ByteCode::BYTE_CODE_PUSH_STK_THIS_VARIABLE
			)){

				// ... we create new instruction
				 symbol_value="this."+symbol_value;
			 }else if(   instruction->byte_code==ByteCode:: BYTE_CODE_INDIRECT_GLOBAL_CALL
					 || instruction->byte_code== BYTE_CODE_INDIRECT_LOCAL_CALL
			 ){
				 symbol_value="@"+symbol_value;
			 }


			 req_stk=instruction_num_required_stack(instruction);
			 sum_stk_load_stk+=req_stk;
			 max_acc_stk_load=MAX(max_acc_stk_load,sum_stk_load_stk);
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

			case  BYTE_CODE_NEW_OBJECT_BY_KNOWN_TYPE:
				printf("[" FORMAT_PRINT_INSTRUCTION "]\t%s\t\t%s\n"
					,idx_instruction
					,req_stk
					,sum_stk_load_stk
					,byte_code_to_str(instruction->byte_code)
					,(char)instruction->value_op1!=ZS_IDX_UNDEFINED?GET_SCRIPT_CLASS_NAME(sfo,instruction->value_op1):"???"
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
					? "\t" : "\t\t"
					,symbol_value.c_str()
				);
				break;

			case BYTE_CODE_LOAD_VECTOR_ITEM:
				printf("[" FORMAT_PRINT_INSTRUCTION "]\t%s\t%s {vector}\n"
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
				//instruction_aux=instruction;

				while((instruction+1)->byte_code == BYTE_CODE_LOAD_OBJECT_ITEM){
					instruction++;
					symbol_value+=zs_string(".")+SFI_GET_SYMBOL_NAME(sfo,instruction);
				}

				printf("[" FORMAT_PRINT_INSTRUCTION "]\t%s%s%s\n"
					,idx_instruction
					,req_stk
					,sum_stk_load_stk
					,byte_code_to_str(instruction->byte_code)
					,instruction->byte_code==BYTE_CODE_LOAD_THIS_VARIABLE
					|| instruction->byte_code == BYTE_CODE_LOAD_OBJECT_ITEM
					?"\t\t":"\t"
					,symbol_value.c_str()
				);
				break;
			case BYTE_CODE_JNT:
			case BYTE_CODE_JT:
			case BYTE_CODE_JMP:
			case BYTE_CODE_JE_CASE:
				printf("[" FORMAT_PRINT_INSTRUCTION "]\t%s\t\t\t%03i (ins%s%i)\n"
					,idx_instruction
					,req_stk
					,sum_stk_load_stk
					,byte_code_to_str(instruction->byte_code)
					,(int)((instruction-sfo->instructions)+instruction->value_op2)
					,(int)(instruction->value_op2)>=0?"+":""
					,(int)(instruction->value_op2)

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
			case BYTE_CODE_CONSTRUCTOR_CALL:
				printf("[" FORMAT_PRINT_INSTRUCTION "]\t%s\targ:%i ret:%i\n"
					,idx_instruction
					,req_stk
					,sum_stk_load_stk
					,byte_code_to_str(instruction->byte_code)
					,INSTRUCTION_GET_PARAMETER_COUNT(instruction)
					,INSTRUCTION_GET_RETURN_COUNT(instruction)
				);
				break;
			case BYTE_CODE_CALL:
			case BYTE_CODE_INDIRECT_GLOBAL_CALL:
			case BYTE_CODE_INDIRECT_LOCAL_CALL:
			case BYTE_CODE_THIS_CALL:
			case BYTE_CODE_THIS_MEMBER_CALL:
			case BYTE_CODE_UNRESOLVED_CALL:
			case BYTE_CODE_UNRESOLVED_THIS_CALL:
				printf("[" FORMAT_PRINT_INSTRUCTION "]\t%s\t\t\t%s\targ:%i ret:%i %s\n"
					,idx_instruction
					,req_stk
					,sum_stk_load_stk
					,byte_code_to_str(instruction->byte_code)
					,symbol_value.c_str()
					,INSTRUCTION_GET_PARAMETER_COUNT(instruction)
					,INSTRUCTION_GET_RETURN_COUNT(instruction)
					,instruction->properties & INSTRUCTION_PROPERTY_RESET_STACK? "[RST]":""
				);
				break;
			case BYTE_CODE_MEMBER_CALL:
				printf("[" FORMAT_PRINT_INSTRUCTION "]\t%s\t\t\targ:%i ret:%i\n"
					,idx_instruction
					,req_stk
					,sum_stk_load_stk
					,byte_code_to_str(instruction->byte_code)
					,INSTRUCTION_GET_PARAMETER_COUNT(instruction)
					,INSTRUCTION_GET_RETURN_COUNT(instruction)
				);
				break;
			case BYTE_CODE_LOAD_TYPE:
				printf("[" FORMAT_PRINT_INSTRUCTION "]\tLOAD_TYPE\t%s\n"
						,idx_instruction
						,req_stk
						,sum_stk_load_stk
						,zs->getScriptClassFactory()->getScriptClassName(instruction->value_op2));
				break;
			case BYTE_CODE_STORE:
			case BYTE_CODE_STORE_CONST:
				printf("[" FORMAT_PRINT_INSTRUCTION "]\t%s\t\t\tn:%i\n"
					,idx_instruction
					,req_stk
					,sum_stk_load_stk
					,byte_code_to_str(instruction->byte_code)
					,(int)instruction->value_op1
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
			 Scope * scope_block
			, const char *file
			, short line
			, const zs_string & symbol_name
			, uint16_t properties
	){
		Symbol * symbol=NULL;
		short idx_position=(short)local_variables->count;
		StackElement *se=NULL;


		if((symbol=scope_block->registerSymbolVariable(
				file
				,line
				, symbol_name
				, REGISTER_SCOPE_CHECK_REPEATED_SYMBOLS_CURRENT_LEVEL
		))==NULL){
				return NULL;
		}

		symbol->properties=properties;
		symbol->idx_position = idx_position;

		local_variables->push_back((zs_int)symbol);

		return symbol;
	}

	Symbol * ScriptFunction::registerLocalVariable(
			 Scope * scope_block
			, const char * file
			, short line
			, const zs_string & symbol_name
			, const zs_string & str_native_type
			, zs_int ref_ptr
			, unsigned short properties
	){
		//ScopeSymbolInfo *irs=new ScopeSymbolInfo;

		Symbol * symbol=NULL;
		short idx_position=(short)local_variables->count;
		StackElement *se=NULL;

		if((symbol=scope_block->registerSymbolVariable(file,line, symbol_name ))==NULL){
				return NULL;
		}

		symbol->ref_ptr =ref_ptr;
		//scope_symbol->symbol=scope_symbol;
		symbol->str_native_type = str_native_type;
		symbol->properties = properties;
		symbol->idx_position = idx_position;

		local_variables->push_back((zs_int)symbol);

		return symbol;
	}

	Symbol * ScriptFunction::registerLocalFunction(
			 Scope * _scope_block
			,const char *_file
			, short _line
			, const zs_string & _function_name
			, ScriptFunctionParam **_params
			,size_t _params_len
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
					,function_name.c_str()
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
				,idx_class 				// idx class is the same which this function belongs to...
				,_function_name
				,_params
				,_params_len
				,_idx_return_type
				,_ref_ptr
				,_function_properties
		);

		// register num symbols only for c symbols...
		if((_function_properties & FUNCTION_PROPERTY_C_OBJECT_REF != 0) && symbol_repeat!=NULL){
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
		params_len = _params_len;

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

	bool ScriptFunction::linkUnresolvedSymbols(){
		ScriptClass *sc_found=NULL;
		if(unresolved_symbols.count > 0){

			const char *str_aux=NULL;
			unsigned i=0;
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


				if((idx_sc_found= script_class_factory->getBuiltinTypeOrClass(ptr_str_symbol_to_find))!= ZS_IDX_UNDEFINED){ // check if class
					unresolved_instruction->byte_code=BYTE_CODE_LOAD_TYPE;
					unresolved_instruction->value_op2=idx_sc_found;
				 }else if((str_aux=strstr(ptr_str_symbol_to_find,"::")) != NULL){ // static
					 zs_string static_error;
					char copy_aux[512]={0};

					// get class
					strncpy(copy_aux,ptr_str_symbol_to_find,str_aux-ptr_str_symbol_to_find);

					sc_found=zs->getScriptClassFactory()->getScriptClass(copy_aux);

					if(sc_found!=NULL){
						// advance ::
						str_aux+=2;

						//get member
						strcpy(copy_aux,str_aux);

						symbol_found=sc_found->getSymbol(copy_aux); // ... and member as well we can define the instruction here
					}
				}else if(unresolved_instruction->byte_code==BYTE_CODE_UNRESOLVED_THIS_CALL){ // try get global symbol
					ScriptClass *this_class=zs->getScriptClassFactory()->getScriptClass(this->idx_class);
					symbol_found=this_class->getSymbolMemberFunction(ptr_str_symbol_to_find);
				}else{
					symbol_found = MAIN_SCOPE(this)->getSymbol(ptr_str_symbol_to_find,NO_PARAMS_SYMBOL_ONLY,REGISTER_SCOPE_CHECK_REPEATED_SYMBOLS_DOWN);
				}


				if(symbol_found !=NULL){ // symbol found

					if(symbol_found->properties & SYMBOL_PROPERTY_FUNCTION){

						if(unresolved_instruction->byte_code==BYTE_CODE_UNRESOLVED_THIS_CALL){
							unresolved_instruction->byte_code=BYTE_CODE_THIS_CALL;
						}else{
							unresolved_instruction->byte_code=BYTE_CODE_CALL;
						}
						unresolved_instruction->value_op2=(zs_int)symbol_found; // store script function
					}else{ // global variable
						if(unresolved_instruction->byte_code==BYTE_CODE_UNRESOLVED_THIS_CALL){ // an indirect
							unresolved_instruction->byte_code=BYTE_CODE_THIS_MEMBER_CALL;
							unresolved_instruction->value_op2=symbol_found->idx_position;
						}else if(unresolved_instruction->byte_code == BYTE_CODE_UNRESOLVED_CALL){
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
					if(unresolved_instruction->byte_code==BYTE_CODE_UNRESOLVED_THIS_CALL){ // it has to solve the symbol
							unresolved_instruction->byte_code=BYTE_CODE_THIS_MEMBER_CALL;
					}
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

	ScriptFunction::~ScriptFunction(){
		clear();

		for(unsigned i=0; i < instruction_source_info.count; i++){
			InstructionSourceInfo *isi=(InstructionSourceInfo *)instruction_source_info.items[i];
			if(isi != NULL){
				delete isi;
			}
		}

		delete local_variables;
		local_variables=NULL;
	}

}
