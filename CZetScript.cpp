#include "zetscript.h"

#define REGISTER_BUILT_IN_CLASS(type_class, idx_class)\
	if(vec_script_class_node.size()!=idx_class){\
		THROW_RUNTIME_ERROR(string::sformat("Error: class built in type %s doesn't match its id",STR(type_class)));\
		return;\
	}\
	register_C_Class<type_class>(STR(type_class));


#define REGISTER_BUILT_IN_TYPE(type_class, idx_class)\
	if(vec_script_class_node.size()!=idx_class){\
		THROW_RUNTIME_ERROR(string::sformat("Error initializing C built in type: %s",STR(type_class)));\
		return;\
	}else{\
		CScriptClass *sc=registerClass(__FILE__,__LINE__,STR(type_class),"");\
		GET_SCOPE(sc->symbol_info.symbol->idxScope)->is_c_node=true;\
		vec_script_class_node.at(idx_class)->symbol_info.properties=SYMBOL_INFO_PROPERTY::PROPERTY_C_OBJECT_REF;\
		vec_script_class_node.at(idx_class)->classPtrType=(typeid(type_class).name());\
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
		std::vector<CScriptFunction *> * m_vf = &sfo->m_function;

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
			 for(unsigned f = 0; f < rc->m_function.size(); f++){
				 printGeneratedCode(rc->m_function[f]);
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

			if(INT_PTR_TYPE_STR==ir_var->c_type){
				return {

						0,
						ptr_variable,
						STK_PROPERTY_TYPE_INTEGER|STK_PROPERTY_IS_C_VAR
				};

			}else if(FLOAT_PTR_TYPE_STR==ir_var->c_type){
				return {

						0,
						ptr_variable,
						STK_PROPERTY_TYPE_NUMBER|STK_PROPERTY_IS_C_VAR
				};
			}else if(CONST_CHAR_PTR_TYPE_STR==ir_var->c_type){

				return {

						ptr_variable,
						0,
						STK_PROPERTY_TYPE_STRING
				};
			}else if(STRING_PTR_TYPE_STR==ir_var->c_type){

				return {

						(void *)((std::string *)ptr_variable)->c_str(),
						ptr_variable,
						STK_PROPERTY_TYPE_STRING|STK_PROPERTY_IS_C_VAR
				};
			}else if(BOOL_PTR_TYPE_STR==ir_var->c_type){
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




	void  internal_print_error(const char *s){
		 virtual_machine->setError(s);
	}


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

	 void CZetScript::setup(){
		 CScriptClass *main_class=NULL;
		register_c_base_symbols=false;
		//mapTypeConversion = new std::map<int, std::map<int, fntConversionType>>();

		VOID_TYPE_STR = typeid(void).name();
		INT_PTR_TYPE_STR = typeid(int *).name();
		FLOAT_PTR_TYPE_STR = typeid(float *).name();
		CONST_CHAR_PTR_TYPE_STR = typeid(const char *).name();
		STRING_PTR_TYPE_STR = typeid(std::string *).name();
		BOOL_PTR_TYPE_STR = typeid(bool *).name();

		// particular case for return ... for not to force user returning pointer (this is the only vars allowed, sorry!)
		BOOL_TYPE_STR =  typeid(bool).name();
		INT_TYPE_STR =  typeid(int).name();
		UNSIGNED_INT_TYPE_STR =  typeid(unsigned int).name();
		INTPTR_T_TYPE_STR =  typeid(intptr_t).name();
		FLOAT_TYPE_STR =  typeid(float).name();
		STACK_ELEMENT_STR=  typeid(StackElement *).name();

		//------------------------------------------------------------------------------------------------------------------------------------------------------------------

		// REGISTER BUILT IN SCRIPT TYPES
		// MAIN CLASS (0)
		main_class=registerClass(__FILE__, __LINE__,MAIN_SCRIPT_CLASS_NAME,""); // 0
		if(main_class->symbol_info.symbol->idxScope!=IDX_GLOBAL_SCOPE){
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
		REGISTER_BUILT_IN_CLASS(StackElement,IDX_STACK_ELEMENT);
		REGISTER_BUILT_IN_CLASS(CScriptVariable,IDX_CLASS_SCRIPT_VAR);
		REGISTER_BUILT_IN_CLASS(CStringScriptVariable,IDX_CLASS_STRING);
		REGISTER_BUILT_IN_CLASS(CVectorScriptVariable,IDX_CLASS_VECTOR);
		REGISTER_BUILT_IN_CLASS(CFunctorScriptVariable,IDX_CLASS_FUNCTOR);
		REGISTER_BUILT_IN_CLASS(CStructScriptVariable,IDX_CLASS_STRUCT);


		//------------------------------------------------------------------------------------------------------------------------------------------------------------------

		// rgister basic classes (Warning!!! must match idx !!! and the order is important!!!)

		//----------------------------------------------------------------------
		// From here you defined all basic, start define hierarchy

		// register custom functions ...
		class_C_BaseOf<CVectorScriptVariable,CScriptVariable>();
		class_C_BaseOf<CFunctorScriptVariable,CScriptVariable>();
		class_C_BaseOf<CStructScriptVariable,CScriptVariable>();


		//------------------------------------------------------------------------------------------------------------
		// Let's register functions,...
		// register c function's

		main_class->registerFunctionMember(__FILE__,__LINE__,MAIN_SCRIPT_FUNCTION_NAME);

		register_C_Function("print",print);
		register_C_Function("error",internal_print_error);

		register_C_FunctionMember<CVectorScriptVariable>("size",&CVectorScriptVariable::size);
		register_C_FunctionMember<CVectorScriptVariable>("push",static_cast<void (CVectorScriptVariable:: *)(StackElement *)>(&CVectorScriptVariable::push));
		register_C_FunctionMember<CVectorScriptVariable>("pop",&CVectorScriptVariable::pop);


		register_C_FunctionMember<CStructScriptVariable>("add",&CStructScriptVariable::add_attr);
		register_C_FunctionMember<CStructScriptVariable>("remove",&CStructScriptVariable::remove_attr);
		register_C_FunctionMember<CStructScriptVariable>("size",&CStructScriptVariable::size);
	 }


	void (* CZetScript::print_out_callback)(const char *) = NULL;


	unsigned char CZetScript::getIdxClassFromIts_C_TypeInternal(const std::string & c_type_str){

		// 1. we have to handle primitives like void, (int *), (bool *),(float *) and (std::string *).
		 // 2. Check for rest registered C classes...
		 for(unsigned i = 0; i < vec_script_class_node.size(); i++){
			 if(vec_script_class_node.at(i)->classPtrType==c_type_str)
			 {
				 return i;
			 }
		 }

		 return ZS_INVALID_CLASS;
	 }

	unsigned char 			CZetScript::getIdxClassFromIts_C_Type(const std::string & c_type_str){
		return CZetScript::getIdxClassFromIts_C_TypeInternal(c_type_str);
	}

	bool 	CZetScript::isIdxClassInstanceOf(unsigned char idxSrcClass, unsigned char class_idx){

		if(idxSrcClass == class_idx){
			return true;
		}

		CScriptClass * theClass = vec_script_class_node.at(idxSrcClass);

		for(unsigned i=0; i < theClass->idxBaseClass.size(); i++){
			if(isIdxClassInstanceOf(theClass->idxBaseClass[i],class_idx)){
				return true;
			}
		}

		return false;
	}

	void CZetScript::register_C_BaseSymbols(bool _register){
		register_c_base_symbols = _register;
	}


	CScriptClass * CZetScript::registerClass(const std::string & file, short line,const std::string & class_name, const std::string & base_class_name){
		unsigned char  index;
		CScriptClass *sci=NULL;


		if(vec_script_class_node.size()>=MAX_REGISTER_CLASSES){
			THROW_RUNTIME_ERROR(string::sformat("Max register classes reached (Max:%i)",MAX_REGISTER_CLASSES));
			return NULL;
		}


		if(class_name.empty()){
			THROW_RUNTIME_ERROR("Class name empty");
			return NULL;
		}

		CScriptClass *base_class=NULL;
		if(base_class_name != ""){
			if((base_class = getScriptClass(base_class_name)) == NULL){
				return NULL;
			}
		}

		if((index = getIdxScriptClass_Internal(class_name))==ZS_INVALID_CLASS){ // check whether is local var registered scope ...

			// NEW SCOPE C and register ...
			//unsigned char idx_class=(unsigned char)vec_script_class_node.size()-1;

			CScope * scope = NEW_SCOPE();

			Symbol *symbol=scope->registerSymbol(file,line,class_name, NO_PARAMS_IS_CLASS);
			if(symbol == NULL){
				return NULL;
			}

			sci = new CScriptClass(vec_script_class_node.size());

			scope->setScriptClass(sci);


			sci->classPtrType = TYPE_SCRIPT_VARIABLE;

			sci->symbol_info.symbol=symbol;

			vec_script_class_node.push_back(sci);

			if(base_class != NULL){
				sci->idxBaseClass.push_back(base_class->idx_class);
			}

			return sci;

		}else{
			THROW_RUNTIME_ERROR(string::sformat("class \"%s\" already registered",class_name.c_str()));
		}

		return NULL;
	}

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

	std::vector<CScriptClass *> * CZetScript::getVectorScriptClassNode(){
		return & vec_script_class_node;
	}

	std::map<unsigned char, std::map<unsigned char, fntConversionType>>  *	 CZetScript::getMapTypeConversion() {
		return & mapTypeConversion;
	}

	CScriptClass 	* CZetScript::getScriptClass(unsigned char idx){
		if(idx == ZS_INVALID_CLASS){
			THROW_RUNTIME_ERROR("CScriptClass node out of bound");
			return NULL;
		}

		return vec_script_class_node.at(idx);
	}

	CScriptClass 	* CZetScript::getScriptClass(const std::string & class_name){
		unsigned char index;
		if((index = getIdxScriptClass_Internal(class_name))!=ZS_INVALID_CLASS){ // check whether is local var registered scope ...
			return vec_script_class_node.at(index);
		}

		return NULL;
	}

	CScriptClass *CZetScript::getScriptClassBy_C_ClassPtr(const std::string & class_type){

		for(unsigned i = 0; i < vec_script_class_node.size(); i++){
			if(class_type == vec_script_class_node.at(i)->classPtrType){//metadata_info.object_info.symbol_info.c_type){
				return vec_script_class_node.at(i);
			}
		}
		return NULL;
	}

	unsigned char CZetScript::getIdxScriptClass_Internal(const std::string & class_name){

		for(unsigned i = 0; i < vec_script_class_node.size(); i++){
			if(class_name == vec_script_class_node.at(i)->symbol_info.symbol->name){
				return i;
			}
		}
		return ZS_INVALID_CLASS;
	}

	bool CZetScript::isClassRegistered(const std::string & v){
		return getIdxScriptClass_Internal(v) != ZS_INVALID_CLASS;
	}


	void CZetScript::clear(){
		bool end=false;
		do{
			CScriptClass * sc = vec_script_class_node.at(vec_script_class_node.size()-1);
			end=(sc->symbol_info.properties & PROPERTY_C_OBJECT_REF) == PROPERTY_C_OBJECT_REF;

			if(!end){

				delete sc;
				vec_script_class_node.pop_back();

			}

		}while(!end);
	}


	CScriptVariable *		CZetScript::instanceScriptVariableByClassName(const std::string & class_name){

		 // 0. Search class info ...
		 CScriptClass * rc = getScriptClass(class_name);

		 if(rc != NULL){
			 return instanceScriptVariableByIdx(rc->idx_class);
		 }

		 return NULL;
	 }

	 CScriptVariable 		 * CZetScript::instanceScriptVariableByIdx(unsigned char idx_class, void * value_object){

		 CScriptVariable *class_object=NULL;

		 // 0. Search class info ...
		 CScriptClass *rc = getScriptClass(idx_class);

		 if(rc != NULL){

			 // Is a primitive ?
			 switch(rc->idx_class){

			 case IDX_TYPE_VOID_C:
			 case IDX_TYPE_INT_PTR_C:
			 case IDX_TYPE_FLOAT_PTR_C:
			 case IDX_TYPE_STRING_PTR_C:
			 case IDX_TYPE_BOOL_PTR_C:
				 THROW_RUNTIME_ERROR("Internal error");
				 return NULL;
				 break;

			 case IDX_CLASS_VECTOR:
			 case IDX_CLASS_STRUCT:
				 class_object = (CScriptVariable *)value_object;
				 break;
			 default:
				 class_object = new CScriptVariable(zs);
				 class_object->init(rc, value_object);
				 break;
			 }
		 }
		 return class_object;
	 }

	/**
	 * Register C variable
	 */
	 VariableSymbolInfo *  CZetScript::register_C_Variable(const std::string & var_name,void * var_ptr, const std::string & var_type)
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

		if(getIdxClassFromIts_C_Type(var_type) == ZS_INVALID_CLASS){
			THROW_RUNTIME_ERROR(string::sformat("%s has not valid type (%s)",var_name.c_str(),var_type.c_str()));
			return  NULL;
		}


		if((irs = main_function->registerVariable(__registered_file__,__registered_line__,var_name,var_type,(intptr_t)var_ptr,::PROPERTY_C_OBJECT_REF | PROPERTY_STATIC_REF)) != NULL){

			zs_print_debug_cr("Registered variable name: %s",var_name.c_str());

			return irs;
		}

		return NULL;
	}

	unsigned char CZetScript::getIdx_C_RegisteredClass(const std::string & str_classPtr){
		// ok check c_type
		for(unsigned i = 0; i < vec_script_class_node.size(); i++){
			if(vec_script_class_node[i]->classPtrType == str_classPtr){
				return i;
			}
		}
		return ZS_INVALID_CLASS;
	}


	intptr_t CZetScript::doCast(intptr_t obj, unsigned char idx_src_class, unsigned char idx_convert_class){//c_class->idx_class,idx_return_type){

		CScriptClass *src_class = getScriptClass(idx_src_class);
		CScriptClass *convert_class = getScriptClass(idx_convert_class);

		//local_map_type_conversion
		if(mapTypeConversion.count(idx_src_class) == 0){
			THROW_RUNTIME_ERROR(string::sformat("There's no type src conversion class \"%s\".",rtti::demangle(src_class->classPtrType).c_str()));
			return 0;
		}

		if((mapTypeConversion)[idx_src_class].count(idx_convert_class) == 0){
			THROW_RUNTIME_ERROR(string::sformat("There's no dest conversion class \"%s\".",rtti::demangle(convert_class->classPtrType).c_str()));
			return 0;
		}

		return (mapTypeConversion)[idx_src_class][idx_convert_class](obj);
	}

	const char * CZetScript::getScriptClassName(unsigned char idx){
		if(idx != ZS_INVALID_CLASS){
			return vec_script_class_node[idx]->symbol_info.symbol->name.c_str();
		}
		 return "class_unknow";
	}



	void CZetScript::clear(){

		virtual_machine->clearGlobalVars();

		main_function = this->script_function_factory->getScriptFunction(IDX_MAIN_FUNCTION);

		// clean main functions ... remove script functions and leave c functions...
		for (unsigned f = 0;
			f < main_function->m_function.size()
			;) {
			// get function info
			CScriptFunction * local_function = main_function->m_function[f];

			if ((local_function->symbol_info.properties & PROPERTY_C_OBJECT_REF) != PROPERTY_C_OBJECT_REF) {
				main_function->m_function.erase(main_function->m_function.begin() + f);
			}
			else {
				f++;
			}

		}

		// remove c variables ...
		for (unsigned v = 0;
			v < main_function->m_variable.size(); ) {

			if ((main_function->m_variable[v].properties & PROPERTY_C_OBJECT_REF) != PROPERTY_C_OBJECT_REF) {

				main_function->m_variable.erase(main_function->m_variable.begin() + v);

			}
			else {
				v++;
			}
		}

		// remove scope vars...
		_scope_factory->clear();
		script_function_factory->clear();
		_script_class_factory->clear();

	}


	CZetScript::CZetScript(){

		scope_factory = new CScopeFactory();
		native_function_factory = new CNativeFunctionFactory();
		script_function_factory= new CScriptFunctionFactory();
		eval = new CEval();

		virtual_machine = new CVirtualMachine();

		main_object=NULL;
		main_function=NULL;

		eval_int=0;
		eval_float=0;
		eval_string="";
		eval_bool = false;
		register_c_base_symbols=false;
		zs=this;

	}


	int * CZetScript::evalIntValue(const std::string & str_to_eval){


		if(!evalString(str_to_eval)){
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

		if(!evalString(str_to_eval)){
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

		if(!evalString(str_to_eval)){
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


		if(!evalString(str_to_eval)){
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

	bool CZetScript::evalString(const std::string & expression, bool exec_vm, const char *filename, bool show_bytecode)  {


		if(!CZetScript::evalString(expression)){
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

	bool CZetScript::evalFile(const std::string & filename, bool exec_vm, bool show_bytecode){
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
					eval->evalString(buf_tmp,error);
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
			THROW_RUNTIME_ERROR(string::sformat("Filename \"%s\" already parsed",filename.c_str()));
			error=true;
		}

		if(buf_tmp!=NULL){
			free(buf_tmp);
		}

		if(show_bytecode){
			printGeneratedCode();
		}

		if(exec_vm){
			execute();
		}

		return !error;
	}


	CZetScript::~CZetScript(){

		virtual_machine->clearGlobalVars();



		// clear objects...
		delete scope_factory;
		delete script_function_factory;
		delete native_function_factory;
		delete eval;

		// we have to destroy all allocated constructor/destructor ...
		for (unsigned i = 0; i < vec_script_class_node.size(); i++) {

			delete vec_script_class_node.at(i);
		}

		vec_script_class_node.clear();

		main_object = NULL;
		main_function = NULL;
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
