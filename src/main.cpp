//#include "script/zg_script.h"

#include <type_traits>
#include <iostream>
#include <functional>
#include <stdexcept>
#include <typeinfo>
#include <string>
#include <vector>
#include <map>

using namespace std;


class CCustomObject{
	int x;
public:
	CCustomObject(){
		printf("CCustomObject constructing point @%p\n", this);
	}
	void member(){

		printf("hola %p!\n",this);
	}

	void member2(int i1){}

	   virtual ~CCustomObject()
	   {
	       printf("CCustomObject @%p destroyed\n", this);
	   }
};


// as seen on http://functionalcpp.wordpress.com/2013/08/05/function-traits/
template<class F>
struct function_traits;

// function pointer
template<class R, class... Args>
struct function_traits<R(*)(Args...)> : public function_traits<R(Args...)>
{};

template<class R, class... Args>
struct function_traits<R(Args...)>
{
    using return_type = R;

    static constexpr std::size_t arity = sizeof...(Args);

    template <std::size_t N>
    struct argument
    {
        static_assert(N < arity, "error: invalid parameter index.");
        //using type = typename std::tuple_element<N,std::tuple<Args...>>::type;
        typedef typename std::tuple_element<N, std::tuple<Args...>>::type type;
    };
};

// member function pointer
template<class C, class R, class... Args>
struct function_traits<R(C::*)(Args...)> : public function_traits<R(C&,Args...)>
{};

// const member function pointer
template<class C, class R, class... Args>
struct function_traits<R(C::*)(Args...) const> : public function_traits<R(C&,Args...)>
{};

// member object pointer
template<class C, class R>
struct function_traits<R(C::*)> : public function_traits<R(C&)>
{};

template <std::size_t...> struct index_sequence {};
template <std::size_t N, std::size_t... Is> struct make_index_sequence : make_index_sequence<N-1, N-1, Is...> {};
template <std::size_t... Is> struct make_index_sequence<0, Is...> : index_sequence<Is...> {};
/*
template <class X, typename ARG>
std::function<void(X*, ARG)> wrapper(void (X::*mfp)(ARG)) {
    return [=](X *x, ARG arg) { (x->*mfp)(arg); };
}

// member function pointer
template<class C, class R, class... Args>
struct function_traits<R(C::*)(Args...)> : public function_traits<R(C&,Args...)>
{};

// const member function pointer
template<class C, class R, class... Args>
struct function_traits<R(C::*)(Args...) const> : public function_traits<R(C&,Args...)>
{};

// member object pointer
template<class C, class R>
struct function_traits<R(C::*)> : public function_traits<R(C&)>
{};*/
/*
// functor
template<class F>
struct function_traits
{
    private:
        using call_type = function_traits<decltype(&F::type::operator())>;
    public:
        using return_type = typename call_type::return_type;

        static constexpr std::size_t arity = call_type::arity - 1;

        template <std::size_t N>
        struct argument
        {
            static_assert(N < arity, "error: invalid parameter index.");
            using type = typename call_type::template argument<N+1>::type;
        };
};

template<class F>
struct traits<F&> : public traits<F>
{};

template<class F>
struct traits<F&&> : public traits<F>
{};
*/

/*
#define registerFunctionMember3P(f)\
{\
  	using Traits = function_traits<decltype(f)>;\
  	std::cout << "number_args:" << Traits::arity << std::endl;\
  	std::cout << "return_type:" << typeid(Traits::return_type).name() << std::endl;\
  	std::cout << "arg0:" << typeid(Traits::argument<0>::type).name() << std::endl;\
  	std::cout << "arg1:" << typeid(Traits::argument<1>::type).name() << std::endl;\
  	std::cout << "arg2:" << typeid(Traits::argument<2>::type).name() << std::endl;\
}

#define registerFunctionMember2P(f)\
{\
  	using Traits = function_traits<decltype(f)>;\
  	std::cout << "number_args:" << Traits::arity << std::endl;\
  	std::cout << "return_type:" << typeid(Traits::return_type).name() << std::endl;\
  	std::cout << "arg0:" << typeid(Traits::argument<0>::type).name() << std::endl;\
  	std::cout << "arg1:" << typeid(Traits::argument<1>::type).name() << std::endl;\
}

#define registerFunctionMember1P(f)\
{\
  	using Traits = function_traits<decltype(f)>;\
  	std::cout << "number_args:" << Traits::arity << std::endl;\
  	std::cout << "return_type:" << typeid(Traits::return_type).name() << std::endl;\
  	std::cout << "arg0:" << typeid(Traits::argument<0>::type).name() << std::endl;\
}


#define registerFunctionMember(f)\
{\
  	using Traits = function_traits<decltype(f)>;\
  	switch(Traits::arity)\
	{\
  	case 1:\
	registerFunctionMember1P(f);\
	break;\
  	case 2:\
	registerFunctionMember2P(f);\
	break;\
  	case 3:{\
	registerFunctionMember3P(f);}\
	break;\
	}\
}*/
/*
template<typename T>
void registerFunctionMember(){

}*/



template <size_t argIdx, typename R, typename... Args>
auto getArgTypes(std::string& ref, std::vector<std::string> & params)
    -> typename std::enable_if<argIdx == 1>::type
{

    using fun = function_traits<std::function<R(Args...)> >;
    ref.append(std::to_string(argIdx)+" ");
    //ref.append(typeid(typename fun::template arg<argIdx-1>::type).name()).append(" ");
   //  typename fun::template argument<argIdx-1>::type var;
    //typename fun::template argument<argIdx-1>::type var=NULL;
    string parameter_type="";//fun::arity;//typeid(Traits::argument<argIdx-1>::type).name();

    printf("\nArg:%i",argIdx);


    //cout << "is same:" << std::is_base_of<CObject ,typename fun::template arg<0>::type> << endl;
    ref.append(parameter_type).append(" ");
    params.insert(params.begin()+0,parameter_type);

    // number parameter ...
   /* if(!CFactoryContainer::getInstance()->classPtrIsRegistered(parameter_type)){
    	print_error_cr("\"%s\" is not registered",parameter_type.c_str());
    }*/


}

template <size_t argIdx, typename R, typename... Args>
auto getArgTypes(std::string& ref, std::vector<std::string> & params)
    -> typename std::enable_if<argIdx != 1>::type
{
    using fun = function_traits<std::function<R(Args...)> >;



    ref.append(std::to_string(argIdx)+" ");
    //std::cout << "arg0:" << typeid(fun::argument<0>::type).name() << std::endl;
  // typename fun2::template arg<argIdx-1>::type var=NULL;

   printf("\nArg:%i ",argIdx);//,typeid(Args<argIdx-1>).name());
    //cout << "is same:" << std::is_same<CObject *,parameter_type>::value << endl;
    string parameter_type="";//typeid(var).name();
    ref.append(parameter_type).append(" ");
    params.insert(params.begin()+0,parameter_type);

  /*  if(!CFactoryContainer::getInstance()->classPtrIsRegistered(parameter_type)){
    	print_error_cr("\"%s\" is not registered",parameter_type.c_str());
    }*/



    getArgTypes<argIdx - 1,R, Args...>(ref, params);
}

template <typename F, std::size_t... Is>
std::vector<std::string> * getParamsFunction(int i, index_sequence<Is...>)
{
	std::vector<std::string> *typeParams= new std::vector<std::string>();
	std::string return_type = typeid(typename F::return_type).name();
    std::string s;
    getArgTypes<F::arity, typename F::return_type, typename F::template argument<Is>::type...>(s,*typeParams);

    std::cout << return_type << " (" << (s) << ")";

    /*if(!CFactoryContainer::getInstance()->classPtrIsRegistered(return_type)){
    	print_error_cr("Return type \"%s\" is not registered",return_type.c_str());
    }
*/

    return typeParams;
}

float free_function(const std::string& a, int b)
{
    return (float)a.size() / b;
}


int main(int argc, char * argv[]){

	std::cout<< "Type:" <<typeid(CCustomObject).name() << std::endl;
	CCustomObject * obj = new CCustomObject();
	obj->member();

	//using Traits = function_traits<decltype(CCustomObject::member2)>;
	using Traits = function_traits<decltype(CCustomObject::member2)>;
	//using Traits = function_traits<decltype(free_function)>;
	//auto reg_function=[](int i){return 0;};
	//typedef function_traits<decltype(reg_function)> Traits;

	//typedef function_traits<decltype(reg_function)> decl;
	std::vector<string> *param_type=getParamsFunction<Traits>(0,make_index_sequence<Traits::arity>{});
	//registerFunctionMember<>(CCustomObject::member2);
	//auto reg_function=std::bind(&CCustomObject::member2,obj,std::placeholders::_1,std::placeholders::_2);
	//std::function<void (CCustomObject::*(int))> fun = &obj->member2;
  	/*using Traits = function_traits<decltype(CCustomObject::member)>;
  	//cout << std::endl;
  	std::cout << "number_args:" << Traits::arity << std::endl;
  	std::cout << "return_type:" << typeid(Traits::return_type).name() << std::endl;
  	std::cout << "arg0:" << typeid(Traits::argument<0>::type).name() << std::endl;*/

   /* static_assert(Traits::arity == 2,"");
    static_assert(std::is_same<Traits::return_type,float>::value,"");
    static_assert(std::is_same<Traits::argument<0>::type,const std::string&>::value,"");
    static_assert(std::is_same<Traits::argument<1>::type,int>::value,"");
*/
//	function_traits<type_fun>::result_type
	/*  callbacks cb;

	  cb.add("lol", [](int a, float b) {
	    std::cout << "a: " << a << std::endl;
	    std::cout << "b: " << b << std::endl;
	  });

	  cb.call("lol", 23, 5.4f);
	  cb.call("lol", 23, 43);
*/

	// auto g = wrapper(&CCustomObject::member2);

	//function(obj,1);

	int result;
	printf("obj:%p\n",obj);
	//typedef std::function<void(int, int)> tFunction = CCustomObject::member2;

	//auto reg_function=std::bind(&CCustomObject::member2,obj,std::placeholders::_1,std::placeholders::_2);


	//std::cout << typeid(decl::arg<0>::type).name() << std::endl;
	//std::cout << count_arg<tFunction>::value << std::endl;

	//std::cout << typeid(function_traits2<decl>::arg<0>::type).name() << std::endl;
	/*std::vector<string> *param_type=getParamsFunction<decl>(0,make_index_sequence<decl::arity>{});
	std::string result_type=typeid(decl::result_type).name();*/

	//ptr_to_member c_fun = (ptr_to_member) &CCustomObject::member;
	//(*c_fun)((void *)obj);

	//void** ppVoid = (void**) &CCustomObject::member; // This is black magic a.k.a. undefined behavior.
	//void* pActualPtr = *ppVoid;

	void * c_fun = (void *) &CCustomObject::member;
#ifdef _WIN32
	asm(
			 "movl %[obj],%%ecx # this pointer\n\t"
			//"push %%esp\n\t"
			"call *%[fun]\n\t" // call function
			//"add $4,%%esp"       // Clean up the stack.
			: "=a" (result) // The result code from puts.
			:[fun] "r"(c_fun) ,[obj] "r"(obj)
			 :"%ecx"
			  );
	#endif // GNU!!!!
//	ptr_to_member c_fun = (ptr_to_member) &obj->member;


	//

/*
	printf("%s\n",(char *) typeid(CString *).name());
	printf("%s\n",(char *) typeid(string *).name());

	int i=0;
	CLog::setUseAnsiEscape(true);

	printf("\nvar %i\n\n",i);

	print_info_cr("sizeof(CObject)=%i sizeof(float)=%i sizeof(string)=%i",sizeof(CObject),sizeof(float),sizeof(string));



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
	ByteBuffer *buffer = CIO_Utils::readFile(argv[1]);

	if(buffer != NULL){

		CZG_Script *zg_script = CZG_Script::getInstance();
		zg_script->init();

		if(zg_script->eval((char *)buffer->data_buffer));

		print_info_cr("sizeobject:%i",sizeof(CObject));
		print_info_cr("sizenumber:%i",sizeof(CNumber));

		print_info_cr("float:%s",typeid(float).name());
		print_info_cr("string:%s",typeid(string).name());
		print_info_cr("bool:%s",typeid(bool).name());


		CZG_Script::destroy();
		delete buffer;
	}

#if defined(__DEBUG__) && defined(__MEMMANAGER__)
  MEM_ViewStatus();
#endif
*/
	return 0;
}
