#include "CSharedPointerManagement.cpp"
#include "ast/CScopeInfo.cpp"
#include "ast/ast.cpp"
#include "CALE.cpp"
#include "CVirtualMachine.cpp"
#include "CCompiler.cpp"
#include "CScriptClassFactory.cpp"

#include "CScriptClass.cpp"
#include "CScriptFunction.cpp"



CZG_Script * CZG_Script::m_instance = NULL;
CAst *CZG_Script::m_ast = NULL;





CZG_Script * CZG_Script::getInstance(){
	if(m_instance==NULL){
		m_instance = new CZG_Script();
		m_instance->init();
	}

	return m_instance;
}

void CZG_Script::destroy(){

	if(m_instance!=NULL){
		delete m_instance;
	}

	CFactoryContainer::destroySingletons();

}

int CZG_Script::call_C_1p(int fp,int ptr_arg){//CObject *obj_arg){





		// Normalize argument ...
		//int ptr_arg = fun1(obj_arg);

		int result = ((int (*)(int))fp)(ptr_arg);

		/*asm(
				"push %[p1]\n\t"
				"call *%P0\n\t" // call function
				"add $4,%%esp"  // Clean up the stack (you must multiply 4*n_arguments here).
				: "=a" (result) // The result code from puts.
				: "r"(fun1),[p1] "r"(ptr_arg)
		);*/

		//float h = (float)result;

		print_info_cr("hh:%i",result);



	return result;

}

int CZG_Script::call_C_0p(int fp){


	if(fp==0){
		print_error_cr("Null function");
		return 0;
	}
	// Normalize argument ...

	int result=0;

	result = ((int (*)())fp)();

	// convert result to object ...


	return result;


}

bool CZG_Script::call_C_function(tInfoRegisteredFunctionSymbol *irfs, int & result, vector<CObject *> * argv){

	int converted_param[MAX_PARAM_C_FUNCTION];

	if((irfs->object_info.symbol_info.properties & SYMBOL_INFO_PROPERTIES::C_OBJECT_REF) != SYMBOL_INFO_PROPERTIES::C_OBJECT_REF) {
		print_error_cr("Function is not registered as C");
		return false;
	}

	int fp = irfs->object_info.symbol_info.ref_aux;

	if(fp==0){
		print_error_cr("Null function");
		return false;
	}

	if(irfs->m_arg.size() != argv->size()){
		print_error_cr("C argument VS scrip argument doestn't match sizes");
		return false;
	}

	if(irfs->m_arg.size() >= MAX_PARAM_C_FUNCTION){
		print_error_cr("Reached max param for C function (Current: %i Max Allowed: %i)",irfs->m_arg.size(),MAX_PARAM_C_FUNCTION);
		return false;
	}

	// convert parameters script to c...
	for(unsigned int i = 0; i < argv->size();i++){
		fntConversionType paramConv=CScriptClassFactory::getInstance()->getConversionType((argv->at(i))->getPointerClassStr(),irfs->m_arg[i]);

		if(paramConv == NULL){
			return false;
		}

		converted_param[i] = paramConv(argv->at(i));
	}

	switch(argv->size()){
	default:
		print_error_cr("cannot call !");
		return false;
	case 0:
		result=call_C_0p(fp);
		print_info_cr("0 call!");
		break;
	case 1:
		result=call_C_1p(fp,converted_param[0]);
		print_info_cr("1 call!");
		break;

	}

	return true;

}
/*
CZG_Script::tLocalScope * CZG_Script::createLocalScope(CZG_Script::tLocalScope *m_parent){
	tLocalScope * localScope = new tLocalScope;

	localScope->m_parentScope = m_parent;

	return localScope;
}

CZG_Script::tContext * CZG_Script::createContext(){

	tContext *aux= new tContext();

	aux->base_scope = createLocalScope(NULL);

	return aux;
}
*/

class CCustomObject{
	int x;
public:
	CCustomObject(){
		printf("CCustomObject constructing point @%p\n", this);
	}
	void member(){

		printf("hola %p!\n",this);
	}

	int member2(int i1, int i2){ return 0;}

	   virtual ~CCustomObject()
	   {
	       printf("CCustomObject @%p destroyed\n", this);
	   }
};

/*
bool CZG_Script::object2float(CObject *obj, float & v){

	// only float ...
	CNumber *n=dynamic_cast<CNumber *>(obj);
	if(n!=NULL){
		v=n->m_value;
		return true;
	}

	return false;
}

bool CZG_Script::object2int(CObject *obj, int & v){
	// only float ...
	CInteger *n=dynamic_cast<CInteger *>(obj);
	if(n!=NULL){
		v=n->m_value;
		return true;
	}

	return false;
}

bool CZG_Script::object2bool(CObject *obj, bool & v){
	// only float ...
	CBoolean *n=dynamic_cast<CBoolean *>(obj);
	if(n!=NULL){
		v=n->m_value;
		return true;
	}

	return false;
}

bool CZG_Script::object2string(CObject *obj, string & v){
	// only float ...
	CString *n=dynamic_cast<CString *>(obj);
	if(n!=NULL){
		v=n->m_value;
		return true;
	}

	return false;
}


CObject * CZG_Script::createObjectFromPrimitiveType(CZG_Script::tPrimitiveType *pt){

	if(pt != NULL){
		switch(pt->id){
		case C_TYPE_VAR::VOID_TYPE:
			return CScopeInfo::VoidSymbol;
			break;
		case C_TYPE_VAR::STRING_TYPE:
			return (CObject *)NEW_STRING();
			break;
		case C_TYPE_VAR::FLOAT_TYPE:
			return (CObject *)NEW_NUMBER();
			break;
		case C_TYPE_VAR::INT_TYPE:
			return (CObject *)NEW_INTEGER();
			break;
		case C_TYPE_VAR::BOOL_TYPE:
			return (CObject *)NEW_BOOLEAN();
			break;
		default:
			print_error_cr("Not found. Must add object type into factory");
			break;

		}
	}
	return NULL;
}
*/

void  print(string * s){
	print_info_cr("dada:%s",s->c_str());
}

CZG_Script::CZG_Script(){

	//registerFunction(&CCustomObject::member2);
	// call_function("print");

}

int interface_variable;



void CZG_Script::init(){

	CScriptClassFactory::registerPrimitiveTypes();

	iniFactory<CNumberFactory>("CNumber");
	iniFactory<CIntegerFactory>("CInteger");
	iniFactory<CBooleanFactory>("CBoolean");
	iniFactory<CStringFactory>("CString");
	iniFactory<CUndefinedFactory>("CUndefined");
	iniFactory<CVectorFactory>("CVector");



	m_ast = CAst::getInstance();

	// setup main struct...
	m_structInfoMain.object_info.symbol_info.ast=m_ast->getMainAstNode();


	//-----------------------
	// Conversion from object types to primitive types (move into factory) ...
	addTypeConversion<CInteger *,int>( [] (CObject *obj){return ((CInteger *)obj)->m_value;});
	addTypeConversion<CInteger *,int *>( [] (CObject *obj){return (int)&((CInteger *)obj)->m_value;});
	addTypeConversion<CInteger *,string *>( [] (CObject *obj){obj->toString();return (int)&obj->m_strValue;});

	addTypeConversion<CNumber *,float *>( [] (CObject *obj){return (int)(&((CNumber *)obj)->m_value);});
	addTypeConversion<CNumber *,int>( [] (CObject *obj){return (int)((CNumber *)obj)->m_value;});
	addTypeConversion<CNumber *,string *>( [] (CObject *obj){obj->toString();return (int)&obj->m_strValue;});

	addTypeConversion<CBoolean *,bool *>( [] (CObject *obj){return (int)&((CBoolean *)obj)->m_value;});
	addTypeConversion<CBoolean *,string *>( [] (CObject *obj){obj->toString();return (int)&obj->m_strValue;});

	addTypeConversion<CString *,string *>( [] (CObject *obj){return (int)&(((CString *)obj)->m_value);});

	// move into factory ...
	//-----------------------

	// ok register CInteger through CScriptClass...
	CScriptClassFactory::getInstance()->register_C_Class<CInteger>("CInteger");
	CScriptClassFactory::getInstance()->register_C_FunctionMember<CInteger>("toString",&CInteger::toString);
	//CScriptClassFactory::getInstance()->register_C_VariableMember<CInteger,CInteger::>("toString");


	//main_context = new CContext();

	// register Main Class function

	//CScriptClassFactory::registerClass("Main");

	// register c function's
	registerGlobal_C_Function(print);

	// register var
	//registerGlobal_C_Variable(&interface_variable);

	//typeid(interface_variable).name();

	// create main global scope ...
	//m_mainFunctionInfo.global_scope = new CScopeInfo();

	//m_globalScope = new CScopeInfo();


	CFactoryContainer::getInstance()->registerScriptFunctions();
	CVirtualMachine::getInstance();

}

bool CZG_Script::eval(const string & s){


	// generate whole AST

	if(m_ast->parse(s.c_str())){

		if(CCompiler::getInstance()->compile(m_ast->getMainAstNode(),&m_structInfoMain)){
			// print generated asm ...
			CCompiler::printGeneratedCode(&m_structInfoMain.object_info);
			return true;
		}
		// then you have all information -> compile into asm!
		//generateAsmCode(root);
	}

	return false;

	//return CCompiler::getInstance()->compile(s, &m_structInfoMain);
}


bool CZG_Script::execute(){
	if(m_mainClass == NULL){
		// creates the main entry function with compiled code. On every executing code, within "execute" function
		// virtual machine is un charge of allocating space for all local variables...
		m_mainClass = new CScriptClass(&m_structInfoMain);//CScriptClassFactory::newClass("Main");
		m_mainFunction = new CScriptFunction(m_mainClass,&m_structInfoMain);
	}

	//CCompiler::getInstance()->printGeneratedCode(m_mainClass);

	// the first code to execute is the main function that in fact is a special member function inside our main class
	return CVirtualMachine::getInstance()->execute(m_mainFunction, NULL,0);//->excute();
}


//-------------------------------------------------------------------------------------
CZG_Script::~CZG_Script(){
	// unregister operators ...

	//delete m_mainClass;
	delete m_mainClass;
	delete m_mainFunction;

	CCompiler::destroySingletons();
	CVirtualMachine::destroySingletons();
	CAst::destroySingletons();
	CScopeInfo::destroySingletons();

}
