#include "zetscript.h"

#define FORMAT_PRINT_INSTRUCTION "%04i"

namespace zetscript{

	ScriptFunction::ScriptFunction(
			ZetScript * _zs
			,int _idx_class
			,short _idx_script_function
			, std::vector<ScriptFunctionArg> _params
			,int _idx_return_type
			,Symbol *_symbol
			, zs_int _ref_native_function_ptr
		) {
		// function data...
		idx_class=_idx_class;
		idx_script_function=_idx_script_function;
		idx_return_type = _idx_return_type;
		ref_native_function_ptr=_ref_native_function_ptr;

		instructions=NULL;

		symbol=*_symbol;

		// local symbols for class or function...
		registered_symbols=new zs_vector(); // std::vector<ScopeSymbolInfo> member variables to be copied in every new instance
		//registered_functions=new zs_vector(); // std::vector<ScriptFunction *> idx member functions (from main std::vector collection)
		params = new zs_vector();

		updateParams(_params);

		// factories
		zs = _zs;
		scope_factory = zs->getScopeFactory();
		script_function_factory= zs->getScriptFunctionFactory();
		script_class_factory=zs->getScriptClassFactory();

		min_stack_needed=0;

		//num_native_functions=new zs_map;
		//function_should_be_deduced_at_runtime=false;
	}

	InstructionSourceInfo * ScriptFunction::getInstructionInfo(Instruction *instruction){
		short idx= (instruction-this->instructions);///sizeof(Instruction *);
		if(instruction_source_info.count(idx)==1){
			return &instruction_source_info[idx];
		}
		return NULL;
	}

#define GET_ILOAD_ACCESS_TYPE_STR(properties) \
 ((properties) & MSK_INSTRUCTION_PROPERTY_ILOAD_R_ACCESS_LOCAL) ? "Local"\
:((properties) & MSK_INSTRUCTION_PROPERTY_ILOAD_R_ACCESS_THIS_MEMBER) ? "This"\
:"Global"\


#define GET_ILOAD_R_STR(properties,value) \
	 ((properties) & MSK_INSTRUCTION_PROPERTY_ILOAD_R_ACCESS_LOCAL) ? ((Symbol *)sfo->registered_symbols->items[value])->name.c_str()\
	:((properties) & MSK_INSTRUCTION_PROPERTY_ILOAD_R_ACCESS_THIS_MEMBER) ? ((Symbol *)sc->symbol_members->items[value])->name.c_str()\
	:((Symbol *)MAIN_FUNCTION(sfo)->registered_symbols->items[value])->name.c_str()\


	 void ScriptFunction::printGeneratedCode(ScriptFunction *sfo,ScriptClass *sc){

		// PRE: it should printed after compile and updateReferences.
		// first print functions  ...
		zs_vector * m_vf = sfo->registered_symbols;

		if(sfo->symbol.properties & SYMBOL_PROPERTY_C_OBJECT_REF){ // c functions has no script instructions
			return;
		}

		if(sfo->instructions == NULL){
			return;
		}

		std::string symbol_ref="????";
		std::string class_str="";
		std::string symbol_value="";
		std::string iload_info="";

		if(sc==NULL){ // no class is a function on a global scope
			symbol_ref=sfo->symbol.name;
		}else{ // is a class
			symbol_ref=sfo->symbol.name;//+std::string("::")+std::string("????");
			class_str=sc->symbol_class.name+"::";
		}



		printf("-------------------------------------------------------\n");
		printf("\nCode for function \"%s%s\"\n\n",class_str.c_str(),symbol_ref.c_str());


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
				)
				==true){

				// ... we create new instruction
				 symbol_value="this."+symbol_value;
			}

			switch(instruction->properties & MSK_INSTRUCTION_PROPERTY_ILOAD){
			default:
				break;
			case MSK_INSTRUCTION_PROPERTY_ILOAD_K: /* only perfom with one constant*/\
				 iload_info=zs_strutils::format("%s", instruction->getConstantValueOp2ToString().c_str());
				 break;
			case MSK_INSTRUCTION_PROPERTY_ILOAD_R: /* only perfom with one Register */\
				 iload_info=zs_strutils::format("%s[\"%s\"]"
					 ,GET_ILOAD_ACCESS_TYPE_STR(instruction->properties)
					 ,GET_ILOAD_R_STR(instruction->properties,instruction->value_op1)
				 );
				 break;
			case MSK_INSTRUCTION_PROPERTY_ILOAD_KR: /* perfom Konstant-Register*/\
			 	 iload_info=zs_strutils::format("%s,%s[\"%s\"]"
					 ,instruction->getConstantValueOp2ToString().c_str()
					 ,GET_ILOAD_ACCESS_TYPE_STR(instruction->properties)
					 ,GET_ILOAD_R_STR(instruction->properties,instruction->value_op1)
				 );
				break;
			case MSK_INSTRUCTION_PROPERTY_ILOAD_RK: /* perfom Register-Konstant */\
				 iload_info=zs_strutils::format("%s[\"%s\"],%s"
					 ,GET_ILOAD_ACCESS_TYPE_STR(instruction->properties)
					 ,GET_ILOAD_R_STR(instruction->properties,instruction->value_op1)
					 ,instruction->getConstantValueOp2ToString().c_str()
				 );
				break;
		   case MSK_INSTRUCTION_PROPERTY_ILOAD_RR: /* perfom Register-Register*/ \
		   	   iload_info=zs_strutils::format("%s[\"%s\"],%s[\"%s\"]"
		  			 ,GET_ILOAD_ACCESS_TYPE_STR(instruction->properties)
		  			 ,GET_ILOAD_R_STR(instruction->properties,instruction->value_op1)
		  			 ,GET_ILOAD_ACCESS_TYPE_STR(instruction->value_op2)
					 ,GET_ILOAD_R_STR(instruction->value_op2,(instruction->value_op2 & 0xff0000) >> 16)
		  		);
				break;
			}

			switch(instruction->byte_code){

			case  BYTE_CODE_NEW_CLASS:
				printf("[" FORMAT_PRINT_INSTRUCTION "]\t%s\t%s\n"
					,idx_instruction
					,byte_code_to_str(instruction->byte_code)
					,(char)instruction->value_op1!=ZS_IDX_UNDEFINED?GET_SCRIPT_CLASS_NAME(sfo,instruction->value_op1):"???"
				);
				break;
			case BYTE_CODE_LOAD_BOOL:
				printf("[" FORMAT_PRINT_INSTRUCTION "]\tLOAD_BOOL\t%s\n",idx_instruction,instruction->value_op2==0?"false":"true");
				break;
			case BYTE_CODE_LOAD_FLOAT:
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
			case BYTE_CODE_PUSH_STK_REF:
			case BYTE_CODE_PUSH_STK_THIS:
			case BYTE_CODE_PUSH_STK_MEMBER_VAR:
			case BYTE_CODE_PUSH_STK_CONSTRUCTOR:
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
				//instruction_aux=instruction;
				while((instruction+1)->byte_code == BYTE_CODE_LOAD_ELEMENT_OBJECT){
					instruction++;
					symbol_value+=std::string(".")+SFI_GET_SYMBOL_NAME(sfo,instruction);
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
			case BYTE_CODE_JE:
				printf("[" FORMAT_PRINT_INSTRUCTION "]\t%s\t\t%03i\n"
					,idx_instruction
					,byte_code_to_str(instruction->byte_code)
					,(int)((instruction-sfo->instructions)+instruction->value_op2)
				);
				break;
			case BYTE_CODE_PUSH_SCOPE:
				printf("[" FORMAT_PRINT_INSTRUCTION "]\t%s\n"
					,idx_instruction
					,byte_code_to_str(instruction->byte_code)
				);
				break;
			case BYTE_CODE_POP_SCOPE:
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
			case BYTE_CODE_CALL:
				printf("[" FORMAT_PRINT_INSTRUCTION "]\t%s\t\targ:%i ret:%s\n"
					,idx_instruction
					,byte_code_to_str(instruction->byte_code)
					,instruction->value_op1
					,instruction->value_op2==ZS_IDX_INSTRUCTION_OP2_RETURN_ALL_STACK?"all":"1"
				);
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
		return "unknown";
	}

	const char * ScriptFunction::getInstructionSourceFile(Instruction * ins){

		InstructionSourceInfo *info=getInstructionInfo(ins);

		if(info!=NULL){
			return info->file;
		}
		return "unknown";
	}

	Symbol * ScriptFunction::registerLocalArgument(
			 Scope * scope_block
			, const std::string & file
			, short line
			, const std::string & symbol_name
			, uint16_t properties
	){
		Symbol * symbol=NULL;
		short idx_position=(short)registered_symbols->count;
		StackElement *se=NULL;


		if((symbol=scope_block->registerSymbol(file,line, symbol_name, NO_PARAMS_SYMBOL_ONLY,  ScopeDirection::SCOPE_DIRECTION_CURRENT))==NULL){
				return NULL;
		}

		symbol->properties=properties;
		symbol->idx_position = idx_position;

		registered_symbols->push_back((zs_int)symbol);

		return symbol;
	}

	Symbol * ScriptFunction::registerLocalVariable(
			 Scope * scope_block
			, const std::string & file
			, short line
			, const std::string & symbol_name
			, const std::string & str_native_type
			, zs_int ref_ptr
			, unsigned short properties
	){
		//ScopeSymbolInfo *irs=new ScopeSymbolInfo;

		Symbol * symbol=NULL;
		short idx_position=(short)registered_symbols->count;
		StackElement *se=NULL;

		if((symbol=scope_block->registerSymbol(file,line, symbol_name /*,var_node*/))==NULL){
				return NULL;
		}

		symbol->ref_ptr =ref_ptr;
		//scope_symbol->symbol=scope_symbol;
		symbol->str_native_type = str_native_type;
		symbol->properties = properties;
		symbol->idx_position = idx_position;

		registered_symbols->push_back((zs_int)symbol);

		if(scope_block == MAIN_SCOPE(this)) { // is global var ...
			StackElement stk_global_var;
			if((properties & SYMBOL_PROPERTY_C_OBJECT_REF)!=0){ // native variable
				stk_global_var=convertSymbolToStackElement(this->zs,symbol,(void *)ref_ptr);
				if(!zs->getVirtualMachine()->setStackElementAt(idx_position,stk_global_var)){
					THROW_RUNTIME_ERROR(zs->getVirtualMachine()->getError().c_str());
				}
			}else{ // script variable
				StackElement *stk_global_var_ptr=zs->getVirtualMachine()->getStackElementAt(idx_position);
				if(stk_global_var_ptr==NULL){
					THROW_RUNTIME_ERROR(zs->getVirtualMachine()->getError().c_str());
				}

				// reset stack to undefined, it can have some garbage from previous operations...
				stk_global_var_ptr->setUndefined();

				symbol->ref_ptr=(zs_int)stk_global_var_ptr;

			}
		}

		return symbol;
	}

	Symbol * ScriptFunction::registerLocalFunction(
			 Scope * scope_block
			,const std::string & file
			, short line
			, const std::string & function_name
			, std::vector<ScriptFunctionArg> params
			, int idx_return_type
			,zs_int ref_ptr
			, unsigned short properties
	){
		Symbol *symbol_found=NULL,*symbol=NULL;

		if((properties & SYMBOL_PROPERTY_C_OBJECT_REF)==0){ // is script function
			//Symbol *existing_symbol;
			if((symbol_found=getSymbol(scope_block, function_name, NO_PARAMS_SYMBOL_ONLY)) != NULL){

				if(symbol_found->scope != scope_block){

					THROW_RUNTIME_ERROR("Symbol \"%s\" defined at [%s:%i] is already defined at [%s:%i]"
						,function_name.c_str()
						,zs_path::get_filename(file.c_str()).c_str()
						,line
						,zs_path::get_filename(symbol_found->file.c_str()).c_str()
						,symbol_found->line
					);
				}
			}
		}

		if(symbol_found != NULL){ // recreate function
			symbol_found->n_params=NO_PARAMS_SYMBOL_ONLY;
			ScriptFunction *sf = (ScriptFunction *)symbol_found->ref_ptr;
			sf->clear();
			sf->updateParams(params);
			symbol_found->n_params=(char)params.size();
			symbol=symbol_found;
		}
		else{
			short idx_position=(short)registered_symbols->count;

			symbol =  script_function_factory->newScriptFunction(
					//---- Register data
					 scope_block
					,file
					,line
					//---- Function data
					,idx_class 				// idx class which belongs to...
					,registered_symbols->count // idx symbol ...
					,function_name
					,params
					,idx_return_type
					,ref_ptr
					,properties
			);

			symbol->idx_position=idx_position;

			if(scope_block == MAIN_SCOPE(this)) { // global function
				// set global stk var...
				zs->getVirtualMachine()->setStackElementAt(
					(int)idx_position
					,{
						(void *)symbol->ref_ptr
						,MSK_STK_PROPERTY_FUNCTION
					}
				);
			}

			// register num symbols only for c symbols...
			if(symbol->properties & SYMBOL_PROPERTY_C_OBJECT_REF){
				Symbol *symbol_repeat=NULL;
				if((symbol_repeat=getSymbol(
					scope_block
					,symbol->name
					,(char)params.size()))!=NULL){ // there's one or more name with same args --> mark deduce at runtime
					((ScriptFunction *)symbol_repeat->ref_ptr)->symbol.properties|=SYMBOL_PROPERTY_DEDUCE_AT_RUNTIME; // mark the function found (only matters for first time)
					((ScriptFunction *)symbol->ref_ptr)->symbol.properties|=SYMBOL_PROPERTY_DEDUCE_AT_RUNTIME;
				}
			}

			registered_symbols->push_back((zs_int)symbol);
		}

		return symbol;
	}

	Symbol *	 ScriptFunction::getSymbol(Scope * scope,const std::string & symbol_name,  char n_params){

		bool only_symbol=n_params<0;
		// from last value to first to get last override function...
		for(int i = (int)(registered_symbols->count-1); i >= 0 ; i--){
			Symbol *symbol=(Symbol *)registered_symbols->items[i];
			if(symbol->name == symbol_name){
				if(only_symbol){
					return symbol;
				}else if(symbol->properties & SYMBOL_PROPERTY_FUNCTION){
					ScriptFunction *current_sf=(ScriptFunction *)symbol->ref_ptr;
					if(

					 (n_params == (int)current_sf->params->count)
					 && (scope ==  NULL?true:(scope == current_sf->symbol.scope))
					 ){
					// set first script function found...
						return symbol;
					}
				}
			}
		}
		return NULL;
	}

	void ScriptFunction::clearParams(){
		for(unsigned i=0; i < params->count; i++){
			ScriptFunctionArg * function_param=(ScriptFunctionArg *)params->items[i];
			delete function_param;
		}

		params->clear();
	}


	void ScriptFunction::updateParams(std::vector<ScriptFunctionArg> _params){
		// delete existing args...
		clearParams();

		// insert new args...
		for(unsigned i = 0; i < _params.size(); i++){
			params->push_back((zs_int)(new ScriptFunctionArg(_params[i])));
		}
	}

	void ScriptFunction::clear(){
		// delete symbols refs from scope...
		registered_symbols->clear();

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

		delete params;
		params=NULL;

		delete registered_symbols;
		registered_symbols=NULL;
	}

}
