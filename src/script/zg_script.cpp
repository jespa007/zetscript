
#include "ast.cpp"
#include "CVirtualMachine.cpp"
#include "CCompiler.cpp"
#include "CScope.cpp"
#include "CScriptFunction.cpp"


CZG_Script::tPrimitiveType CZG_Script::primitiveType[MAX_VAR_C_TYPES];
CZG_Script * CZG_Script::m_instance = NULL;


void CZG_Script::registerPrimitiveTypes(){

	primitiveType[VOID_TYPE]={typeid(void).name(),"void",VOID_TYPE};
	primitiveType[INT_TYPE]={typeid(int).name(),"int",INT_TYPE};
	primitiveType[INT_PTR_TYPE]={typeid(int *).name(),"int *",INT_PTR_TYPE};
	primitiveType[FLOAT_TYPE]={typeid(float).name(),"float",FLOAT_TYPE};
	primitiveType[FLOAT_PTR_TYPE]={typeid(float *).name(),"float *",FLOAT_PTR_TYPE};
	primitiveType[STRING_TYPE]={typeid(string).name(),"string",STRING_TYPE};
	primitiveType[STRING_PTR_TYPE]={typeid(string *).name(),"string *",STRING_PTR_TYPE};
	primitiveType[BOOL_TYPE]={typeid(bool).name(),"bool",BOOL_TYPE};
	primitiveType[BOOL_PTR_TYPE]={typeid(bool *).name(),"bool *",BOOL_PTR_TYPE};
}


CZG_Script::tPrimitiveType *CZG_Script::getPrimitiveTypeFromStr(const string & str){

	for(unsigned i=0; i < MAX_VAR_C_TYPES; i++){
		if(primitiveType[i].type_str == str){
			return &primitiveType[i];
		}
	}

	print_error_cr("type \"%s\" is not registered",str.c_str());

	return NULL;
}

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
			return CScope::VoidSymbol;
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
			print_error_cr("Not found");
			break;

		}
	}
	return NULL;
}


void  print(float caca){
	print_info_cr("Hola!");
}

CZG_Script::CZG_Script(){
	registerPrimitiveTypes();
	//registerFunction(&CCustomObject::member2);
	// call_function("print");
}

int interface_variable;



void CZG_Script::init(){

	iniFactory<CNumberFactory>("CNumber");
	iniFactory<CIntegerFactory>("CInteger");
	iniFactory<CBooleanFactory>("CBoolean");
	iniFactory<CStringFactory>("CString");
	iniFactory<CUndefinedFactory>("CUndefined");
	iniFactory<CVectorFactory>("CVector");

	CScope::createSingletons();
	CAst::createSingletons();

	//main_context = new CContext();

	m_mainFunction = new CScriptFunction();

	// register c function's
	registerFunction(print);

	// register var
	registerVariable(&interface_variable);

	//typeid(interface_variable).name();


	CFactoryContainer::getInstance()->registerScriptFunctions();
	CVirtualMachine::getInstance();
}

bool CZG_Script::eval(const string & s){
	return CCompiler::getInstance()->compile(s, m_mainFunction);
}

bool CZG_Script::execute(){


	//CCompiler::getInstance()->printGeneratedCode(m_mainFunction);

	return CVirtualMachine::getInstance()->execute(m_mainFunction);//->excute();
}


//-------------------------------------------------------------------------------------
CZG_Script::~CZG_Script(){
	// unregister operators ...

	delete m_mainFunction;


	CCompiler::destroySingletons();
	CVirtualMachine::destroySingletons();
	CAst::destroySingletons();
	CScope::destroySingletons();

}
