


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


    //cout << "is same:" << std::is_base_of<CVariable ,typename fun::template arg<0>::type> << endl;
    ref.append(parameter_type).append(" ");
    params.insert(params.begin()+0,parameter_type);


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
    //cout << "is same:" << std::is_same<CVariable *,parameter_type>::value << endl;
    string parameter_type=typeid(typename F::template argument<argIdx-1>::type).name();
    ref.append(parameter_type).append(" ");
    params.insert(params.begin()+0,parameter_type);



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

}
