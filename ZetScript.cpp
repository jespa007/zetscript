#include "ZetScript.h"

#define FORMAT_PRINT_INSTRUCTION "%04i"

#include "ZetScript_eval.cpp"

namespace zetscript{

	// define prototype ...
	int error_line;
	char error_filename[512];
	char error_description[1024];
	const char *error_type="NA";

	void  WriteError(const char *filename, int line, const  char  *input_text, ...){

		char output_text[4096];

		va_list  ap;
		va_start(ap,  input_text);
		vsprintf(output_text,  input_text,  ap);
		va_end(ap);


		memset(error_filename,0,sizeof(error_filename));

#ifdef __DEBUG__
		fprintf(stderr,"[ERR %s:%i] %s",filename,line,output_text);
#endif

		if(filename){
			strcpy(error_filename,filename);
		}

		error_line=line;
		strcpy(error_description,output_text);
	}

	int GetErrorLine(){
		return error_line;
	}

	const char * GetErrorDescription(){
		return error_description;
	}

	const char * GetErrorSourceFilename(){
		return error_filename;
	}

	ZetScript::ZetScript(){

		scope_factory = new ScopeFactory(this);
		native_function_factory = new NativeFunctionFactory(this);
		script_function_factory= new ScriptFunctionFactory(this);
		eval_obj = new ScriptEval(this);
		virtual_machine = new VirtualMachine(this);
		script_class_factory = new ScriptClassFactory(this);

		eval_int=0;
		eval_float=0;
		eval_string="";
		eval_bool = false;

		zs=this;

		//----
		//conversion_types = new std::map<int, std::map<int, ConversionType>>();

		str_void_type = typeid(void).name();
		str_int_type_ptr = typeid(int *).name();
		str_float_type_ptr = typeid(float *).name();
		str_const_char_type_ptr = typeid(const char *).name();
		str_string_type_ptr = typeid(std::string *).name();
		str_bool_type_ptr = typeid(bool *).name();

		// particular case for return ... for not to force user returning pointer (this is the only vars allowed, sorry!)
		str_bool_type =  typeid(bool).name();
		str_int_type =  typeid(int).name();
		str_unsigned_int_type =  typeid(unsigned int).name();
		str_intptr_t_type =  typeid(intptr_t).name();
		str_float_type =  typeid(float).name();
		str_stack_element_type=  typeid(StackElement *).name();



	}

	//----------------------------------------------------------------------------------------------------------------------------------------------------------------
	 // PRINT ASM INFO

	 std::string ZetScript::getStrTypeLoadValue(ScriptFunction *current_function,PtrInstruction list_statements, int current_instruction){

		 char print_aux_load_value[512] = {0};
		 char object_access[512] = "";

		 Instruction * instruction =&list_statements[current_instruction];
		 ConstantValueInfo *icv;
		 std::string symbol_value=INSTRUCTION_GET_SYMBOL_NAME(current_function,instruction);
		 if(instruction->byte_code_op != BYTE_CODE_LOAD){
			 return "ERROR";
		 }

		 sprintf(print_aux_load_value,"UNDEFINED");

		 if(instruction->properties & MSK_INSTRUCTION_PROPERTY_SCOPE_TYPE_ACCESS){

			 sprintf(object_access,
					"[" FORMAT_PRINT_INSTRUCTION "]."

					,(int)instruction->op2_value);
		 }
		 else if(instruction->properties & MSK_INSTRUCTION_PROPERTY_SCOPE_TYPE_THIS){
			sprintf(object_access,"this.");
		 }

		 switch(instruction->op1_value){

			case LoadType::LOAD_TYPE_CONSTANT:
				icv=(ConstantValueInfo *)instruction->op2_value;
				switch(icv->properties){
				case MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_BOOLEAN:
				case MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_INTEGER:
					sprintf(print_aux_load_value,"CONST(%i)",(int)((intptr_t)icv->stk_value));
					break;
				case MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_FLOAT:
					sprintf(print_aux_load_value,"CONST(%f)",*((float *)&icv->stk_value));
					break;
				case MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_STRING:
					sprintf(print_aux_load_value,"CONST(%s)",((const char *)icv->stk_value));
					break;

				}
				break;

			case LoadType::LOAD_TYPE_VARIABLE:
				sprintf(print_aux_load_value,"%sVAR(%s)",object_access,symbol_value.c_str());
				break;
			case LoadType::LOAD_TYPE_FUNCTION:

				sprintf(print_aux_load_value,"%sFUN(%s)",object_access,symbol_value.c_str());
				break;

			case LoadType::LOAD_TYPE_ARGUMENT:
				sprintf(print_aux_load_value,"ARG(%s)",symbol_value.c_str());
				break;
			default:

				break;
		}
		return print_aux_load_value;
	 }

	 void ZetScript::PrintGeneratedCode(){

		 std::vector<ScriptClass *> *script_classes=script_class_factory->getVectorScriptClassNode();
		 // for all classes print code...
		 for(unsigned i = 0; i < script_classes->size(); i++){
			 ScriptClass *rc=script_classes->at(i);
			 for(unsigned f = 0; f < rc->local_function.size(); f++){
				 PrintGeneratedCode(rc->local_function[f]);
			 }
		 }
	 }
	 // PRINT ASM INFO
	 //----------------------------------------------------------------------------------------------------------------------------------------------------------------



	//--------------------------------
	 // FILE MANAGEMENT


	bool ZetScript::IsFilenameAlreadyParsed(const std::string & filename){
		for(unsigned i = 0; i < m_parsedSource.size(); i++){
			if(m_parsedSource.at(i).filename==filename){
				return true;
			}
		}
		return false;
	}

	const char * ZetScript::GetParsedFilenameFromIdx(unsigned idx){

		if(idx >= m_parsedSource.size()){
			THROW_RUNTIME_ERROR("out of bounds");
			return DEFAULT_NO_FILENAME;
		}

		return m_parsedSource.at(idx).filename.c_str();
	}

	//-----------------------------------------------------------------------------------------------------------------------------------------
	//
	// CONSTANT MANAGEMENT

	ConstantValueInfo *ZetScript::getRegisteredConstantValue(const std::string & const_name){

		if((m_contantPool).count(const_name) == 1){
			return (m_contantPool)[const_name];
		}
		return NULL;
	}

	ConstantValueInfo * ZetScript::registerConstantValue(const std::string & const_name, void *obj, unsigned short properties){

		ConstantValueInfo * info_ptr=NULL;

		if(getRegisteredConstantValue(const_name) == NULL){
			info_ptr=new ConstantValueInfo;
			*info_ptr={obj,NULL,properties};
			(m_contantPool)[const_name]=info_ptr;
		}else{
			THROW_RUNTIME_ERROR(zs_strutils::Format("internal:constant %s already exist",const_name.c_str()));
		}

		return info_ptr;
	}

	ConstantValueInfo * ZetScript::registerConstantIntValue(const std::string & const_name, int _value){
		intptr_t value = _value;
		unsigned short type=MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_INTEGER;
		StackElement *stk;

		if((stk = getRegisteredConstantValue(const_name))!=NULL){
			return stk;
		}

		return registerConstantValue(const_name,(void *)value,type);

	}

	// CONSTANT MANAGEMENT
	//
	//-----------------------------------------------------------------------------------------------------------------------------------------







	/*void  ZetScript::InternalPrintError(const char *s){
		 virtual_machine->SetError(s);
	}*/


	void ZetScript::SetPrintOutCallback(void (* _printout_callback)(const char *)){
		print_out_callback=_printout_callback;
	}

	void (* ZetScript::print_out_callback)(const char *) = NULL;



	void ZetScript::clear(){

		virtual_machine->ClearGlobalVars();

		ScriptFunction * main_function = this->script_function_factory->GetScriptFunction(IDX_MAIN_FUNCTION);

		// clean main functions ... Remove script functions and leave c functions...
		for (unsigned f = 0;
			f < main_function->local_function.size()
			;) {
			// get function info
			ScriptFunction * local_function = main_function->local_function[f];

			if ((local_function->symbol_info.symbol_info_properties & SYMBOL_INFO_PROPERTY_C_OBJECT_REF) != SYMBOL_INFO_PROPERTY_C_OBJECT_REF) {
				main_function->local_function.erase(main_function->local_function.begin() + f);
			}
			else {
				f++;
			}

		}

		// Remove variables except c variables ...
		for (unsigned v = 0;
			v < main_function->local_variable.size(); ) {

			if ((main_function->local_variable[v].symbol_info_properties & SYMBOL_INFO_PROPERTY_C_OBJECT_REF) != SYMBOL_INFO_PROPERTY_C_OBJECT_REF) {

				main_function->local_variable.erase(main_function->local_variable.begin() + v);

			}
			else {
				v++;
			}
		}

		// Remove scope vars...
		scope_factory->clear();
		script_function_factory->clear();
		script_class_factory->clear();
	}

	int * ZetScript::EvalIntValue(const std::string & str_to_eval){


		if(!eval(str_to_eval.c_str())){
			return NULL;
		}

		StackElement *se=virtual_machine->GetLastStackValue();

		if(se != NULL){

			if(se->properties & MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_INTEGER){

				eval_int=(int)((intptr_t)se->stk_value);
				return &eval_int;
			}
			else{
				THROW_RUNTIME_ERROR(zs_strutils::Format("EvalIntValue(...): Error evaluating \"%s\". Property:0x%X",str_to_eval.c_str(),se->properties));
			}
		}

		return NULL;


	}

	bool * ZetScript::EvalBoolValue(const std::string & str_to_eval){

		if(!eval(str_to_eval.c_str())){
			return NULL;
		}

		StackElement *se=virtual_machine->GetLastStackValue();

		if(se != NULL){

			if(se->properties & MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_BOOLEAN){
				eval_bool=(bool)((intptr_t)se->stk_value);
				return &eval_bool;

			}else{
				THROW_RUNTIME_ERROR(zs_strutils::Format("EvalBoolValue(...): Error evaluating \"%s\". Property:0x%X",str_to_eval.c_str(),se->properties));
			}
		}

		return NULL;
	}

	float * ZetScript::EvalFloatValue(const std::string & str_to_eval){

		if(!eval(str_to_eval.c_str())){
			return NULL;
		}

		StackElement *se=virtual_machine->GetLastStackValue();

		if(se != NULL){

			if(se->properties & MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_FLOAT){
				eval_float = *((float *)(&se->stk_value));
				return &eval_float;
			}
			else{
				THROW_RUNTIME_ERROR(zs_strutils::Format("EvalFloatValue(...): Error evaluating \"%s\". Property:0x%X",str_to_eval.c_str(),se->properties));

			}
		}


		return NULL;
	}

	std::string * ZetScript::EvalStringValue(const std::string & str_to_eval){


		if(!eval(str_to_eval.c_str())){
			return NULL;
		}

		StackElement *se=virtual_machine->GetLastStackValue();

		if(se != NULL){

			if(se->properties & MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_STRING){

				eval_string = ((const char *)se->stk_value);
				return &eval_string;
			}
			else{
				zs_strutils::Format("EvalStringValue(...): Error evaluating \"%s\". Property:0x%X",str_to_eval.c_str(),se->properties);
			}
		}

		return NULL;
	}


	void ZetScript::Execute(){

		bool error=false;

		// the first code to Execute is the main function that in fact is a special member function inside our main class
		virtual_machine->Execute(script_class_factory->getMainFunction(), NULL,error,NO_PARAMS);

		if(error){
			THROW_SCRIPT_ERROR();
		}
	}

	bool ZetScript::eval(const std::string & expression, bool exec_vm, bool show_bytecode, const char * filename)  {


		if(!eval_obj->eval(expression.c_str(),filename)){
			return false;
		}


		virtual_machine->buildCache();


		if(show_bytecode){
			PrintGeneratedCode();
		}

		if(exec_vm){
			Execute();
		}

		return true;
	}

	bool ZetScript::evalFile(const std::string &  filename, bool exec_vm, bool show_bytecode){
		//int idx_file=-1;
		bool error=false;
		char *buf_tmp=NULL;

		if(!IsFilenameAlreadyParsed(filename)){
			ParsedSourceInfo ps;
			ps.filename = filename;
			m_parsedSource.push_back(ps);
			//idx_file=m_parsedSource.size()-1;
			int n_bytes;

			if((buf_tmp=zs_io::ReadFile(filename, n_bytes))!=NULL){
				try{
					eval(buf_tmp, exec_vm, show_bytecode,filename.c_str());
				}
				catch(exception::ScriptExceptionError & e){
					free(buf_tmp);
					buf_tmp=NULL;
					THROW_EXCEPTION(e);
					error=true;
				}
			}

		}else{
			// already parsed
			THROW_RUNTIME_ERROR(zs_strutils::Format("Filename \"%s\" already parsed",filename));
			error=true;
		}

		if(buf_tmp!=NULL){
			free(buf_tmp);
		}

		return !error;
	}

	bool ZetScript::GetScriptObject(const std::string &function_access,ScriptVar **calling_obj,ScriptFunction **fun_obj ){

		//ZS_CLEAR_ERROR_MSG();

		std::vector<std::string> access_var = zs_strutils::Split(function_access,'.');
		//ScriptFunction * main_function = main_function;

		/*if(main_function == NULL){
			zs_strutils::Format("main_function is not initialized");
			return false;
		}*/
		ScriptFunction * main_function = script_class_factory->getMainFunction();
		*calling_obj = NULL;
		FunctionSymbol *is=NULL;
		StackElement *se=NULL;
		*fun_obj=NULL;

		// 1. some variable in main function ...
		if(access_var.size()>1){
			for(unsigned i=0; i < access_var.size()-1; i++){

				std::string symbol_to_find=access_var[i];
				if(i==0){ // get variable through main_class.main_function (global element)
					//symbol_to_find= ScriptEval::makeSymbolRef(symbol_to_find,IDX_GLOBAL_SCOPE);
					for(unsigned j = 0; j < main_function->local_variable.size() && *calling_obj==NULL; j++){
						if(main_function->local_variable[j].symbol->name==symbol_to_find
						&& main_function->local_variable[j].symbol->idx_scope == IDX_GLOBAL_SCOPE){
							StackElement *stk = virtual_machine->GetStackElement(j); // main_function->object_info.local_symbols.variable[j].
							if(stk!=NULL){
								if(stk->properties & MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_SCRIPTVAR){
									*calling_obj=(ScriptVar *)stk->var_ref;
								}
							}
							else{
								zs_strutils::Format("cannot access i (%i)",j);
								return false;
							}
						}
					}

					if((*calling_obj) == NULL){
						zs_strutils::Format("error evaluating \"%s\". Variable name \"%s\" doesn't exist",function_access.c_str(),symbol_to_find.c_str());
						return false;
					}

				}else{ // we have got the calling_obj from last iteration ...
					se = (*calling_obj)->getVariableSymbol(symbol_to_find);

					if(se!=NULL){

						if(se->properties & MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_SCRIPTVAR){
							*calling_obj=(ScriptVar *)se->var_ref;
						}else{
							zs_strutils::Format("error evaluating \"%s\". Variable name \"%s\" not script variable",function_access.c_str(),symbol_to_find.c_str());
							return false;
						}
					}
					else{
						zs_strutils::Format("error evaluating \"%s\". Variable name \"%s\" doesn't exist",function_access.c_str(),symbol_to_find.c_str());
						return false;
					}
				}
			}

			is=(*calling_obj)->getFunctionSymbol(access_var[access_var.size()-1]);
			if(is!=NULL){
				if(is->object.properties & MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_FUNCTION){
					*fun_obj=(ScriptFunction *)is->object.stk_value;
				}
			}else{

				zs_strutils::Format("error evaluating \"%s\". Cannot find function \"%s\"",function_access.c_str(),access_var[access_var.size()-1].c_str());
				return false;
			}

		}else{ // some function in main function
			//*calling_obj = m_mainObject;
			std::string symbol_to_find=access_var[0];
			for(unsigned i = 0; i < main_function->local_function.size() && *fun_obj==NULL; i++){
				ScriptFunction *aux_fun_obj=main_function->local_function[i];
				if(		aux_fun_obj->symbol_info.symbol->name  == symbol_to_find
				  && aux_fun_obj->symbol_info.symbol->idx_scope == IDX_GLOBAL_SCOPE){
					*fun_obj=aux_fun_obj;
				}
			}
		}

		if(*fun_obj==NULL){
			THROW_RUNTIME_ERROR(zs_strutils::Format("error evaluating \"%s\". Variable name \"%s\" is not function type",function_access.c_str(),access_var[access_var.size()-1].c_str()));
			return false;
		}

		return true;
	}

	ZetScript::~ZetScript(){

		virtual_machine->ClearGlobalVars();



		// clear objects...
		delete scope_factory;
		delete script_function_factory;
		delete native_function_factory;
		delete script_class_factory;
		delete eval_obj;

		virtual_machine=NULL;


		for(std::map<std::string,ConstantValueInfo *>::iterator it=m_contantPool.begin();it!=m_contantPool.end();it++){
			ConstantValueInfo *icv=it->second;
			switch(GET_MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_TYPES(icv->properties)){
			default:
				break;
			case MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_INTEGER:
				//delete (int *)icv->var_ref;
				break;
			case MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_BOOLEAN:
				//delete (bool *)icv->stk_value;
				break;
			case MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_FLOAT:
				//delete (float *)icv->stk_value;
				break;
			case MSK_STACK_ELEMENT_PROPERTY_VAR_TYPE_STRING:
				delete (ScriptVarString *)icv->var_ref;
				break;

			}

			delete icv;
		}

		m_contantPool.clear();

	}

}


