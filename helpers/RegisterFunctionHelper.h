/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */

namespace zetscript{

	template <typename T>
	struct function_traits
		: public function_traits<decltype(&T::operator())>
	{};


	//------------------------------------------------------------
	// Function traits for typedef function

	// function without args ...
	template<class R>
	struct function_traits<R()>
	{
		using return_type = R;
		static constexpr std::size_t arity = 0;
		struct argument
		{

		};

	};


	// with args  ...
	template<class R, class... Args>
	struct function_traits<R(Args...)>
	{
		using return_type = R;

		static constexpr std::size_t arity = sizeof...(Args);

		template <std::size_t N>
		struct argument
		{
			static_assert(N < arity, "error:  Member function OBJECT invalid parameter index.");
			using type = typename std::tuple_element<N,std::tuple<Args...>>::type;
		};
	};


	//------------------------------------------------------------
	// Function traits typedef C pointer function
	template<class R, class... Args>
	struct function_traits<R(*)(Args...)> : public function_traits<R(Args...)>
	{
		using return_type = R;

		static constexpr std::size_t arity = sizeof...(Args);

		template <std::size_t N>
		struct argument
		{
			static_assert(N < arity, "error: C function invalid parameter index.");
			using type = typename std::tuple_element<N,std::tuple<Args...>>::type;
		};
	};

	//------------------------------------------------------------
	// Function traits member object pointer
	template<class C, class R>
	struct function_traits<R(C::*)> : public function_traits<R(C&)>
	{};

	//------------------------------------------------------------
	// Function traits typedef function member pointer function

	template<class C, class R, class... Args>
	struct function_traits<R(C::*)(Args...)> : public function_traits<R(C&,Args...)>
	{
		using return_type = R;

		static constexpr std::size_t arity = sizeof...(Args);

		template <std::size_t N>
		struct argument
		{
			static_assert(N < arity, "error: C function invalid parameter index.");
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
			static_assert(N < arity, "error: Member function invalid parameter index.");
			using type = typename std::tuple_element<N,std::tuple<Args...>>::type;
		};
	};



	// unpack function helper
	template <std::size_t...> struct index_sequence {};
	template <std::size_t N, std::size_t... Is> struct make_index_sequence : make_index_sequence<N-1, N-1, Is...> {};
	template <std::size_t... Is> struct make_index_sequence<0, Is...> : index_sequence<Is...> {};


	// Dynaminc unpack parameter function ...
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
	-> typename std::enable_if<(argIdx == 0)>::type
	{
		 // NO ARGS CASE
	}

	template <typename _F, std::size_t... Is>
	auto getParamsFunction(int i,string & returnType, std::vector<std::string> & typeParams, index_sequence<Is...>)
	-> typename std::enable_if<(_F::arity > 0)>::type
	{
		returnType = typeid(typename _F::return_type).name();
		getArgTypes<_F::arity, _F, typename _F::template argument<Is>::type...>(typeParams);
	}

	template <typename _F, std::size_t... Is>
	auto getParamsFunction(int i,string & returnType, std::vector<std::string> & typeParams, index_sequence<Is...>)
	-> typename std::enable_if<(_F::arity == 0)>::type
	{
		returnType = typeid(typename _F::return_type).name();
		getArgTypes<0, _F>(typeParams);
	}


	//--------------- OFFSET OF


	template <typename T1, typename T2>
	inline size_t offset_of(T1 T2::*member) {
	  static T2 obj;
	  return size_t(&(obj.*member)) - size_t(&obj);
	}

}
