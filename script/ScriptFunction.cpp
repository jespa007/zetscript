#include "zetscript.h"

#define FORMAT_PRINT_INSTRUCTION "%04i"

namespace zetscript{

	ScriptFunction::ScriptFunction(
			ZetScript * _zs
			,ClassTypeIdx _idx_class
			,short _idx_script_function
			, std::vector<FunctionParam> _params
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

		//num_native_functions=new zs_map;
		//function_should_be_deduced_at_runtime=false;
	}

	const char * ScriptFunction::instructionPropertyPreOperationToStr(unsigned int properties){
		switch(GET_MSK_INSTRUCTION_PROPERTY_PRE_POST_OP(properties)){
			case MSK_INSTRUCTION_PROPERTY_PRE_NEG_OR_NOT:
				return "-";
			case MSK_INSTRUCTION_PROPERTY_PRE_INC:
				return "++";
			case MSK_INSTRUCTION_PROPERTY_PRE_DEC:
				return "--";
			default:
				break;
		}

		return "";
	}

	const char * ScriptFunction::instructionPropertyPostOperationToStr(unsigned int properties){
		switch(GET_MSK_INSTRUCTION_PROPERTY_PRE_POST_OP(properties)){
			case MSK_INSTRUCTION_PROPERTY_POST_INC:
					return "++";
				case MSK_INSTRUCTION_PROPERTY_POST_DEC:
					return "--";
				default:
					break;
		}

		return "";
	}

	InstructionSourceInfo * ScriptFunction::getInstructionInfo(Instruction *instruction){
		short idx= (instruction-this->instructions);///sizeof(Instruction *);
		if(instruction_source_info.count(idx)==1){
			return &instruction_source_info[idx];
		}
		return NULL;
	}

	 void ScriptFunction::printGeneratedCode(ScriptFunction *sfo,ScriptClass *sc){

		// PRE: it should printed after compile and updateReferences.
		// first print functions  ...
		zs_vector * m_vf = sfo->registered_symbols;
		ConstantValue *icv=NULL;

		if(sfo->symbol.properties & SYMBOL_PROPERTY_C_OBJECT_REF){ // c functions has no script instructions
			return;
		}

		std::string symbol_ref="????";
		std::string class_str="";
		std::string symbol_value="";

		if(sc==NULL){ // no class is a function on a global scope
			symbol_ref=sfo->symbol.name;
		}else{ // is a class
			symbol_ref=sfo->symbol.name;//+std::string("::")+std::string("????");
			class_str=sc->symbol_class.name+"::";
		}


		printf("-------------------------------------------------------\n");
		printf("\nCode for function \"%s%s\"\n\n",class_str.c_str(),symbol_ref.c_str());

		unsigned idx_instruction=0;
		for(Instruction * instruction=sfo->instructions; instruction->byte_code!= BYTE_CODE_END_FUNCTION; instruction++,idx_instruction++){

			int n_ops=0;
			//const char *start_expression=(instruction->properties & MSK_INSTRUCTION_PROPERTY_START_EXPRESSION)?"{start expression}":"";
			unsigned char value_op1 = instruction->value_op1;
			int value_op2 = instruction->value_op2;
			symbol_value=SFI_GET_SYMBOL_NAME(sfo,instruction);
			char object_access[512] = "";
			const char *pre=instructionPropertyPreOperationToStr(instruction->properties)
					 ,*post=instructionPropertyPostOperationToStr(instruction->properties);

			if((char)value_op1 != ZS_IDX_UNDEFINED){
				n_ops++;
			}

			 if(value_op2 != ZS_IDX_UNDEFINED){
				 n_ops++;
			 }

			 if(instruction->properties & MSK_INSTRUCTION_PROPERTY_ACCESS_TYPE_FIELD){
				 sprintf(object_access,
						"."
						);
			 }
			 else if(symbol_value.c_str() == SYMBOL_VALUE_SUPER){
				sprintf(object_access,"super.");
			 }
			 else if(instruction->properties & MSK_INSTRUCTION_PROPERTY_ACCESS_TYPE_THIS){
				sprintf(object_access,"this.");
			 }
			 else if(instruction->properties & MSK_INSTRUCTION_PROPERTY_ACCESS_TYPE_VECTOR){
				 sprintf(object_access,"{vector element}");
			 }

			switch(instruction->byte_code){

			case  BYTE_CODE_NEW:
				printf("[" FORMAT_PRINT_INSTRUCTION "]\t%s\t%s\n"
					,idx_instruction
					,ByteCodeToStr(instruction->byte_code)
					,instruction->value_op1!=ZS_INVALID_CLASS?GET_SCRIPT_CLASS_NAME(sfo,instruction->value_op1):"???"
				);
				break;
			case BYTE_CODE_LOAD_TYPE_CONSTANT:
			//case BYTE_CODE_LOAD_TYPE_STATIC:
				icv=(ConstantValue *)instruction->value_op2;
				switch(icv->properties & MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_TYPE_PRIMITIVES){
				case MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_BOOL:
					printf("[" FORMAT_PRINT_INSTRUCTION "]\tCONST\t%s\n",idx_instruction,(int)((zs_int)icv->stk_value)==0?"false":"true");
					break;
				case MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_ZS_INT:
					printf("[" FORMAT_PRINT_INSTRUCTION "]\tCONST\t%i\n",idx_instruction,(int)((zs_int)icv->stk_value));
					break;
				case MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_FLOAT:
					printf("[" FORMAT_PRINT_INSTRUCTION "]\tCONST\t%f\n",idx_instruction,*((float *)&icv->stk_value));
					break;
				case MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_STRING:
					printf("[" FORMAT_PRINT_INSTRUCTION "]\tCONST\t\"%s\"\n",idx_instruction,((const char *)icv->stk_value));
					break;
				case MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_FUNCTION:
					printf("[" FORMAT_PRINT_INSTRUCTION "]\tCONST FUN\t%s\n",idx_instruction,icv->toString());
					break;
				default:
					printf("[" FORMAT_PRINT_INSTRUCTION "]\tCONST VAR\t%s\n",idx_instruction,symbol_value.c_str());
					break;
				}
				break;
			case BYTE_CODE_LOAD_TYPE_FIND:
				printf("[" FORMAT_PRINT_INSTRUCTION "]\t%s\t%s%s%s%s\n"
					,idx_instruction
					,ByteCodeToStr(instruction->byte_code)
					,pre
					,object_access
					,symbol_value.c_str()
					,post
				);
				break;
			case BYTE_CODE_LOAD_TYPE_VARIABLE:
				printf("[" FORMAT_PRINT_INSTRUCTION "]\t%s\t%s%s%s%s\n"
					,idx_instruction
					,ByteCodeToStr(instruction->byte_code)
					,pre
					,object_access
					,symbol_value.c_str()
					,post
				);
				break;
			case BYTE_CODE_LOAD_TYPE_FUNCTION:
				printf("[" FORMAT_PRINT_INSTRUCTION "]\t%s\t%s%s\n"
					,idx_instruction
					,ByteCodeToStr(instruction->byte_code)
					,object_access
					,symbol_value.c_str()
				);
				break;
			case BYTE_CODE_JNT:
			case BYTE_CODE_JT:
			case BYTE_CODE_JMP:
			case BYTE_CODE_JE:
				printf("[" FORMAT_PRINT_INSTRUCTION "]\t%s\t\t%03i\n"
						,idx_instruction
						,ByteCodeToStr(instruction->byte_code)
						,(int)instruction->value_op2
						);
				break;
			case BYTE_CODE_PUSH_SCOPE:
				printf("[" FORMAT_PRINT_INSTRUCTION "]\t%s\n"
					,idx_instruction
					,ByteCodeToStr(instruction->byte_code)
				);
				break;
			case BYTE_CODE_POP_SCOPE:
				printf("[" FORMAT_PRINT_INSTRUCTION "]\t%s\n"
					,idx_instruction
					,ByteCodeToStr(instruction->byte_code)
				);
				break;
			default:
				if(n_ops==0){
					printf("[" FORMAT_PRINT_INSTRUCTION "]\t%s%s%s %s\n", // VGET CAN HAVE PRE/POST INCREMENTS
						idx_instruction
						,instructionPropertyPreOperationToStr(instruction->properties)
						,ByteCodeToStr(instruction->byte_code)
						,instructionPropertyPostOperationToStr(instruction->properties)
						,instruction->properties & MSK_INSTRUCTION_PROPERTY_POP_ONE ? "{pop one}":""
					);
				}else if(n_ops==1){
					printf("[" FORMAT_PRINT_INSTRUCTION "]\t%s%s\t%i\n"
						,idx_instruction
						,ByteCodeToStr(instruction->byte_code)
						,(instruction->properties & MSK_INSTRUCTION_PROPERTY_POP_ONE)?"_CS":""
						,instruction->value_op1
					);
				}else{ //2 ops
					printf("[" FORMAT_PRINT_INSTRUCTION "]\t%s\t%i,%i\n"
						,idx_instruction
						,ByteCodeToStr(instruction->byte_code)
						,instruction->value_op1
						,(int)instruction->value_op2
					);
				}
				break;
			}
		}
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

	/*int ScriptFunction::existArgumentName(const std::string & arg_name){
		int idx_arg=ZS_IDX_UNDEFINED;

		for(unsigned i = 0; i < this->params->count && idx_arg == ZS_IDX_UNDEFINED; i++){
			FunctionParam *function_param=(FunctionParam *)this->params->items[i];
			if(function_param->arg_name == arg_name){
				idx_arg=i;
			}
		}
		return idx_arg;
	}*/

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
			StackElement ref_stk;
			if((properties & SYMBOL_PROPERTY_C_OBJECT_REF)!=0){
				ref_stk=convertSymbolToStackElement(this->zs,symbol,(void *)ref_ptr);
				if(!zs->getVirtualMachine()->setStackElement(idx_position,ref_stk)){
					THROW_RUNTIME_ERROR(zs->getVirtualMachine()->getError().c_str());
				}
			}else{
				if((symbol->ref_ptr=(zs_int)zs->getVirtualMachine()->getStackElement(idx_position))==0){
					THROW_RUNTIME_ERROR(zs->getVirtualMachine()->getError().c_str());
				}
			}
		}



		return symbol;
	}

	Symbol * ScriptFunction::registerLocalFunction(
			 Scope * scope_block
			,const std::string & file
			, short line
			, const std::string & function_name
			, std::vector<FunctionParam> params
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
			/*idx_script_function=sf->idx_script_function;
			delete sf;
			sf = new ScriptFunction(
					zs
					,idx_class
					,idx_script_function
					,params
					,idx_return_type
					,symbol
					,ref_ptr
			);*/

			//script_function_factory->setScriptFunction(idx_script_function,sf);
			//symbol_found->ref_ptr=(zs_int)sf;
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
				zs->getVirtualMachine()->setStackElement(
					(int)idx_position
					,{
						NULL
						,(void *)symbol->ref_ptr
						,MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_FUNCTION
					}
				);
			}

			// register num symbols only for c symbols...
			if(symbol->properties & SYMBOL_PROPERTY_C_OBJECT_REF){
				Symbol *symbol_repeat=NULL;
				if((symbol_repeat=getSymbol(
					scope_block
					,symbol->name
					,(char)params.size()))!=NULL){ // there's one or more name with same args --> mark
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

	void ScriptFunction::updateParams(std::vector<FunctionParam> _params){
		// delete existing args...
		for(unsigned i=0; i < params->count; i++){
			delete (FunctionParam *)params->items[i];
		}

		params->clear();

		// insert new args...
		for(unsigned i = 0; i < _params.size(); i++){
			FunctionParam *script_param = new FunctionParam();
			*script_param=_params[i];
			params->push_back((zs_int)script_param);
		}
	}

	void ScriptFunction::clear(){
		// delete symbols refs from scope...
		registered_symbols->clear();

		// delete arg info variables...
		for(unsigned i=0; i < params->count; i++){
			delete (FunctionParam *)params->items[i];
		}

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


		//delete num_native_functions;
	}

}
