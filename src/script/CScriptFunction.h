#pragma once




#include "CScope.h"



/*

template <typename T>
struct function_traits
    : public function_traits<decltype(&T::operator())>
{};
// For generic types, directly use the result of the signature of its 'operator()'

template <typename ClassType, typename ReturnType, typename... Args>
struct function_traits<ReturnType(ClassType::*)(Args...) const>
// we specialize for pointers to member function
{
    enum { arity = sizeof...(Args) };
    // arity is the number of arguments.

    typedef ReturnType result_type;

    template <size_t i>
    struct arg
    {
        typedef typename std::tuple_element<i, std::tuple<Args...>>::type type;
        // the i-th argument is equivalent to the i-th tuple element of a tuple
        // composed of those arguments.
    };
};

template <std::size_t...> struct index_sequence {};
template <std::size_t N, std::size_t... Is> struct make_index_sequence : make_index_sequence<N-1, N-1, Is...> {};
template <std::size_t... Is> struct make_index_sequence<0, Is...> : index_sequence<Is...> {};
class CObject;
template <size_t argIdx, typename R, typename... Args>
auto getArgTypes(std::string& ref, std::vector<std::string> & params)
    -> typename std::enable_if<argIdx == 1>::type
{
    using fun = function_traits<std::function<R(Args...)> >;
    ref.append(std::to_string(argIdx)+" ");

    typename fun::template arg<argIdx-1>::type var=NULL;
    string parameter_type=typeid(var).name();



    //cout << "is same:" << std::is_base_of<CObject ,typename fun::template arg<0>::type> << endl;
    ref.append(parameter_type).append(" ");
    params.insert(params.begin()+0,parameter_type);

    // number parameter ...
    if(!CFactoryContainer::getInstance()->classPtrIsRegistered(parameter_type)){
    	print_error_cr("\"%s\" is not registered",parameter_type.c_str());
    }


}

template <size_t argIdx, typename R, typename... Args>
auto getArgTypes(std::string& ref, std::vector<std::string> & params)
    -> typename std::enable_if<argIdx != 1>::type
{
    using fun = function_traits<std::function<R(Args...)> >;

    ref.append(std::to_string(argIdx)+" ");

   typename fun::template arg<argIdx-1>::type var=NULL;

    //cout << "is same:" << std::is_same<CObject *,parameter_type>::value << endl;
    string parameter_type=typeid(var).name();
    ref.append(parameter_type).append(" ");
    params.insert(params.begin()+0,parameter_type);

    if(!CFactoryContainer::getInstance()->classPtrIsRegistered(parameter_type)){
    	print_error_cr("\"%s\" is not registered",parameter_type.c_str());
    }



    getArgTypes<argIdx - 1,R, Args...>(ref, params);
}


template <typename F, std::size_t... Is>
std::vector<std::string> * getParamsFunction(int i, index_sequence<Is...>)
{
	std::vector<std::string> *typeParams= new std::vector<std::string>();
	std::string return_type = typeid(typename F::result_type).name();
    std::string s;
    getArgTypes<F::arity, typename F::result_type, typename F::template arg<Is>::type...>(s,*typeParams);

    std::cout << return_type << " (" << (s) << ")";

    if(!CFactoryContainer::getInstance()->classPtrIsRegistered(return_type)){
    	print_error_cr("Return type \"%s\" is not registered",return_type.c_str());
    }


    return typeParams;
}

template<class FunctorT>
void* getcodeptr(const FunctorT& f) {
  auto ptr = &FunctorT::operator();
  return *(void**)&ptr;
}
*/

/*
#define registerOperator(op_name,f){\
	auto reg_function=f;\
	typedef function_traits<decltype(reg_function)> decl;\
	std::vector<string> *param_type=getParamsFunction<decl>(0,make_index_sequence<decl::arity>{});\
	std::string result_type=typeid(decl::result_type).name();\
	CZG_Script::getInstance()->registerOperatorInternal(op_name,result_type,param_type, (void(*)()) getcodeptr(reg_function));\
}
*/

template <typename T>
struct function_traits
    : public function_traits<decltype(&T::operator())>
{};

// function pointer
template<class R, class... Args>
struct function_traits<R(*)(Args...)> : public function_traits<R(Args...)>
{
    using return_type = R;

    static constexpr std::size_t arity = sizeof...(Args);

    template <std::size_t N>
    struct argument
    {
        static_assert(N < arity, "error: invalid parameter index.");
        using type = typename std::tuple_element<N,std::tuple<Args...>>::type;
    };
};

// member function pointer
template<class C, class R, class... Args>
struct function_traits<R(C::*)(Args...)> : public function_traits<R(C&,Args...)>
{
    using return_type = R;

    static constexpr std::size_t arity = sizeof...(Args);

    template <std::size_t N>
    struct argument
    {
        static_assert(N < arity, "error: invalid parameter index.");
        using type = typename std::tuple_element<N,std::tuple<Args...>>::type;
    };
};

// const member function pointer (lambda)
template<class C, class R, class... Args>
struct function_traits<R(C::*)(Args...) const> : public function_traits<R(C&,Args...)>
{
    using return_type = R;

    static constexpr std::size_t arity = sizeof...(Args);

    template <std::size_t N>
    struct argument
    {
        static_assert(N < arity, "error: invalid parameter index.");
        using type = typename std::tuple_element<N,std::tuple<Args...>>::type;
    };
};

// member object pointer
template<class C, class R>
struct function_traits<R(C::*)> : public function_traits<R(C&)>
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
        using type = typename std::tuple_element<N,std::tuple<Args...>>::type;
    };
};



template <std::size_t...> struct index_sequence {};
template <std::size_t N, std::size_t... Is> struct make_index_sequence : make_index_sequence<N-1, N-1, Is...> {};
template <std::size_t... Is> struct make_index_sequence<0, Is...> : index_sequence<Is...> {};

// template for last parameter (argIdx == 1)
template <size_t argIdx, typename F, typename... Args>
auto getArgTypes(std::string& ref, std::vector<std::string> & params)
    -> typename std::enable_if<argIdx == 1>::type
{
 // trivial case...
    //using fun = function_traits<std::function<R(C::*)(Args...)> >;

    //typename F::template argument<0>::type var;


    ref.append(std::to_string(argIdx)+" ");
    //ref.append(typeid(typename fun::template arg<argIdx-1>::type).name()).append(" ");
 //    typename fun::argument<argIdx-1>::type var;
    //typename fun::template argument<0>::type var=NULL;
    string parameter_type=typeid(typename F::template argument<argIdx-1>::type).name();

    printf("\nArg:%i",argIdx);


    //cout << "is same:" << std::is_base_of<CObject ,typename fun::template arg<0>::type> << endl;
    ref.append(parameter_type).append(" ");
    params.insert(params.begin()+0,parameter_type);

    // number parameter ...
   /* if(!CFactoryContainer::getInstance()->classPtrIsRegistered(parameter_type)){
    	print_error_cr("\"%s\" is not registered",parameter_type.c_str());
    }*/


}

// template when parameters are > 1
template <size_t argIdx, typename F, typename... Args>
auto getArgTypes(std::string& ref, std::vector<std::string> & params)
    -> typename std::enable_if<(argIdx > 1)>::type
{
    //using fun = function_traits<std::function<R(Args...)> >;
	//F::argument<0>::type var=NULL;



    ref.append(std::to_string(argIdx)+" ");
   // std::cout << "arg0:" << typeid(fun::argument<0>::type).name() << std::endl;
  // typename fun2::template arg<argIdx-1>::type var=NULL;

   printf("\nArg:%i ",argIdx);//,typeid(Args<argIdx-1>).name());
    //cout << "is same:" << std::is_same<CObject *,parameter_type>::value << endl;
    string parameter_type=typeid(typename F::template argument<argIdx-1>::type).name();
    ref.append(parameter_type).append(" ");
    params.insert(params.begin()+0,parameter_type);

  /*  if(!CFactoryContainer::getInstance()->classPtrIsRegistered(parameter_type)){
    	print_error_cr("\"%s\" is not registered",parameter_type.c_str());
    }*/



    getArgTypes<argIdx - 1,F, Args...>(ref, params);
}

// trivial case when parameters are 0.
template <size_t argIdx, typename F>
auto getArgTypes(std::string& ref, std::vector<std::string> & params)

{
	 printf("\nNo Args!");
}

template <typename F, std::size_t... Is>
void getParamsFunction(int i,string & returnType, std::vector<std::string> & typeParams, index_sequence<Is...>)
{
	//std::vector<std::string> *typeParams= new std::vector<std::string>();
	/*std::string*/ returnType = typeid(typename F::return_type).name();


    std::string s;

    getArgTypes<F::arity, F, typename F::template argument<Is>::type...>(s,typeParams);


    std::cout << returnType << " (" << (s) << ")";

    /*if(!CFactoryContainer::getInstance()->classPtrIsRegistered(return_type)){
    	print_error_cr("Return type \"%s\" is not registered",return_type.c_str());
    }
*/

    //return typeParams;
}





class CScriptFunction: public CFunctor{

public:

	enum TYPE{
		SCRIPT_FUNCTION_TYPE=0, // function with args (default)...
		C_FUNCTION_TYPE, // C Function with defined symbols ...
		CLASS_TYPE // complex script function. Includes args available for constructor and a set of functions
	};

	TYPE m_type;


	typedef struct{
		int m_line;
		CScriptFunction *m_scriptFunction;
	}tInfoRegisteredScriptFunction,tInfoRegisteredClass;


	tInfoRegisteredClass *getRegisteredClass(const string & v, bool print_msg=true);
	CScriptFunction * registerClass(const string & class_name, int line);


	CScriptFunction(CScriptFunction *_parentFunction=NULL);
	vector<CCompiler::tInfoStatementOp> * getCompiledCode();

	CScope::tInfoRegisteredVar * registerArgument(const string & var_name);
	void 	 addFunction(CScriptFunction *sf);
	vector<CScriptFunction *> *	 getVectorFunction();
	vector<string> *	 getArgVector();

	CObject **getArg(const string & var_name);
	CObject **getArg(unsigned index);

	CObject 	*getReturnObject();
	CObject ** 	 getReturnObjectPtr();

	void setReturnObject(CObject *);
	void setupAsFunctionPointer(void * _pointer_function);
	void add_C_function_argument(string arg_type);

	bool call_C_function(vector<CObject *> * argv);

	TYPE getType();


	CScriptFunction *getParent();
	CScope *getScope();

	PASTNode getRootAst();
	PASTNode * getRootAstPtr();


	/*
	 * eval: Evaluates its body.
	 */
//	bool eval(const string & s);
	//bool execute(vector<CObject *> *argv=NULL);

	~CScriptFunction();

private:



	/**
	 * AST root
	 */
	PASTNode m_rootAst;

	/**
	 * This variable tells whether is pointer function or not.
	 */
	void * pointer_function;

	vector<string> m_arg;
	vector<string> m_c_arg; // for c function
	vector<CScriptFunction *> m_function;


	vector<CCompiler::tInfoStatementOp>  	m_listStatements;
	map<string,tInfoRegisteredClass *>  	 m_registeredClass;
	/**
	 * Return variable is assigned at the begin as undefined and when return keyword occurs,
	 * returnVariable is assigned as the result of expression after return. By default return variable is type void.
	 */
	CObject *returnVariable;
	tInfoRegisteredClass * existRegisteredClass(const string & class_name);


	// calling function...
	CObject * call_print_1p( CObject *arg);
	CObject * call_print_0p();

	CScope	*m_scope; // base scope...
	CScriptFunction *m_parentFunction;

};
