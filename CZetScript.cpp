#include "zetscript.h"

#define FORMAT_PRINT_INSTRUCTION "%04i"



namespace zetscript{

	// define prototype ...
	int error_line;
	char error_filename[512];
	char error_description[1024];
	const char *error_type="NA";


	void  write_error(const char *filename, int line, const  char  *input_text, ...){

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

	int getErrorLine(){
		return error_line;
	}

	const char * getErrorDescription(){
		return error_description;
	}

	const char * getErrorFilename(){
		return error_filename;
	}



	CZetScript::CZetScript(){

		scope_factory = new CScopeFactory(this);
		native_function_factory = new CNativeFunctionFactory(this);
		script_function_factory= new CScriptFunctionFactory(this);
		eval_obj = new CEval(this);
		virtual_machine = new CVirtualMachine(this);
		script_class_factory = new CScriptClassFactory(this);


		eval_int=0;
		eval_float=0;
		eval_string="";
		eval_bool = false;

		zs=this;

		//----
		//mapTypeConversion = new std::map<int, std::map<int, fntConversionType>>();

		STR_VOID_TYPE = typeid(void).name();
		STR_INT_TYPE_PTR = typeid(int *).name();
		STR_FLOAT_TYPE_PTR = typeid(float *).name();
		STR_CONST_CHAR_TYPE_PTR = typeid(const char *).name();
		STR_STRING_TYPE_PTR = typeid(std::string *).name();
		STR_BOOL_TYPE_PTR = typeid(bool *).name();

		// particular case for return ... for not to force user returning pointer (this is the only vars allowed, sorry!)
		STR_BOOL_TYPE =  typeid(bool).name();
		STR_INT_TYPE =  typeid(int).name();
		STR_UNSIGNED_INT_TYPE =  typeid(unsigned int).name();
		STR_INTPTR_T_TYPE =  typeid(intptr_t).name();
		STR_FLOAT_TYPE =  typeid(float).name();
		STR_STACK_ELEMENT_TYPE=  typeid(StackElement *).name();



	}

	//----------------------------------------------------------------------------------------------------------------------------------------------------------------
	 // PRINT ASM INFO


	 std::string CZetScript::getStrTypeLoadValue(CScriptFunction *current_function,PtrInstruction m_listStatements, int current_instruction){

		 char print_aux_load_value[512] = {0};
		 char object_access[512] = "";

		 CByteCode * instruction =&m_listStatements[current_instruction];
		 ConstantValueInfo *icv;
		 std::string symbol_value=INSTRUCTION_GET_SYMBOL_NAME(current_function,instruction);
		 if(instruction->op_code != LOAD){
			 return "ERROR";
		 }



		 sprintf(print_aux_load_value,"UNDEFINED");

		 if(instruction->properties & INS_PROPERTY_ACCESS_SCOPE){

			 sprintf(object_access,
					"[" FORMAT_PRINT_INSTRUCTION "]."

					,(int)instruction->op2_value);
		 }
		 else if(instruction->properties & INS_PROPERTY_THIS_SCOPE){
			sprintf(object_access,"this.");
		 }

		 switch(instruction->op1_value){

			case LOAD_TYPE::LOAD_TYPE_CONSTANT:
				icv=(ConstantValueInfo *)instruction->op2_value;
				switch(icv->properties){
				case STK_PROPERTY_TYPE_BOOLEAN:
				case STK_PROPERTY_TYPE_INTEGER:
					sprintf(print_aux_load_value,"CONST(%i)",(int)((intptr_t)icv->stkValue));
					break;
				case STK_PROPERTY_TYPE_NUMBER:
					sprintf(print_aux_load_value,"CONST(%f)",*((float *)&icv->stkValue));
					break;
				case STK_PROPERTY_TYPE_STRING:
					sprintf(print_aux_load_value,"CONST(%s)",((const char *)icv->stkValue));
					break;

				}
				break;

			case LOAD_TYPE::LOAD_TYPE_VARIABLE:
				sprintf(print_aux_load_value,"%sVAR(%s)",object_access,symbol_value.c_str());
				break;
			case LOAD_TYPE::LOAD_TYPE_FUNCTION:

				sprintf(print_aux_load_value,"%sFUN(%s)",object_access,symbol_value.c_str());
				break;

			case LOAD_TYPE::LOAD_TYPE_ARGUMENT:
				sprintf(print_aux_load_value,"ARG(%s)",symbol_value.c_str());
				break;
			default:

				break;
		}
		return print_aux_load_value;
	 }

	 void CZetScript::printGeneratedCode(CScriptFunction *sfo){

		// PRE: it should printed after compile and updateReferences.
		std::string pre="";
		std::string post="";

		unsigned idx_instruction=0;
		for(CByteCode * instruction=sfo->instruction; instruction->op_code!= END_FUNCTION; instruction++,idx_instruction++){

			int n_ops=0;
			int op1_value = instruction->op1_value;
			int op2_value = instruction->op2_value;

			if(op1_value != -1)
				n_ops++;

			 if(op2_value != -1)
				 n_ops++;

			 pre="";
			 post="";

				switch(GET_INS_PROPERTY_PRE_POST_OP(instruction->properties)){
				case INS_PROPERTY_PRE_NEG:
					pre="-";
					break;
				case INS_PROPERTY_PRE_INC:
					pre="++";
					break;
				case INS_PROPERTY_PRE_DEC:
					pre="--";
					break;
				case INS_PROPERTY_POST_INC:
					post="++";
					break;
				case INS_PROPERTY_POST_DEC:
					post="--";
					break;
				default:
					// check whether is constant and numeric
					if(instruction->op_code==OP_CODE::LOAD && instruction->op1_value==LOAD_TYPE_CONSTANT){
						ConstantValueInfo *icv = (((ConstantValueInfo *)instruction->op2_value));
						float n;

						// change the sign
						switch(icv->properties){
						default:
							break;
						case STK_PROPERTY_TYPE_INTEGER:
							if(((intptr_t)icv->stkValue)<0){
								pre="-";
							}
							break;
						case STK_PROPERTY_TYPE_NUMBER:
							memcpy(&n,&icv->stkValue,sizeof(float));
							if(n<0){
								pre="-";
							}
							break;
						}
					}
					break;

				}
			switch(instruction->op_code){

			case  NEW:
				printf("[" FORMAT_PRINT_INSTRUCTION "]\t%s\t%s\n",idx_instruction,CByteCode::opCodeToStr(instruction->op_code),instruction->op1_value!=ZS_INVALID_CLASS?GET_SCRIPT_CLASS_NAME(instruction->op1_value):"???");
				break;
			case  LOAD:
				printf("[" FORMAT_PRINT_INSTRUCTION "]\t%s\t%s%s%s\n"
						,idx_instruction,
						CByteCode::opCodeToStr(instruction->op_code),
						pre.c_str(),
						getStrTypeLoadValue(sfo,sfo->instruction,idx_instruction).c_str(),
						post.c_str());
				break;
			case JNT:
			case JT:
			case JMP:
				printf("[" FORMAT_PRINT_INSTRUCTION "]\t%s\t%03i\n"
						,idx_instruction
						,CByteCode::opCodeToStr(instruction->op_code)
						,(int)instruction->op2_value);
				break;
			case PUSH_SCOPE:
				printf("[" FORMAT_PRINT_INSTRUCTION "]\t%s%c%s%s%s%c\n"
						,idx_instruction
						,CByteCode::opCodeToStr(instruction->op_code)
						,instruction->op1_value!=0?'(':' '
						,instruction->op1_value&SCOPE_PROPERTY::BREAK?"BREAK":""
						,instruction->op1_value&SCOPE_PROPERTY::CONTINUE?" CONTINUE":""
						,instruction->op1_value&SCOPE_PROPERTY::FOR_IN?" FOR_IN":""
						,instruction->op1_value!=0?')':' '
						);
				break;
			case POP_SCOPE:
				printf("[" FORMAT_PRINT_INSTRUCTION "]\t%s%c%s%s%s%c\n"
						,idx_instruction
						,CByteCode::opCodeToStr(instruction->op_code)
						,instruction->op1_value!=0?'(':' '
						,instruction->op1_value&SCOPE_PROPERTY::BREAK?"BREAK":""
						,instruction->op1_value&SCOPE_PROPERTY::CONTINUE?" CONTINUE":""
						,instruction->op1_value&SCOPE_PROPERTY::FOR_IN?" FOR_IN":""
						,instruction->op1_value!=0?')':' '
						);
				break;
			default:

				if(n_ops==0){
					printf("[" FORMAT_PRINT_INSTRUCTION "]\t%s\n",idx_instruction,CByteCode::opCodeToStr(instruction->op_code));
				}else if(n_ops==1){
					printf("[" FORMAT_PRINT_INSTRUCTION "]\t%s%s\n"
							,idx_instruction
							,CByteCode::opCodeToStr(instruction->op_code)
							,(instruction->properties & STK_PROPERTY_READ_TWO_POP_ONE)?"_CS":""
							);
				}else{
					printf("[" FORMAT_PRINT_INSTRUCTION "]\t%s\n"
							,idx_instruction
							,CByteCode::opCodeToStr(instruction->op_code)
							);
				}
				break;
			}
		}


		// and then print its functions ...
		std::vector<CScriptFunction *> * m_vf = &sfo->local_function;

		for(unsigned j =0; j < m_vf->size(); j++){

			CScriptFunction *local_irfs = (*m_vf)[j];

			if(( local_irfs->symbol_info.symbol_info_properties & SYMBOL_INFO_PROPERTY_C_OBJECT_REF) != SYMBOL_INFO_PROPERTY_C_OBJECT_REF){
				std::string symbol_ref="????";


				//strcpy(symbol_ref,AST_SYMBOL_VALUEZJ_CONST_CHAR(local_irfs->symbol_info.idxAstNode));

				if(local_irfs->idx_class!=ZS_INVALID_CLASS){
					CScriptClass *sc = GET_SCRIPT_CLASS(local_irfs->idx_class);
					if(sc->idx_class == IDX_CLASS_MAIN){
						symbol_ref="Main";
					}else{
						symbol_ref=sfo->symbol_info.symbol->name+std::string("::")+std::string("????");
					}
				}

				printf("-------------------------------------------------------\n");
				printf("\nCode for function \"%s\"\n\n",symbol_ref.c_str());
				printGeneratedCode(m_vf->at(j));
			}
		}
	 }

	 void CZetScript::printGeneratedCode(){

		 std::vector<CScriptClass *> *vec_script_class_node=script_class_factory->getVectorScriptClassNode();
		 // for all classes print code...
		 for(unsigned i = 0; i < vec_script_class_node->size(); i++){
			 CScriptClass *rc=vec_script_class_node->at(i);
			 for(unsigned f = 0; f < rc->local_function.size(); f++){
				 printGeneratedCode(rc->local_function[f]);
			 }
		 }
	 }
	 // PRINT ASM INFO
	 //----------------------------------------------------------------------------------------------------------------------------------------------------------------



	//--------------------------------
	 // FILE MANAGEMENT


	bool CZetScript::isFilenameAlreadyParsed(const std::string & filename){
		for(unsigned i = 0; i < m_parsedSource.size(); i++){
			if(m_parsedSource.at(i).filename==filename){
				return true;
			}
		}
		return false;
	}

	const char * CZetScript::getParsedFilenameFromIdx(unsigned idx){

		if(idx >= m_parsedSource.size()){
			THROW_RUNTIME_ERROR("out of bounds");
			return DEFAULT_NO_FILENAME;
		}

		return m_parsedSource.at(idx).filename.c_str();
	}

	//-----------------------------------------------------------------------------------------------------------------------------------------
	//
	// CONSTANT MANAGEMENT

	ConstantValueInfo *CZetScript::getRegisteredConstantValue(const std::string & const_name){

		if((m_contantPool).count(const_name) == 1){
			return (m_contantPool)[const_name];
		}
		return NULL;
	}

	ConstantValueInfo * CZetScript::registerConstantValue(const std::string & const_name, void *obj, unsigned short properties){

		ConstantValueInfo * info_ptr=NULL;

		if(getRegisteredConstantValue(const_name) == NULL){
			info_ptr=new ConstantValueInfo;
			*info_ptr={obj,NULL,properties};
			(m_contantPool)[const_name]=info_ptr;
		}else{
			THROW_RUNTIME_ERROR(zs_string::sformat("internal:constant %s already exist",const_name.c_str()));
		}

		return info_ptr;
	}

	ConstantValueInfo * CZetScript::registerConstantIntValue(const std::string & const_name, int _value){
		intptr_t value = _value;
		unsigned short type=STK_PROPERTY_TYPE_INTEGER;
		StackElement *stk;

		if((stk = getRegisteredConstantValue(const_name))!=NULL){
			return stk;
		}

		return registerConstantValue(const_name,(void *)value,type);

	}

	// CONSTANT MANAGEMENT
	//
	//-----------------------------------------------------------------------------------------------------------------------------------------



	StackElement CZetScript::C_REF_InfoVariable_2_StackElement(VariableSymbolInfo *ir_var, void *ptr_variable){

		if(ir_var->symbol_info_properties & SYMBOL_INFO_PROPERTY_C_OBJECT_REF){

			if(STR_INT_TYPE_PTR==ir_var->c_type){
				return {

						0,
						ptr_variable,
						STK_PROPERTY_TYPE_INTEGER|STK_PROPERTY_IS_C_VAR
				};

			}else if(STR_FLOAT_TYPE_PTR==ir_var->c_type){
				return {

						0,
						ptr_variable,
						STK_PROPERTY_TYPE_NUMBER|STK_PROPERTY_IS_C_VAR
				};
			}else if(STR_CONST_CHAR_TYPE_PTR==ir_var->c_type){

				return {

						ptr_variable,
						0,
						STK_PROPERTY_TYPE_STRING
				};
			}else if(STR_STRING_TYPE_PTR==ir_var->c_type){

				return {

						(void *)((std::string *)ptr_variable)->c_str(),
						ptr_variable,
						STK_PROPERTY_TYPE_STRING|STK_PROPERTY_IS_C_VAR
				};
			}else if(STR_BOOL_TYPE_PTR==ir_var->c_type){
				return {

						0,
						ptr_variable,
						STK_PROPERTY_TYPE_BOOLEAN|STK_PROPERTY_IS_C_VAR
				};
			}else{
				CScriptClass *info_registered_class = GET_SCRIPT_CLASS_INFO_BY_C_PTR_NAME(ir_var->c_type);//  CScriptClass::getInstance()->getRegisteredClassBy_C_ClassPtr(ir_var->c_type);

				if(info_registered_class){
					CScriptVar *var = new CScriptVar(this);
					var->init(info_registered_class,ptr_variable);

					return{

							NULL,
							var,
							STK_PROPERTY_TYPE_SCRIPTVAR|STK_PROPERTY_IS_C_VAR
					};
				}
		}
		}else{
			THROW_RUNTIME_ERROR(zs_string::sformat("Variable %s is not c referenced as C symbol",ir_var->symbol->name.c_str()));
		}

		return{
			0,
			NULL,
			STK_PROPERTY_TYPE_UNDEFINED};
	}




	/*void  CZetScript::internal_print_error(const char *s){
		 virtual_machine->setError(s);
	}*/


	void CZetScript::setPrintOutCallback(void (* _printout_callback)(const char *)){
		print_out_callback=_printout_callback;
	}

	void (* CZetScript::print_out_callback)(const char *) = NULL;



	void CZetScript::clear(){

		virtual_machine->clearGlobalVars();

		CScriptFunction * main_function = this->script_function_factory->getScriptFunction(IDX_MAIN_FUNCTION);

		// clean main functions ... remove script functions and leave c functions...
		for (unsigned f = 0;
			f < main_function->local_function.size()
			;) {
			// get function info
			CScriptFunction * local_function = main_function->local_function[f];

			if ((local_function->symbol_info.symbol_info_properties & SYMBOL_INFO_PROPERTY_C_OBJECT_REF) != SYMBOL_INFO_PROPERTY_C_OBJECT_REF) {
				main_function->local_function.erase(main_function->local_function.begin() + f);
			}
			else {
				f++;
			}

		}

		// remove variables except c variables ...
		for (unsigned v = 0;
			v < main_function->local_variable.size(); ) {

			if ((main_function->local_variable[v].symbol_info_properties & SYMBOL_INFO_PROPERTY_C_OBJECT_REF) != SYMBOL_INFO_PROPERTY_C_OBJECT_REF) {

				main_function->local_variable.erase(main_function->local_variable.begin() + v);

			}
			else {
				v++;
			}
		}

		// remove scope vars...
		scope_factory->clear();
		script_function_factory->clear();
		script_class_factory->clear();




	}





	int * CZetScript::evalIntValue(const std::string & str_to_eval){


		if(!eval(str_to_eval.c_str())){
			return NULL;
		}

		StackElement *se=virtual_machine->getLastStackValue();

		if(se != NULL){

			if(se->properties & STK_PROPERTY_TYPE_INTEGER){

				eval_int=(int)((intptr_t)se->stkValue);
				return &eval_int;
			}
			else{
				THROW_RUNTIME_ERROR(zs_string::sformat("evalIntValue(...): Error evaluating \"%s\". Property:0x%X",str_to_eval.c_str(),se->properties));
			}
		}

		return NULL;


	}

	bool * CZetScript::evalBoolValue(const std::string & str_to_eval){

		if(!eval(str_to_eval.c_str())){
			return NULL;
		}

		StackElement *se=virtual_machine->getLastStackValue();

		if(se != NULL){

			if(se->properties & STK_PROPERTY_TYPE_BOOLEAN){
				eval_bool=(bool)((intptr_t)se->stkValue);
				return &eval_bool;

			}else{
				THROW_RUNTIME_ERROR(zs_string::sformat("evalBoolValue(...): Error evaluating \"%s\". Property:0x%X",str_to_eval.c_str(),se->properties));
			}
		}

		return NULL;
	}

	float * CZetScript::evalFloatValue(const std::string & str_to_eval){

		if(!eval(str_to_eval.c_str())){
			return NULL;
		}

		StackElement *se=virtual_machine->getLastStackValue();

		if(se != NULL){

			if(se->properties & STK_PROPERTY_TYPE_NUMBER){
				eval_float = *((float *)(&se->stkValue));
				return &eval_float;
			}
			else{
				THROW_RUNTIME_ERROR(zs_string::sformat("evalFloatValue(...): Error evaluating \"%s\". Property:0x%X",str_to_eval.c_str(),se->properties));

			}
		}


		return NULL;
	}

	std::string * CZetScript::evalStringValue(const std::string & str_to_eval){


		if(!eval(str_to_eval.c_str())){
			return NULL;
		}

		StackElement *se=virtual_machine->getLastStackValue();

		if(se != NULL){

			if(se->properties & STK_PROPERTY_TYPE_STRING){

				eval_string = ((const char *)se->stkValue);
				return &eval_string;
			}
			else{
				zs_string::sformat("evalStringValue(...): Error evaluating \"%s\". Property:0x%X",str_to_eval.c_str(),se->properties);
			}
		}

		return NULL;
	}


	void CZetScript::execute(){

		bool error=false;

		// the first code to execute is the main function that in fact is a special member function inside our main class
		virtual_machine->execute(script_class_factory->getMainFunction(), NULL,error,NO_PARAMS);

		if(error){
			THROW_SCRIPT_ERROR();
		}
	}

	bool CZetScript::eval(const std::string & expression, bool exec_vm, bool show_bytecode, const char * filename)  {


		if(!eval_obj->eval(expression.c_str(),filename)){
			return false;
		}


		virtual_machine->buildCache();


		if(show_bytecode){
			printGeneratedCode();
		}

		if(exec_vm){
			execute();
		}

		return true;
	}

	bool CZetScript::evalFile(const std::string &  filename, bool exec_vm, bool show_bytecode){
		//int idx_file=-1;
		bool error=false;
		char *buf_tmp=NULL;

		if(!isFilenameAlreadyParsed(filename)){
			ParsedSourceInfo ps;
			ps.filename = filename;
			m_parsedSource.push_back(ps);
			//idx_file=m_parsedSource.size()-1;
			int n_bytes;

			if((buf_tmp=zs_io::read_file(filename, n_bytes))!=NULL){
				try{
					eval(buf_tmp, exec_vm, show_bytecode,filename.c_str());
				}
				catch(exception::script_error & e){
					free(buf_tmp);
					buf_tmp=NULL;
					THROW_EXCEPTION(e);
					error=true;
				}
			}

		}else{
			// already parsed
			THROW_RUNTIME_ERROR(zs_string::sformat("Filename \"%s\" already parsed",filename));
			error=true;
		}

		if(buf_tmp!=NULL){
			free(buf_tmp);
		}

		return !error;
	}

	bool CZetScript::getScriptObject(const std::string &function_access,CScriptVar **calling_obj,CScriptFunction **fun_obj ){

		//ZS_CLEAR_ERROR_MSG();

		std::vector<std::string> access_var = zs_string::split(function_access,'.');
		//CScriptFunction * main_function = main_function;

		/*if(main_function == NULL){
			zs_string::sformat("main_function is not initialized");
			return false;
		}*/
		CScriptFunction * main_function = script_class_factory->getMainFunction();
		*calling_obj = NULL;
		FunctionSymbol *is=NULL;
		StackElement *se=NULL;
		*fun_obj=NULL;

		// 1. some variable in main function ...
		if(access_var.size()>1){
			for(unsigned i=0; i < access_var.size()-1; i++){

				std::string symbol_to_find=access_var[i];
				if(i==0){ // get variable through main_class.main_function (global element)
					//symbol_to_find= CEval::makeSymbolRef(symbol_to_find,IDX_GLOBAL_SCOPE);
					for(unsigned j = 0; j < main_function->local_variable.size() && *calling_obj==NULL; j++){
						if(main_function->local_variable[j].symbol->name==symbol_to_find
						&& main_function->local_variable[j].symbol->idxScope == IDX_GLOBAL_SCOPE){
							StackElement *stk = virtual_machine->getStackElement(j); // main_function->object_info.local_symbols.variable[j].
							if(stk!=NULL){
								if(stk->properties & STK_PROPERTY_TYPE_SCRIPTVAR){
									*calling_obj=(CScriptVar *)stk->varRef;
								}
							}
							else{
								zs_string::sformat("cannot access i (%i)",j);
								return false;
							}
						}
					}

					if((*calling_obj) == NULL){
						zs_string::sformat("error evaluating \"%s\". Variable name \"%s\" doesn't exist",function_access.c_str(),symbol_to_find.c_str());
						return false;
					}

				}else{ // we have got the calling_obj from last iteration ...
					se = (*calling_obj)->getVariableSymbol(symbol_to_find);

					if(se!=NULL){

						if(se->properties & STK_PROPERTY_TYPE_SCRIPTVAR){
							*calling_obj=(CScriptVar *)se->varRef;
						}else{
							zs_string::sformat("error evaluating \"%s\". Variable name \"%s\" not script variable",function_access.c_str(),symbol_to_find.c_str());
							return false;
						}
					}
					else{
						zs_string::sformat("error evaluating \"%s\". Variable name \"%s\" doesn't exist",function_access.c_str(),symbol_to_find.c_str());
						return false;
					}
				}
			}

			is=(*calling_obj)->getFunctionSymbol(access_var[access_var.size()-1]);
			if(is!=NULL){
				if(is->object.properties & STK_PROPERTY_TYPE_FUNCTION){
					*fun_obj=(CScriptFunction *)is->object.stkValue;
				}
			}else{

				zs_string::sformat("error evaluating \"%s\". Cannot find function \"%s\"",function_access.c_str(),access_var[access_var.size()-1].c_str());
				return false;
			}

		}else{ // some function in main function
			//*calling_obj = m_mainObject;
			std::string symbol_to_find=access_var[0];
			for(unsigned i = 0; i < main_function->local_function.size() && *fun_obj==NULL; i++){
				CScriptFunction *aux_fun_obj=main_function->local_function[i];
				if(		aux_fun_obj->symbol_info.symbol->name  == symbol_to_find
				  && aux_fun_obj->symbol_info.symbol->idxScope == IDX_GLOBAL_SCOPE){
					*fun_obj=aux_fun_obj;
				}
			}
		}

		if(*fun_obj==NULL){
			THROW_RUNTIME_ERROR(zs_string::sformat("error evaluating \"%s\". Variable name \"%s\" is not function type",function_access.c_str(),access_var[access_var.size()-1].c_str()));
			return false;
		}

		return true;
	}

	CZetScript::~CZetScript(){

		virtual_machine->clearGlobalVars();



		// clear objects...
		delete scope_factory;
		delete script_function_factory;
		delete native_function_factory;
		delete script_class_factory;
		delete eval_obj;

		virtual_machine=NULL;


		for(std::map<std::string,ConstantValueInfo *>::iterator it=m_contantPool.begin();it!=m_contantPool.end();it++){
			ConstantValueInfo *icv=it->second;
			switch(GET_INS_PROPERTY_VAR_TYPE(icv->properties)){
			default:
				break;
			case STK_PROPERTY_TYPE_INTEGER:
				//delete (int *)icv->varRef;
				break;
			case STK_PROPERTY_TYPE_BOOLEAN:
				//delete (bool *)icv->stkValue;
				break;
			case STK_PROPERTY_TYPE_NUMBER:
				//delete (float *)icv->stkValue;
				break;
			case STK_PROPERTY_TYPE_STRING:
				delete (CScriptVarString *)icv->varRef;
				break;

			}

			delete icv;
		}

		m_contantPool.clear();

	}

}
