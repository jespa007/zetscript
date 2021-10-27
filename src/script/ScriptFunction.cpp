/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
#include "zetscript.h"

#define FORMAT_PRINT_INSTRUCTION "%04i"

#define GET_ILOAD_ACCESS_TYPE_STR(properties) \
 ((properties) & INSTRUCTION_PROPERTY_ILOAD_R_ACCESS_LOCAL) ? "Local"\
:((properties) & INSTRUCTION_PROPERTY_ILOAD_R_ACCESS_THIS_MEMBER) ? "This"\
:"Global"\

#define GET_ILOAD_R_STR(properties,value) \
	 ((properties) & INSTRUCTION_PROPERTY_ILOAD_R_ACCESS_LOCAL) ? ((Symbol *)sfo->local_variables->items[value])->name.c_str()\
	:((properties) & INSTRUCTION_PROPERTY_ILOAD_R_ACCESS_THIS_MEMBER) ? ((Symbol *)sc->class_scope->symbol_variables->items[value])->name.c_str()\
	:((Symbol *)MAIN_FUNCTION(sfo)->local_variables->items[value])->name.c_str()\

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

		if(sfo->properties & FUNCTION_PROPERTY_C_OBJECT_REF){ // c functions has no script instructions
			return;
		}

		if(sfo->instructions == NULL){
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


		printf("-------------------------------------------------------\n");
		printf("\nCode for function \"%s%s\" (Required stack: %i)\n\n",class_str.c_str(),symbol_ref.c_str(),sfo->min_stack_needed);


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

			 if((	   instruction->byte_code==ByteCode::BYTE_CODE_LOAD_MEMBER_VAR
					|| instruction->byte_code==ByteCode::BYTE_CODE_LOAD_ELEMENT_THIS
					|| instruction->byte_code==ByteCode::BYTE_CODE_PUSH_STK_ELEMENT_THIS
			)){

				// ... we create new instruction
				 symbol_value="this."+symbol_value;
			}

			switch(instruction->properties & INSTRUCTION_PROPERTY_ILOAD){
			default:
				break;
			case INSTRUCTION_PROPERTY_ILOAD_K: /* only perfom with one constant*/\
				 iload_info=zs_strutils::format("%s", instruction->getConstantValueOp2ToString().c_str());
				 break;
			case INSTRUCTION_PROPERTY_ILOAD_R: /* only perfom with one Register */\
				 iload_info=zs_strutils::format("%s[\"%s\"]"
					 ,GET_ILOAD_ACCESS_TYPE_STR(instruction->properties)
					 ,GET_ILOAD_R_STR(instruction->properties,instruction->value_op1)
				 );
				 break;
			case INSTRUCTION_PROPERTY_ILOAD_KR: /* perfom Konstant-Register*/\
			 	 iload_info=zs_strutils::format("%s,%s[\"%s\"]"
					 ,instruction->getConstantValueOp2ToString().c_str()
					 ,GET_ILOAD_ACCESS_TYPE_STR(instruction->properties)
					 ,GET_ILOAD_R_STR(instruction->properties,instruction->value_op1)
				 );
				break;
			case INSTRUCTION_PROPERTY_ILOAD_RK: /* perfom Register-Konstant */\
				 iload_info=zs_strutils::format("%s[\"%s\"],%s"
					 ,GET_ILOAD_ACCESS_TYPE_STR(instruction->properties)
					 ,GET_ILOAD_R_STR(instruction->properties,instruction->value_op1)
					 ,instruction->getConstantValueOp2ToString().c_str()
				 );
				break;
		   case INSTRUCTION_PROPERTY_ILOAD_RR: /* perfom Register-Register*/ \
		   	   iload_info=zs_strutils::format(
		   			 "%s[\"%s\"],%s[\"%s\"]"
		  			 ,GET_ILOAD_ACCESS_TYPE_STR(instruction->properties)
		  			 ,GET_ILOAD_R_STR(instruction->properties,instruction->value_op1)
		  			 ,GET_ILOAD_ACCESS_TYPE_STR(instruction->value_op2)
					 ,GET_ILOAD_R_STR(instruction->value_op2,(instruction->value_op2 & 0xff0000) >> 16)
		  		);
				break;
			}

			switch(instruction->byte_code){

			case  BYTE_CODE_NEW_OBJECT_BY_KNOWN_TYPE:
				printf("[" FORMAT_PRINT_INSTRUCTION "]\t%s\t%s\n"
					,idx_instruction
					,byte_code_to_str(instruction->byte_code)
					,(char)instruction->value_op1!=ZS_IDX_UNDEFINED?GET_SCRIPT_CLASS_NAME(sfo,instruction->value_op1):"???"
				);
				break;
			case BYTE_CODE_LOAD_BOOL:
				printf("[" FORMAT_PRINT_INSTRUCTION "]\tLOAD_BOOL\t%s\n",idx_instruction,instruction->value_op2==0?"false":"true");
				break;
			case BYTE_CODE_LOAD_ZS_FLOAT:
				printf("[" FORMAT_PRINT_INSTRUCTION "]\tLOAD_FLT\t%f\n",idx_instruction,*((zs_float *)&instruction->value_op2));
				break;
			case BYTE_CODE_LOAD_ZS_INT:
				printf("[" FORMAT_PRINT_INSTRUCTION "]\tLOAD_INT\t%i\n",idx_instruction,(int)(instruction->value_op2));
				break;
			case BYTE_CODE_LOAD_STRING:
				printf("[" FORMAT_PRINT_INSTRUCTION "]\tLOAD_STRING\t%s\n",idx_instruction,instruction->getConstantValueOp2ToString().c_str());
				break;
			case BYTE_CODE_NEW_STRING:
				printf("[" FORMAT_PRINT_INSTRUCTION "]\tNEW_STRING\t%s\n",idx_instruction,instruction->getConstantValueOp2ToString().c_str());
				break;

			case BYTE_CODE_PUSH_STK_GLOBAL:
			case BYTE_CODE_PUSH_STK_LOCAL:
			//case BYTE_CODE_PUSH_STK_REF:
			case BYTE_CODE_PUSH_STK_THIS:
			case BYTE_CODE_PUSH_STK_MEMBER_VAR:
			case BYTE_CODE_LOAD_SCRIPT_FUNCTION_CONSTRUCTOR:
			case BYTE_CODE_LOAD_FUNCTION:
			case BYTE_CODE_FIND_VARIABLE:
			case BYTE_CODE_LOAD_REF:
			case BYTE_CODE_LOAD_LOCAL:
			case BYTE_CODE_LOAD_THIS:
			case BYTE_CODE_LOAD_GLOBAL:
			case BYTE_CODE_LOAD_MEMBER_VAR:
				printf("[" FORMAT_PRINT_INSTRUCTION "]\t%s\t%s\n"
					,idx_instruction
					,byte_code_to_str(instruction->byte_code)
					,symbol_value.c_str()
				);
				break;

			case BYTE_CODE_LOAD_ELEMENT_VECTOR:
				printf("[" FORMAT_PRINT_INSTRUCTION "]\t%s\t%s {vector}\n"
					,idx_instruction
					,byte_code_to_str(instruction->byte_code)
					,symbol_value.c_str()
				);
				break;
			case BYTE_CODE_LOAD_ELEMENT_OBJECT:
			case BYTE_CODE_LOAD_ELEMENT_THIS:
			case BYTE_CODE_PUSH_STK_ELEMENT_THIS:
			case BYTE_CODE_PUSH_STK_ELEMENT_OBJECT:
				//instruction_aux=instruction;

				while((instruction+1)->byte_code == BYTE_CODE_LOAD_ELEMENT_OBJECT){
					instruction++;
					symbol_value+=zs_string(".")+SFI_GET_SYMBOL_NAME(sfo,instruction);
				}

				printf("[" FORMAT_PRINT_INSTRUCTION "]\t%s\t%s\n"
					,idx_instruction
					,byte_code_to_str(instruction->byte_code)
					,symbol_value.c_str()
				);
				break;
			case BYTE_CODE_JNT:
			case BYTE_CODE_JT:
			case BYTE_CODE_JMP:
			case BYTE_CODE_JE_CASE:
				printf("[" FORMAT_PRINT_INSTRUCTION "]\t%s\t\t%03i (ins%s%i)\n"
					,idx_instruction
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
					,byte_code_to_str(instruction->byte_code)
				);
				break;
			case BYTE_CODE_INSTANCEOF:
				printf("[" FORMAT_PRINT_INSTRUCTION "]\t%s\n"
					,idx_instruction
					,byte_code_to_str(instruction->byte_code)
				);
				break;
			case BYTE_CODE_CALL_CONSTRUCTOR:
				printf("[" FORMAT_PRINT_INSTRUCTION "]\t%s\targ:%i\n"
					,idx_instruction
					,byte_code_to_str(instruction->byte_code)
					,instruction->value_op1
				);
				break;
			case BYTE_CODE_FIND_IMMEDIATE_CALL:
			case BYTE_CODE_IMMEDIATE_CALL:
				printf("[" FORMAT_PRINT_INSTRUCTION "]\t%s\t\t%s\targ:%i ret:%s\n"
					,idx_instruction
					,byte_code_to_str(instruction->byte_code)
					,symbol_value.c_str()
					,instruction->value_op1
					,instruction->properties&INSTRUCTION_PROPERTY_RETURN_ALL_STACK?"all":"1"
				);
				break;
			case BYTE_CODE_CALL:
				printf("[" FORMAT_PRINT_INSTRUCTION "]\t%s\t\targ:%i ret:%s\n"
					,idx_instruction
					,byte_code_to_str(instruction->byte_code)
					,instruction->value_op1
					,instruction->properties&INSTRUCTION_PROPERTY_RETURN_ALL_STACK?"all":"1"
				);
				break;
			case BYTE_CODE_LOAD_TYPE:
				printf("[" FORMAT_PRINT_INSTRUCTION "]\tLOAD_TYPE\t%s\n",idx_instruction,zs->getScriptClassFactory()->getScriptClassName(instruction->value_op2));
				break;
			default:

				if(iload_info != ""){
					printf("[" FORMAT_PRINT_INSTRUCTION "]\t%s\t\t%s\n", // VGET CAN HAVE PRE/POST INCREMENTS
						idx_instruction
						,byte_code_to_str(instruction->byte_code)
						,iload_info.c_str()
					);
				}else{
					if(n_ops==0){
						printf("[" FORMAT_PRINT_INSTRUCTION "]\t%s\n", // VGET CAN HAVE PRE/POST INCREMENTS
							idx_instruction
							,byte_code_to_str(instruction->byte_code)
						);
					}else if(n_ops==1){
						printf("[" FORMAT_PRINT_INSTRUCTION "]\t%s\t\t%i\n"
							,idx_instruction
							,byte_code_to_str(instruction->byte_code)
							,instruction->value_op1
						);
					}else{ //2 ops
						printf("[" FORMAT_PRINT_INSTRUCTION "]\t%s\t\t%i,%i\n"
							,idx_instruction
							,byte_code_to_str(instruction->byte_code)
							,instruction->value_op1
							,(int)instruction->value_op2
						);
					}
				}
				break;
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
				THROW_RUNTIME_ERROR("Symbol \"%s\" defined at %s is already defined at %s"
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
		// delete existing args...
		clearParams();
		params=*_params;
		params_len = _params_len;

		// mark ptr that was assigned. Now ScriptFunction is in charge to deallocate this variable
		*_params=NULL;
	}

	void ScriptFunction::clear(){
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

	ScriptFunction::~ScriptFunction(){
		clear();

		for(unsigned i=0; i < instruction_source_info.count; i++){
			delete (InstructionSourceInfo *)instruction_source_info.items[i];
		}

		delete local_variables;
		local_variables=NULL;
	}

}
