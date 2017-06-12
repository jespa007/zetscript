#include "CZetScript.h"

#define N_INTS 1000000000

int print_hello(int hh){
	return hh+1;
}
/*
class Prova{



public:

	int i;

	Prova(){
		i=0;
	}

	Prova(int h){
		i=h;
	}

	static Prova * _add(Prova *v1, Prova * v2){


		return new Prova(v1->i+ v2->i);
	}

};


intptr_t  _add(intptr_t v1, intptr_t  v2){
		printf("%p %p\n",v1,v2);

		return (intptr_t)(new Prova(v1+ v2));
	}
*/
int main(int argc, char * argv[]){

	/*using namespace std::placeholders;

	printf("HOLA\n");

	//intptr_t fun_ptr=(intptr_t)&_add;
	void * proxy_function=(void *)( new std::function<intptr_t (intptr_t, intptr_t )>(std::bind((intptr_t (*)(intptr_t,intptr_t))_add, _1,_2)));

	Prova *v1=new Prova(5);
	Prova *v2=new Prova(2);

	printf("HOLA %p %p\n",v1,v2);

	intptr_t p=(*((std::function<intptr_t (intptr_t,intptr_t )> *)proxy_function))(
			(intptr_t)v1,
			(intptr_t)v2
							);

	printf("result %i\n",((Prova *)p)->i);
	return 0;
*/
/*	vector<char> vp1;
	char *vp2= (char *)malloc(N_INTS);
	struct timeval start, end;


	for(int i = 0; i < N_INTS; i++){
		vp1.push_back(0);
	}

	gettimeofday(&start, NULL);
	int result=0;
	// compare method 1...
	for(int i = 0; i < N_INTS; i++){

		for(int j = 0; j < N_INTS; j++){

			for(int k = 0; k < N_INTS; k++){
				result+=print_hello(vp1[i]+vp1[j]+vp1[k]+1);
			}
		}
	}




	gettimeofday(&end, NULL);
	printf("\nTime vector:%lu",end.tv_usec-start.tv_usec);
	gettimeofday(&start, NULL);


	// compare method 2...
	for(int i = 0; i < N_INTS; i++){

		for(int j = 0; j < N_INTS; j++){

			for(int k = 0; k < N_INTS; k++){
				result+=print_hello(vp2[i]+vp2[j]+vp2[k]+1);
			}
		}
	}

	gettimeofday(&end, NULL);
	printf("\nTime array_c:%lu",end.tv_usec-start.tv_usec);
	return 0;*/

	print_info_cr("sizeof(tInfoAsmOp):%i",sizeof(tInfoAsmOp));
	print_info_cr("sizeof(CVirtualMachine):%i bytes",sizeof(CVirtualMachine));
	print_info_cr("sizeof(CAstNode):%i bytes",sizeof(CASTNode));
	print_info_cr("sizeof(CCompiler):%i bytes",sizeof(CCompiler));
	print_info_cr("sizeof(CScope):%i bytes",sizeof(CScope));
	print_info_cr("sizeof(CScriptVariable):%i bytes",sizeof(CScriptVariable));
	print_info_cr("sizeof(CScriptClass):%i bytes",sizeof(CScriptClass));
	print_info_cr("sizeof(tStackElement):%i bytes",sizeof(tStackElement));


	CLog::setUseAnsiEscape(true);
	CZetScript *zet_script = CZetScript::getInstance();








	if(argc < 2){
		printf("Put file to evaluate.\n");
		printf("\n");
		printf("Example:\n");
		printf("\n");
		printf("file.zs");
		printf("\n");
		printf("\n");
		return 0;
	}

	char *data_buffer = argv[1];
	ByteBuffer *buffer = NULL;


	if((buffer = CIO_Utils::readFile(argv[1]))!=NULL){

		data_buffer=(char *)buffer->data_buffer;
	}

	if(zet_script->eval(data_buffer)){
		zet_script->execute();


		/*auto f=zet_script->script_call("rec");

		if(f != NULL){
			CScriptVariable *r;
			CInteger *iu = new CInteger(10);
			r = (*f)({iu});
			if(r != UNDEFINED_SYMBOL){//CScriptVariable::UndefinedSymbol){
				delete r;
			}


			delete iu;
			delete f;
		}*/
	}

	if(buffer != NULL){
		delete buffer;
	}


	CZetScript::destroy();



#if defined(__DEBUG__) && defined(__MEMMANAGER__)
  MEM_ViewStatus();
#endif

	return 0;
}
