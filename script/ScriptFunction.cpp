#include "zetscript.h"

#define FORMAT_PRINT_INSTRUCTION "%04i"

namespace zetscript{

	ScriptFunction::ScriptFunction(
			ZetScript * _zs
			,unsigned char _idx_class
			,short _idx_script_function
			, std::vector<FunctionParam> _function_params
			,int _idx_return_type
			,Symbol *_symbol
		) {
		// function data...
		idx_class=_idx_class;
		idx_script_function=_idx_script_function;
		idx_return_type = _idx_return_type;

		instructions=NULL;

		symbol=*_symbol;

		// local symbols for class or function...
		registered_symbols=new zs_vector(); // std::vector<ScopeSymbolInfo> member variables to be copied in every new instance
		registered_functions=new zs_vector(); // std::vector<ScriptFunction *> idx member functions (from main std::vector collection)
		function_params = new zs_vector();
		for(unsigned i = 0; i < _function_params.size(); i++){
			FunctionParam *script_param = new FunctionParam();
			*script_param=_function_params[i];
			function_params->push_back((intptr_t)script_param);
		}

		// factories
		zs = _zs;
		scope_factory = zs->getScopeFactory();
		script_function_factory= zs->getScriptFunctionFactory();
		script_class_factory=zs->getScriptClassFactory();
	}

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
		zs_vector * m_vf = sfo->registered_functions;

		for(unsigned j =0; j < m_vf->count; j++){

			ScriptFunction *local_irfs = (ScriptFunction *)m_vf->items[j];

			if(( local_irfs->symbol.symbol_properties & SYMBOL_PROPERTY_C_OBJECT_REF) != SYMBOL_PROPERTY_C_OBJECT_REF){
				printGeneratedCode((ScriptFunction *)m_vf->items[j]);
			}
		}

		if(sfo->symbol.symbol_properties & SYMBOL_PROPERTY_C_OBJECT_REF){ // c functions has no script instructions
			return;
		}

		std::string symbol_ref="????";

		//strcpy(symbol_ref,AST_SYMBOL_VALUEZJ_CONST_CHAR(local_irfs->symbol_info.idxAstNode));

		if(sfo->idx_class!=ZS_INVALID_CLASS){
			ScriptClass *sc = GET_SCRIPT_CLASS(sfo,sfo->idx_class);
			if(sc->idx_class == ZS_IDX_UNDEFINED){ // no class is a function on a global scope
				symbol_ref=sfo->symbol.name;
			}else{
				symbol_ref=sfo->symbol.name+std::string("::")+std::string("????");
			}
		}


		printf("-------------------------------------------------------\n");
		printf("\nCode for function \"%s\"\n\n",symbol_ref.c_str());


		/*if(sfo->instruction==NULL){
			ZS_PRINT_WARNING("No instructions for function \"%s\"",sfo->symbol_info.symbol->name);
			return;
		}*/

		unsigned idx_instruction=0;
		for(Instruction * instruction=sfo->instructions; instruction->byte_code!= BYTE_CODE_END_FUNCTION; instruction++,idx_instruction++){

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
				case MSK_INSTRUCTION_PROPERTY_PRE_NEG_OR_NOT:
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
						formatInstructionLoadType(sfo,(Instruction *)sfo->instructions,idx_instruction).c_str(),
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

		for(unsigned i = 0; i < this->function_params->count && idx_arg == ZS_IDX_UNDEFINED; i++){
			FunctionParam *function_param=(FunctionParam *)this->function_params->items[i];
			if(function_param->arg_name == arg_name){
				idx_arg=i;
			}
		}

		return idx_arg;

	}

	Symbol * ScriptFunction::addSymbol(
			 Scope * scope_block
			, const std::string & file
			, short line
			, const std::string & symbol_name
			, const std::string & c_type
			, intptr_t ref_ptr
			, unsigned short symbol_properties
	){
		//ScopeSymbolInfo *irs=new ScopeSymbolInfo;

		Symbol * symbol=NULL;
		short idx_position=(short)registered_symbols->count;

		if((symbol=scope_block->registerSymbol(file,line, symbol_name /*,var_node*/))==NULL){
				return NULL;
		}


		/*if(getVariable(scope_symbol->name,scope_symbol->scope) != NULL){
			THROW_RUNTIME_ERROR(zs_strutils::format("Variable \"%s\" already exist",variable_name.c_str()));
			return NULL;
		}*/

		symbol->ref_ptr =ref_ptr;
		//scope_symbol->symbol=scope_symbol;
		symbol->c_type = c_type;
		symbol->symbol_properties = symbol_properties;

		symbol->idx_position = idx_position;

		registered_symbols->push_back((intptr_t)symbol);

		return symbol;
	}

	/*ScopeSymbolInfo *	ScriptFunction::registerVariable(
			const std::string & file
			, short line
			, const std::string & variable_name
			, const std::string & c_type
			, intptr_t ref_ptr
			, unsigned short symbol_properties)
	{
			return registerVariable(
					file
					,line
					,this->symbol_info.symbol->idx_scope
					,  variable_name
					,  c_type
					,  ref_ptr
					,   symbol_properties
			);
	}*/

	Symbol *	 ScriptFunction::getSymbol(Scope *scope,const std::string & symbol_name){

		if(registered_symbols->count>0){

			// from lat value to first to get last override function...
			for(int i = (int)registered_symbols->count-1; i >= 0 ; i--){
				Symbol *symbol=(Symbol *)registered_symbols->items[i];
				if((symbol->name == symbol_name)
				&& (scope ==  NULL?true:(scope == symbol->scope))
				  ){
					return symbol;
				}
			}
		}

		return NULL;
	}



	ScriptFunction * ScriptFunction::addFunction(
			 Scope * scope_block
			,const std::string & file
			, short line
			, const std::string & function_name
			, std::vector<FunctionParam> args
			, int idx_return_type
			,intptr_t ref_ptr
			, unsigned short symbol_properties
		){

			if(getFunction(scope_block,function_name,(char)args.size()) != NULL){
				THROW_RUNTIME_ERROR(zs_strutils::format("Function \"%s\" already exist",function_name.c_str()));
				return NULL;
			}

			ScriptFunction *sf =  script_function_factory->newScriptFunction(
					//---- Register data
					 scope_block
					,file
					,line
					//---- Function data
					,idx_class 				// idx class which belongs to...
					,registered_functions->count // idx symbol ...
					,function_name
					,args
					,idx_return_type
					,ref_ptr
					,symbol_properties
			);

			registered_functions->push_back((intptr_t)sf);

			return sf;
	}

	/*ScriptFunction * ScriptFunction::registerFunction(const std::string & file, short line, const std::string & function_name, std::vector<FunctionParam> args, int idx_return_type,intptr_t ref_ptr, unsigned short symbol_properties){

		return registerFunction(file, line,this->symbol_info.symbol->idx_scope, function_name,  args, idx_return_type,ref_ptr, symbol_properties);
	}
*/
	ScriptFunction *	 ScriptFunction::getFunction(Scope * scope,const std::string & function_name,  char n_args){

		if(registered_functions->count>0){

			// from last value to first to get last override function...
			for(unsigned i = (int)(registered_functions->count-1); i >= 0 ; i--){
				ScriptFunction *sf=(ScriptFunction *)registered_functions->items[i];
				if(
						(sf->symbol.name == function_name)
					 && (n_args == (int)sf->function_params->count)
					 && (scope ==  NULL?true:(scope == sf->symbol.scope))
					 ){

					return sf;
				}
			}
		}

		return NULL;
	}


	ScriptFunction::~ScriptFunction(){

		// delete local functions...
		registered_functions->free_all_items_and_clear();
		delete registered_functions;

		// delete local variables...
		for(unsigned i=0; i < registered_symbols->count; i++){
			delete (Symbol *)registered_symbols->items[i];
		}
		delete registered_symbols;

		// delete arg info variables...
		for(unsigned i=0; i < function_params->count; i++){
			delete (FunctionParam *)function_params->items[i];
		}
		delete function_params;

		// delete arg info variables...
		free(instructions);
	}

}
