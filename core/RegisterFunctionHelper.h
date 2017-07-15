
namespace zetscript{

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




	// template for last parameter argIdx == 1
	template <size_t argIdx, typename F, typename... Args>
	auto getArgTypes( std::vector<std::string> & params)
		-> typename std::enable_if<argIdx == 1>::type
	{
		string parameter_type=typeid(typename F::template argument<argIdx-1>::type).name();
		params.insert(params.begin()+0,parameter_type);
	}

	// template when parameters argIdx > 1
	template <size_t argIdx, typename F, typename... Args>
	auto getArgTypes(std::vector<std::string> & params)
		-> typename std::enable_if<(argIdx > 1)>::type
	{

		string parameter_type=typeid(typename F::template argument<argIdx-1>::type).name();
		params.insert(params.begin()+0,parameter_type);
		getArgTypes<argIdx - 1,F, Args...>( params);
	}


	// trivial case when parameters (argIdx == 0).
	template <size_t argIdx, typename F>
	auto getArgTypes(std::vector<std::string> & params)

	{
		 // NO ARGS CASE
	}

	template <typename F, std::size_t... Is>
	void getParamsFunction(int i,string & returnType, std::vector<std::string> & typeParams, index_sequence<Is...>)
	{
		returnType = typeid(typename F::return_type).name();
		getArgTypes<F::arity, F, typename F::template argument<Is>::type...>(typeParams);
	}

}
