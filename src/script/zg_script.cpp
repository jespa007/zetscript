
#include "ast/CScope.cpp"
#include "ast/ast.cpp"
#include "CALE.cpp"
#include "CVirtualMachine.cpp"
#include "CCompiler.cpp"
#include "CScriptClassFactory.cpp"

#include "CScriptFunction.cpp"



CZG_Script * CZG_Script::m_instance = NULL;




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

CObject * CZG_Script::call_C_1p(int fp,CObject *obj_arg){




	if(fp==0){
		print_error_cr("Null function");
		return NULL;
	}

	if(this->m_c_arg.size() != 1){
		print_error_cr("Argument doestn't match");
		return NULL;
	}

	//
	fntConversionType fun1=getConversionType(obj_arg->getPointerClassStr(),m_c_arg[0]);
	int result=0;

	if(fun1 != NULL){
		// Normalize argument ...
		int ptr_arg = fun1(obj_arg);

		((int (*)(int))fp)(ptr_arg);

		/*asm(
				"push %[p1]\n\t"
				"call *%P0\n\t" // call function
				"add $4,%%esp"  // Clean up the stack (you must multiply 4*n_arguments here).
				: "=a" (result) // The result code from puts.
				: "r"(fun1),[p1] "r"(ptr_arg)
		);*/

		//float h = (float)result;

		print_info_cr("hh:%i",result);


	}
	return NULL;

}

CObject * CZG_Script::call_C_0p(int fp){


	if(fp==0){
		print_error_cr("Null function");
		return NULL;
	}
	// Normalize argument ...

	int result=0;

	((int (*)())fp)();

	// convert result to object ...


	return (CObject *)result;


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
			print_error_cr("Not found. Must add object type into factory");
			break;

		}
	}
	return NULL;
}


void  print(string * s){
	print_info_cr("dada:%s",s->c_str());
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

	//-----------------------
	// move into factory ...
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

	//main_context = new CContext();

	// register Main Class function

	//CScriptClassFactory::registerClass("Main");

	// register c function's
	registerGlobal_C_Function(print);

	// register var
	registerGlobal_C_Variable(&interface_variable);

	//typeid(interface_variable).name();


	CFactoryContainer::getInstance()->registerScriptFunctions();
	CVirtualMachine::getInstance();
}

bool CZG_Script::eval(const string & s){



	// create main scope ...
	irfs.symbol_info.scope = new CScope();


	return CCompiler::getInstance()->compile(s, &m_mainFunction);
}

bool CZG_Script::execute(){


	//CCompiler::getInstance()->printGeneratedCode(m_mainFunction);

	return CVirtualMachine::getInstance()->execute(m_mainFunction,0);//->excute();
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
