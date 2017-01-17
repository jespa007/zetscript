#include "core/zg_core.h"


#ifdef _WIN32
#ifdef main
#undef main
#endif
#endif


void print(const  string * s){
	printf("\njjjj %s",s->c_str());
}

/*
template<class v1,class v2, typename f>
int function(_f F){

	print("type:%s",typeid(v1).name);
}
*/
class CProva{
	string *i,*j;
public:
	CProva(){i=NULL;j=NULL;}
	//CProva(string *_i){i=_i;j=NULL;}
	//CProva(string *_i, string *_j){i=_i;j=_j;}


	void print(){
		if(i != NULL && j != NULL){
			printf("%s - %s", i->c_str(), j->c_str());
		}
	}

};

//template<F>
//std::function<void *(int,int)> *
/*
template <class _T, class _R,typename _F>
void * c_constructor_class_function_proxy(unsigned int n_args, _F fun_obj){
	using namespace std::placeholders;
	string return_type;
	vector<string >m_arg;
	using Traits3 = function_traits<decltype(fun_obj)>;
	getParamsFunction<Traits3>(0,return_type, m_arg, make_index_sequence<Traits3::arity>{});


	// check valid parameters ...
	if((irs.idx_return_type=getIdxClassFromIts_C_Type(return_type)) == -1){
		print_error_cr("Return type \"%s\" for function \"%s\" not registered",demangle(return_type).c_str(),function_name);
		return false;
	}

	for(unsigned int i = 0; i < m_arg.size(); i++){
		//if(getIdxClassFromIts_C_Type(irs.m_arg[i])==-1){
			print_info_cr("Argument (%i) type \"%s\"",i,demangle(m_arg[i]).c_str());
			//return false;
		//}

	}


	std::function<void *(void *,PROXY_CREATOR)> *c_function_builder=NULL;



	switch(n_args){
	case 0:
		c_function_builder = new std::function<void *(void *,PROXY_CREATOR)> ([fun_obj](void *obj,PROXY_CREATOR proxy_creator){

			void *return_val=NULL;

			if(proxy_creator == PROXY_CREATOR::CREATE_FUNCTION){
				return_val= (void *)(new std::function<_R ()>(std::bind((_R (_T::*)())(fun_obj), (_T *)obj)));
			}
			else if(proxy_creator == PROXY_CREATOR::DESTROY_FUNCTION){
				delete (std::function<_R ()> *)obj;
			}
			return return_val;
		});
		break;
	case 1:
		c_function_builder = new std::function<void *(void *,PROXY_CREATOR)> ([fun_obj](void *obj,PROXY_CREATOR proxy_creator){

			void *return_val=NULL;

			if(proxy_creator == PROXY_CREATOR::CREATE_FUNCTION){
				return_val=  (void *)(new std::function<_R (int)>(std::bind((_R (_T::*)(int))(fun_obj), (_T *)obj, _1)));
			}
			else if(proxy_creator == PROXY_CREATOR::DESTROY_FUNCTION){
				delete (std::function<_R (int)> *)obj;
			}
			return return_val;
		});
		break;
	case 2:
		print_error_cr("Max args 2!");
		break;

	}

	return NULL;
}
*/
/*
template <class TObject>
class DelayedConstructor
{
public:
    template <class ...Args>
    DelayedConstructor(Args ...args)
    {
        class ConstructHelper
        {
        public:
            static TObject* DoConstruct(Args ...args)
            {
                return new TObject(args...);
            }
        };
        m_funHelper = std::bind(ConstructHelper::DoConstruct, args...);
    }

    TObject* operator()()
    {
        return m_funHelper();
    }

protected:
    std::function<TObject*(void)> m_funHelper;
};


template <class _T, typename F>
bool register_C_FunctionMemberInt_Prova(const char *function_name,F function_type)
{
	string return_type;
	vector<string> params;
	tInfoRegisteredFunctionSymbol irs;
	string str_classPtr = typeid( _T *).name();


	// 1. check all parameters ok.
	using Traits3 = function_traits<decltype(function_type)>;
	getParamsFunction<Traits3>(0,return_type, irs.m_arg, make_index_sequence<Traits3::arity>{});
}

class A
{
public:
    A(int a, std::string str) { }
};*/

int main(int argc, char * argv[]){


	//string str_hola_merda;
	//string s1="h1", s2="h2";

	// 1. check all parameters ok.
	//using namespace std::placeholders;

	//std::function<void *()> * c_constructor = new std::function<void *()>([](){return new CProva;});
	//std::function<void *(int,int)> * c_constructor = new std::function<void *(int _i,int _j)>([](){return new CProva(_i,_j);});;
	//return_val=  (void *)(new std::function<_R (int,int)>(std::bind((_R (_T::*)(int,int))(fun_obj), (_T *)obj, _1,_2)));



	//c_constructor_class_function_proxy<CProva>("CProba",&CProva::Prova);

	//void * p = malloc(sizeof(CProva)); // reserve memory...

	//register_C_FunctionMemberInt_Prova<CProva>("constructor1",&CProva::CProva);

	//Binding values to the constructor
	//DelayedConstructor<A> dc(21, "Good stuff!");

	//Actually creating an object
	//A *pA = dc();

	//const std::function<CProva()> func = [](void) { return CProva(); };
	//CProva *cp = new(p)func();
	//std::bind(&CProva::CProva, _1);
	//std::function<void()> f = std::function<void()>(std::bind(CProva));
	//CProva * cp = new(p) ((void (*)())CProva)();  // call constructor as function ...


	//void *return_val=NULL;
	//return_val= (void *)(new std::function<void ()>(std::bind((void (*)())(&CProva))));
	//void *fun_ptr =(void *)( new std::function<void (void *)>((void (*)(void *))print));


	//delete cp;
	//std::function<void(int)> x = std::function<CProva * (string *)>{ return new CProva;};

	//std::function<void *(int,int)> * c_constructor = new std::function<void *(int,int)>([](){return new _T;});

	//new CProva(&s1,&s2);


	//str_hola_merda = "caca";
	//function<void,int>(print);


	//-2e-10;
	//int result;
	/*void *fun_ptr = CScriptClassFactory::new_proxy_function(1,print,true);


	//fun_ptr=(void *)( new std::function<int (int)>((int (*)(int))print));
	//void *fun_ptr =(void *)(new std::function<void(string *)>(print));

	// this doesn' work
	//void *fun_ptr =(void *)( new std::function<void (void *)>((void (*)(void *))print));
	string ss="prova";

	printf("prova1 %i %i",sizeof(string *), sizeof(int));

	//auto vv = new std::function<void (string *)>(print);

	//fun_ptr=(void *)vv;


	//vv(&ss);
	(*((std::function<void (int)> *)fun_ptr))((int)&ss);

	return 0;*/



	CLog::setUseAnsiEscape(true);
	CZG_ScriptCore *zg_script = CZG_ScriptCore::getInstance();

	print_info_cr("sizeof(int *):%i sizeof(string *):%i sizeof(int):%i sizeof(tInfoStruct)",sizeof(int *),sizeof(string *),sizeof(int),sizeof(tInfoAsmOp));


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

	if(zg_script->eval(data_buffer)){
		zg_script->execute();
	}

	if(buffer != NULL){
		delete buffer;
	}


	CZG_ScriptCore::destroy();



#if defined(__DEBUG__) && defined(__MEMMANAGER__)
  MEM_ViewStatus();
#endif

	return 0;
}
