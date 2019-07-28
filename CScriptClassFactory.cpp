#include "zetscript.h"

namespace zetscript{

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

		 std::vector<CScriptClass *> *vec_script_class_node=getVectorScriptClassNode();
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



	void CScriptClassFactory::clear(){

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

			sci = new CScriptClass(this,vec_script_class_node.size());

			scope->setScriptClass(sci);


			sci->str_class_ptr_type = TYPE_SCRIPT_VARIABLE;

			sci->symbol_info.symbol=symbol;

			vec_script_class_node.push_back(sci);

			if(base_class != NULL){
				sci->idx_base_class.push_back(base_class->idx_class);
			}

			return sci;

		}else{
			THROW_RUNTIME_ERROR(string::sformat("class \"%s\" already registered",class_name.c_str()));
		}

		return NULL;
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
			if(class_type == vec_script_class_node.at(i)->str_class_ptr_type){//metadata_info.object_info.symbol_info.c_type){
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

	CScriptVariable *		CScriptClassFactory::instanceScriptVariableByClassName(const std::string & class_name){

		 // 0. Search class info ...
		 CScriptClass * rc = getScriptClass(class_name);

		 if(rc != NULL){
			 return instanceScriptVariableByIdx(rc->idx_class);
		 }

		 return NULL;
	 }

	 CScriptVariable 		 * CScriptClassFactory::instanceScriptVariableByIdx(unsigned char idx_class, void * value_object){

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

	unsigned char CScriptClassFactory::getIdx_C_RegisteredClass(const std::string & str_classPtr){
		// ok check c_type
		for(unsigned i = 0; i < vec_script_class_node.size(); i++){
			if(vec_script_class_node[i]->str_class_ptr_type == str_classPtr){
				return i;
			}
		}
		return ZS_INVALID_CLASS;
	}


	intptr_t CScriptClassFactory::doCast(intptr_t obj, unsigned char idx_src_class, unsigned char idx_convert_class){//c_class->idx_class,idx_return_type){

		CScriptClass *src_class = getScriptClass(idx_src_class);
		CScriptClass *convert_class = getScriptClass(idx_convert_class);

		//local_map_type_conversion
		if(mapTypeConversion.count(idx_src_class) == 0){
			THROW_RUNTIME_ERROR(string::sformat("There's no type src conversion class \"%s\".",rtti::demangle(src_class->str_class_ptr_type).c_str()));
			return 0;
		}

		if((mapTypeConversion)[idx_src_class].count(idx_convert_class) == 0){
			THROW_RUNTIME_ERROR(string::sformat("There's no dest conversion class \"%s\".",rtti::demangle(convert_class->str_class_ptr_type).c_str()));
			return 0;
		}

		return (mapTypeConversion)[idx_src_class][idx_convert_class](obj);
	}

	const char * CScriptClassFactory::getScriptClassName(unsigned char idx){
		if(idx != ZS_INVALID_CLASS){
			return vec_script_class_node[idx]->symbol_info.symbol->name.c_str();
		}
		 return "class_unknow";
	}


	unsigned char CScriptClassFactory::getIdxClassFromIts_C_TypeInternal(const std::string & c_type_str){

		// 1. we have to handle primitives like void, (int *), (bool *),(float *) and (std::string *).
		 // 2. Check for rest registered C classes...
		 for(unsigned i = 0; i < vec_script_class_node.size(); i++){
			 if(vec_script_class_node.at(i)->str_class_ptr_type==c_type_str)
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

		for(unsigned i=0; i < theClass->idx_base_class.size(); i++){
			if(isIdxClassInstanceOf(theClass->idx_base_class[i],class_idx)){
				return true;
			}
		}

		return false;
	}

	CScriptClassFactory::~CScriptClassFactory(){
		// we have to destroy all allocated constructor/destructor ...
		for (unsigned i = 0; i < vec_script_class_node.size(); i++) {

			delete vec_script_class_node.at(i);
		}

		vec_script_class_node.clear();
	}

}
