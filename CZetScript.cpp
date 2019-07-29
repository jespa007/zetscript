#include "zetscript.h"

#define FORMAT_PRINT_INSTRUCTION "%04i"

#define REGISTER_BUILT_IN_STRUCT(type_class, idx_class)\
	if(vec_script_class_node.size()!=idx_class){\
		THROW_RUNTIME_ERROR(string::sformat("Error: class built in type %s doesn't match its id",STR(type_class)));\
		return;\
	}\
	register_C_Class<type_class>(STR(type_class));


#define REGISTER_BUILT_IN_CLASS(type_class, idx_class)\
	if(script_class_factory->getVectorScriptClassNode()->size()!=idx_class){\
		THROW_RUNTIME_ERROR(string::sformat("Error: class built in type %s doesn't match its id",STR(type_class)));\
		return;\
	}\
	register_C_ClassBuiltIn<type_class>(STR(type_class));


#define REGISTER_BUILT_IN_TYPE(type_class, idx_class)\
	if(script_class_factory->getVectorScriptClassNode()->size()!=idx_class){\
		THROW_RUNTIME_ERROR(string::sformat("Error initializing C built in type: %s",STR(type_class)));\
		return;\
	}else{\
		CScriptClass *sc=script_class_factory->registerClass(__FILE__,__LINE__,STR(type_class),"");\
		GET_SCOPE(sc->symbol_info.symbol->idxScope)->is_c_node=true;\
		sc->symbol_info.properties=SYMBOL_INFO_PROPERTY::PROPERTY_C_OBJECT_REF;\
		sc->str_class_ptr_type=(typeid(type_class).name());\
	}

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
		eval = new CEval(this);
		virtual_machine = new CVirtualMachine(this);
		script_class_factory = new CScriptClassFactory(this);


		main_function=NULL;
		main_object=NULL;

		eval_int=0;
		eval_float=0;
		eval_string="";
		eval_bool = false;
		register_c_base_symbols=false;
		zs=this;

		//----

		register_c_base_symbols=false;
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

		//------------------------------------------------------------------------------------------------------------------------------------------------------------------

		// REGISTER BUILT IN SCRIPT TYPES
		// MAIN CLASS (0)
		main_object=script_class_factory->registerClass(__FILE__, __LINE__,MAIN_SCRIPT_CLASS_NAME,""); // 0
		if(main_object->symbol_info.symbol->idxScope!=IDX_GLOBAL_SCOPE){
			THROW_RUNTIME_ERROR("Error initializing global scope");
			return;
		}

		// REGISTER BUILT IN C TYPES
		REGISTER_BUILT_IN_TYPE(void,IDX_TYPE_VOID_C);
		REGISTER_BUILT_IN_TYPE(int *,IDX_TYPE_INT_PTR_C);
		REGISTER_BUILT_IN_TYPE(float *,IDX_TYPE_FLOAT_PTR_C);
		REGISTER_BUILT_IN_TYPE(const char *,IDX_TYPE_CONST_CHAR_PTR_C);
		REGISTER_BUILT_IN_TYPE(std::string *,IDX_TYPE_STRING_PTR_C);
		REGISTER_BUILT_IN_TYPE(bool *,IDX_TYPE_BOOL_PTR_C);
		REGISTER_BUILT_IN_TYPE(int,IDX_TYPE_INT_C);
		REGISTER_BUILT_IN_TYPE(unsigned int,IDX_TYPE_UNSIGNED_INT_C);
		REGISTER_BUILT_IN_TYPE(intptr_t,IDX_TYPE_INTPTR_T_C);
		REGISTER_BUILT_IN_TYPE(float,IDX_TYPE_FLOAT_C);
		REGISTER_BUILT_IN_TYPE(bool,IDX_TYPE_BOOL_C);

		// REGISTER BUILT IN CLASS TYPES
		REGISTER_BUILT_IN_STRUCT(StackElement,IDX_STACK_ELEMENT);
		REGISTER_BUILT_IN_CLASS(CScriptVariable,IDX_CLASS_SCRIPT_VAR);
		REGISTER_BUILT_IN_CLASS(CStringScriptVariable,IDX_CLASS_STRING);
		REGISTER_BUILT_IN_CLASS(CVectorScriptVariable,IDX_CLASS_VECTOR);
		REGISTER_BUILT_IN_CLASS(CFunctorScriptVariable,IDX_CLASS_FUNCTOR);
		REGISTER_BUILT_IN_CLASS(CDictionaryScriptVariable,IDX_CLASS_DICTIONARY);


		//------------------------------------------------------------------------------------------------------------------------------------------------------------------

		// rgister basic classes (Warning!!! must match idx !!! and the order is important!!!)

		//----------------------------------------------------------------------
		// From here you defined all basic, start define hierarchy

		// register custom functions ...
		class_C_BaseOf<CVectorScriptVariable,CScriptVariable>();
		class_C_BaseOf<CFunctorScriptVariable,CScriptVariable>();
		class_C_BaseOf<CDictionaryScriptVariable,CScriptVariable>();


		//------------------------------------------------------------------------------------------------------------
		// Let's register functions,...
		// register c function's

		main_object->registerFunctionMember(__FILE__,__LINE__,MAIN_SCRIPT_FUNCTION_NAME);

		register_C_Function("print",print);
		//register_C_Function("error",internal_print_error);

		register_C_FunctionMember<CVectorScriptVariable>("size",&CVectorScriptVariable::size);
		register_C_FunctionMember<CVectorScriptVariable>("push",static_cast<void (CVectorScriptVariable:: *)(StackElement *)>(&CVectorScriptVariable::push));
		register_C_FunctionMember<CVectorScriptVariable>("pop",&CVectorScriptVariable::pop);


		register_C_FunctionMember<CDictionaryScriptVariable>("add",&CDictionaryScriptVariable::add_attr);
		register_C_FunctionMember<CDictionaryScriptVariable>("remove",&CDictionaryScriptVariable::remove_attr);
		register_C_FunctionMember<CDictionaryScriptVariable>("size",&CDictionaryScriptVariable::size);

	}

	//----------------------------------------------------------------------------------------------------------------------------------------------------------------
	 // PRINT ASM INFO


	 std::string CZetScript::getStrTypeLoadValue(CScriptFunction *current_function,PtrInstruction m_listStatements, int current_instruction){

		 char print_aux_load_value[512] = {0};
		 char object_access[512] = "";

		 OpCodeInstruction * instruction =&m_listStatements[current_instruction];
		 ConstantValueInfo *icv;
		 std::string symbol_value=INSTRUCTION_GET_SYMBOL_NAME(current_function,instruction);
		 if(instruction->op_code != LOAD){
			 return "ERROR";
		 }



		 sprintf(print_aux_load_value,"UNDEFINED");

		 if(instruction->properties & INS_PROPERTY_ACCESS_SCOPE){

			 sprintf(object_access,
					"[" FORMAT_PRINT_INSTRUCTION "]."

					,(int)instruction->index_op2);
		 }
		 else if(instruction->properties & INS_PROPERTY_THIS_SCOPE){
			sprintf(object_access,"this.");
		 }

		 switch(instruction->index_op1){

			case LOAD_TYPE::LOAD_TYPE_CONSTANT:
				icv=(ConstantValueInfo *)instruction->index_op2;
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
		for(OpCodeInstruction * instruction=sfo->instruction; instruction->op_code!= END_FUNCTION; instruction++,idx_instruction++){

			int n_ops=0;
			int index_op1 = instruction->index_op1;
			int index_op2 = instruction->index_op2;

			if(index_op1 != -1)
				n_ops++;

			 if(index_op2 != -1)
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
					if(instruction->op_code==OP_CODE::LOAD && instruction->index_op1==LOAD_TYPE_CONSTANT){
						ConstantValueInfo *icv = (((ConstantValueInfo *)instruction->index_op2));
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
				printf("[" FORMAT_PRINT_INSTRUCTION "]\t%s\t%s\n",idx_instruction,virtual_machine->getOpCodeStr(instruction->op_code),instruction->index_op1!=ZS_INVALID_CLASS?GET_SCRIPT_CLASS_NAME(instruction->index_op1):"???");
				break;
			case  LOAD:
				printf("[" FORMAT_PRINT_INSTRUCTION "]\t%s\t%s%s%s\n"
						,idx_instruction,
						virtual_machine->getOpCodeStr(instruction->op_code),
						pre.c_str(),
						getStrTypeLoadValue(sfo,sfo->instruction,idx_instruction).c_str(),
						post.c_str());
				break;
			case JNT:
			case JT:
			case JMP:
				printf("[" FORMAT_PRINT_INSTRUCTION "]\t%s\t%03i\n"
						,idx_instruction
						,CZetScript::virtual_machine->getOpCodeStr(instruction->op_code)
						,(int)instruction->index_op2);
				break;
			case PUSH_SCOPE:
				printf("[" FORMAT_PRINT_INSTRUCTION "]\t%s%c%s%s%s%c\n"
						,idx_instruction
						,virtual_machine->getOpCodeStr(instruction->op_code)
						,instruction->index_op1!=0?'(':' '
						,instruction->index_op1&SCOPE_PROPERTY::BREAK?"BREAK":""
						,instruction->index_op1&SCOPE_PROPERTY::CONTINUE?" CONTINUE":""
						,instruction->index_op1&SCOPE_PROPERTY::FOR_IN?" FOR_IN":""
						,instruction->index_op1!=0?')':' '
						);
				break;
			case POP_SCOPE:
				printf("[" FORMAT_PRINT_INSTRUCTION "]\t%s%c%s%s%s%c\n"
						,idx_instruction
						,virtual_machine->getOpCodeStr(instruction->op_code)
						,instruction->index_op1!=0?'(':' '
						,instruction->index_op1&SCOPE_PROPERTY::BREAK?"BREAK":""
						,instruction->index_op1&SCOPE_PROPERTY::CONTINUE?" CONTINUE":""
						,instruction->index_op1&SCOPE_PROPERTY::FOR_IN?" FOR_IN":""
						,instruction->index_op1!=0?')':' '
						);
				break;
			default:

				if(n_ops==0){
					printf("[" FORMAT_PRINT_INSTRUCTION "]\t%s\n",idx_instruction,virtual_machine->getOpCodeStr(instruction->op_code));
				}else if(n_ops==1){
					printf("[" FORMAT_PRINT_INSTRUCTION "]\t%s%s\n"
							,idx_instruction
							,virtual_machine->getOpCodeStr(instruction->op_code)
							,(instruction->properties & STK_PROPERTY_READ_TWO_POP_ONE)?"_CS":""
							);
				}else{
					printf("[" FORMAT_PRINT_INSTRUCTION "]\t%s\n"
							,idx_instruction
							,virtual_machine->getOpCodeStr(instruction->op_code)
							);
				}
				break;
			}
		}


		// and then print its functions ...
		std::vector<CScriptFunction *> * m_vf = &sfo->local_function;

		for(unsigned j =0; j < m_vf->size(); j++){

			CScriptFunction *local_irfs = (*m_vf)[j];

			if(( local_irfs->symbol_info.properties & PROPERTY_C_OBJECT_REF) != PROPERTY_C_OBJECT_REF){
				std::string symbol_ref="????";


				//strcpy(symbol_ref,AST_SYMBOL_VALUE_CONST_CHAR(local_irfs->symbol_info.idxAstNode));

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

	ConstantValueInfo *CZetScript::getConstant(const std::string & const_name){

		if((m_contantPool).count(const_name) == 1){
			return (m_contantPool)[const_name];
		}
		return NULL;
	}

	ConstantValueInfo * CZetScript::addConstant(const std::string & const_name, void *obj, unsigned short properties){

		ConstantValueInfo * info_ptr=NULL;

		if(getConstant(const_name) == NULL){
			info_ptr=new ConstantValueInfo;
			*info_ptr={obj,NULL,properties};
			(m_contantPool)[const_name]=info_ptr;
		}else{
			THROW_RUNTIME_ERROR(string::sformat("internal:constant %s already exist",const_name.c_str()));
		}

		return info_ptr;
	}

	ConstantValueInfo * CZetScript::addConstant(const std::string & const_name, int _value){
		intptr_t value = _value;
		unsigned short type=STK_PROPERTY_TYPE_INTEGER;
		StackElement *stk;

		if((stk = getConstant(const_name))!=NULL){
			return stk;
		}

		return addConstant(const_name,(void *)value,type);

	}

	// CONSTANT MANAGEMENT
	//
	//-----------------------------------------------------------------------------------------------------------------------------------------



	StackElement CZetScript::C_REF_InfoVariable_2_StackElement(VariableSymbolInfo *ir_var, void *ptr_variable){

		if(ir_var->properties & PROPERTY_C_OBJECT_REF){

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
					CScriptVariable *var = new CScriptVariable(this);
					var->init(info_registered_class,ptr_variable);

					return{

							NULL,
							var,
							STK_PROPERTY_TYPE_SCRIPTVAR|STK_PROPERTY_IS_C_VAR
					};
				}
		}
		}else{
			THROW_RUNTIME_ERROR(string::sformat("Variable %s is not c referenced as C symbol",ir_var->symbol->name.c_str()));
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
	 void  CZetScript::print(const char *s){

		printf("%s\n",s);
		fflush(stdout);

		if(print_out_callback){
			print_out_callback(s);
		}
	 }

	void (* CZetScript::print_out_callback)(const char *) = NULL;


	const char * CZetScript::getMetamethod(METAMETHOD_OPERATOR op){

		switch (op) {
		case	EQU_METAMETHOD:		return  "_equ";  // ==
		case	NOT_EQU_METAMETHOD: return  "_nequ";  // !=,
		case	LT_METAMETHOD:		return  "_lt";  // <
		case	LTE_METAMETHOD:		return  "_lte";  // <=
		case	NOT_METAMETHOD:		return  "_not"; // !
		case	GT_METAMETHOD:		return  "_gt";  // >
		case	GTE_METAMETHOD:		return  "_gte"; // >=

		case	NEG_METAMETHOD:		return  "_neg"; // -a, !a
		case	ADD_METAMETHOD:		return  "_add"; // +
		case	DIV_METAMETHOD:		return  "_div"; // /
		case	MUL_METAMETHOD:		return  "_mul"; // *
		case	MOD_METAMETHOD:		return  "_mod";  // %
		case	AND_METAMETHOD:		return  "_and"; // binary and
		case	OR_METAMETHOD:		return  "_or"; //   binary or
		case	XOR_METAMETHOD:		return  "_xor"; // binary xor
		case	SHL_METAMETHOD:		return  "_shl"; // binary shift left
		case	SHR_METAMETHOD:		return  "_shr"; // binary shift right
		case	SET_METAMETHOD:		return  "_set"; // set
		default:
			return "none";
		}

		return "none";
	}

	void CZetScript::register_C_BaseSymbols(bool _register){
		register_c_base_symbols = _register;
	}


	/**
	 * Register C variable
	 */
	 VariableSymbolInfo *  CZetScript::register_C_Variable(const std::string & var_name,void * var_ptr, const std::string & var_type, const char *registered_file,int registered_line)
	{
		//CScope *scope;
		VariableSymbolInfo *irs;
		//int idxVariable;

		if(var_ptr==NULL){
			THROW_RUNTIME_ERROR(string::sformat("cannot register var \"%s\" with NULL reference value", var_name.c_str()));
			return NULL;
		}

		CScriptFunction *main_function=MAIN_FUNCTION;

		if(main_function == NULL){
			THROW_RUNTIME_ERROR("main function is not created");
			return  NULL;
		}

		if(script_class_factory->getIdxClassFromIts_C_Type(var_type) == ZS_INVALID_CLASS){
			THROW_RUNTIME_ERROR(string::sformat("%s has not valid type (%s)",var_name.c_str(),var_type.c_str()));
			return  NULL;
		}


		if((irs = main_function->registerVariable(registered_file,registered_line,var_name,var_type,(intptr_t)var_ptr,PROPERTY_C_OBJECT_REF | PROPERTY_STATIC_REF)) != NULL){

			zs_print_debug_cr("Registered variable name: %s",var_name.c_str());

			return irs;
		}

		return NULL;
	}




	void CZetScript::clear(){

		virtual_machine->clearGlobalVars();

		main_function = this->script_function_factory->getScriptFunction(IDX_MAIN_FUNCTION);

		// clean main functions ... remove script functions and leave c functions...
		for (unsigned f = 0;
			f < main_function->local_function.size()
			;) {
			// get function info
			CScriptFunction * local_function = main_function->local_function[f];

			if ((local_function->symbol_info.properties & PROPERTY_C_OBJECT_REF) != PROPERTY_C_OBJECT_REF) {
				main_function->local_function.erase(main_function->local_function.begin() + f);
			}
			else {
				f++;
			}

		}

		// remove variables except c variables ...
		for (unsigned v = 0;
			v < main_function->local_variable.size(); ) {

			if ((main_function->local_variable[v].properties & PROPERTY_C_OBJECT_REF) != PROPERTY_C_OBJECT_REF) {

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


		if(!evalString(str_to_eval.c_str())){
			return NULL;
		}

		StackElement *se=virtual_machine->getLastStackValue();

		if(se != NULL){

			if(se->properties & STK_PROPERTY_TYPE_INTEGER){

				eval_int=(int)((intptr_t)se->stkValue);
				return &eval_int;
			}
			else{
				THROW_RUNTIME_ERROR(string::sformat("evalIntValue(...): Error evaluating \"%s\". Property:0x%X",str_to_eval.c_str(),se->properties));
			}
		}

		return NULL;


	}

	bool * CZetScript::evalBoolValue(const std::string & str_to_eval){

		if(!evalString(str_to_eval.c_str())){
			return NULL;
		}

		StackElement *se=virtual_machine->getLastStackValue();

		if(se != NULL){

			if(se->properties & STK_PROPERTY_TYPE_BOOLEAN){
				eval_bool=(bool)((intptr_t)se->stkValue);
				return &eval_bool;

			}else{
				THROW_RUNTIME_ERROR(string::sformat("evalBoolValue(...): Error evaluating \"%s\". Property:0x%X",str_to_eval.c_str(),se->properties));
			}
		}

		return NULL;
	}

	float * CZetScript::evalFloatValue(const std::string & str_to_eval){

		if(!evalString(str_to_eval.c_str())){
			return NULL;
		}

		StackElement *se=virtual_machine->getLastStackValue();

		if(se != NULL){

			if(se->properties & STK_PROPERTY_TYPE_NUMBER){
				eval_float = *((float *)(&se->stkValue));
				return &eval_float;
			}
			else{
				THROW_RUNTIME_ERROR(string::sformat("evalFloatValue(...): Error evaluating \"%s\". Property:0x%X",str_to_eval.c_str(),se->properties));

			}
		}


		return NULL;
	}

	std::string * CZetScript::evalStringValue(const std::string & str_to_eval){


		if(!evalString(str_to_eval.c_str())){
			return NULL;
		}

		StackElement *se=virtual_machine->getLastStackValue();

		if(se != NULL){

			if(se->properties & STK_PROPERTY_TYPE_STRING){

				eval_string = ((const char *)se->stkValue);
				return &eval_string;
			}
			else{
				string::sformat("evalStringValue(...): Error evaluating \"%s\". Property:0x%X",str_to_eval.c_str(),se->properties);
			}
		}

		return NULL;
	}


	void CZetScript::execute(){

		bool error=false;

		// the first code to execute is the main function that in fact is a special member function inside our main class
		virtual_machine->execute(main_function, NULL,error,NO_PARAMS);

		if(error){
			THROW_SCRIPT_ERROR();
		}
	}

	bool CZetScript::evalString(const char * expression, bool exec_vm, bool show_bytecode, const char * filename)  {


		if(!eval->eval(expression,filename)){
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

	bool CZetScript::evalFile(const char * filename, bool exec_vm, bool show_bytecode){
		int idx_file=-1;
		bool error=false;
		char *buf_tmp=NULL;

		if(!isFilenameAlreadyParsed(filename)){
			ParsedSourceInfo ps;
			ps.filename = filename;
			m_parsedSource.push_back(ps);
			idx_file=m_parsedSource.size()-1;
			int n_bytes;

			if((buf_tmp=io::read_file(filename, n_bytes))!=NULL){
				try{
					evalString(buf_tmp, exec_vm, show_bytecode,filename);
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
			THROW_RUNTIME_ERROR(string::sformat("Filename \"%s\" already parsed",filename));
			error=true;
		}

		if(buf_tmp!=NULL){
			free(buf_tmp);
		}

		return !error;
	}


	CZetScript::~CZetScript(){

		virtual_machine->clearGlobalVars();



		// clear objects...
		delete scope_factory;
		delete script_function_factory;
		delete native_function_factory;
		delete script_class_factory;
		delete eval;



		main_function = NULL;
		main_object = NULL;
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
				delete (CStringScriptVariable *)icv->varRef;
				break;

			}

			delete icv;
		}

		m_contantPool.clear();

	}

}
