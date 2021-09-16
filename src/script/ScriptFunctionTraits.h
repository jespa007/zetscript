/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */

#pragma once

namespace zetscript{


	template <typename T>
	struct FunctionTraits
		: public FunctionTraits<decltype(&T::operator())>
	{};


	//------------------------------------------------------------
	// Function traits for typedef function

	// function without args ...
	template<class R>
	struct FunctionTraits<R()>
	{
		using return_type = R;
		static constexpr std::size_t arity = 0;
		struct Argument
		{

		};

	};


	// with args  ...
	template<class R, class... Args>
	struct FunctionTraits<R(Args...)>
	{
		using return_type = R;

		static constexpr std::size_t arity = sizeof...(Args);

		template <std::size_t N>
		struct Argument
		{
			static_assert(N < arity, "error:  Member function OBJECT invalid parameter index.");
			using type = typename std::tuple_element<N,std::tuple<Args...>>::type;
		};
	};


	//------------------------------------------------------------
	// Function traits typedef C pointer function
	template<class R, class... Args>
	struct FunctionTraits<R(*)(Args...)> : public FunctionTraits<R(Args...)>
	{
		using return_type = R;

		static constexpr std::size_t arity = sizeof...(Args);

		template <std::size_t N>
		struct Argument
		{
			static_assert(N < arity, "error: C function invalid parameter index.");
			using type = typename std::tuple_element<N,std::tuple<Args...>>::type;
		};
	};

	//------------------------------------------------------------
	// Function traits member object pointer
	template<class C, class R>
	struct FunctionTraits<R(C::*)> : public FunctionTraits<R(C&)>
	{};

	//------------------------------------------------------------
	// Function traits typedef function member pointer function

	template<class C, class R, class... Args>
	struct FunctionTraits<R(C::*)(Args...)> : public FunctionTraits<R(C&,Args...)>
	{
		using return_type = R;

		static constexpr std::size_t arity = sizeof...(Args);

		template <std::size_t N>
		struct Argument
		{
			static_assert(N < arity, "error: C function invalid parameter index.");
			using type = typename std::tuple_element<N,std::tuple<Args...>>::type;
		};
	};

	// const member function pointer (lambda)
	template<class C, class R, class... Args>
	struct FunctionTraits<R(C::*)(Args...) const> : public FunctionTraits<R(C&,Args...)>
	{
		using return_type = R;

		static constexpr std::size_t arity = sizeof...(Args);

		template <std::size_t N>
		struct Argument
		{
			static_assert(N < arity, "error: Member function invalid parameter index.");
			using type = typename std::tuple_element<N,std::tuple<Args...>>::type;
		};
	};



	// unpack function helper
	template <std::size_t...> struct IndexSequence {};
	template <std::size_t N, std::size_t... Is> struct MakeIndexSequence : MakeIndexSequence<N-1, N-1, Is...> {};
	template <std::size_t... Is> struct MakeIndexSequence<0, Is...> : IndexSequence<Is...> {};


	// Dynaminc unpack parameter function ...
	// template for last parameter ArgIdx == 1
	template <size_t ArgIdx, typename F, typename... Args>
	auto getArgTypes( std::vector<zs_string> & params)
		-> typename std::enable_if<ArgIdx == 1>::type
	{
		zs_string parameter_type=typeid(typename F::template Argument<ArgIdx-1>::type).name();
		params.insert(params.begin()+0,parameter_type);
	}

	// template when parameters ArgIdx > 1
	template <size_t ArgIdx, typename F, typename... Args>
	auto getArgTypes(std::vector<zs_string> & params)
		-> typename std::enable_if<(ArgIdx > 1)>::type
	{

		zs_string parameter_type=typeid(typename F::template Argument<ArgIdx-1>::type).name();
		params.insert(params.begin()+0,parameter_type);
		getArgTypes<ArgIdx - 1,F, Args...>( params);
	}


	// trivial case when parameters (ArgIdx == 0).
	template <size_t ArgIdx, typename F>
	auto getArgTypes(std::vector<zs_string> & params)
	-> typename std::enable_if<(ArgIdx == 0)>::type
	{
		 // NO ARGS CASE
	}

	template <typename _F, std::size_t... Is>
	auto getParamsFunction(int i,zs_string & returnType, std::vector<zs_string> & type_params, IndexSequence<Is...>)
	-> typename std::enable_if<(_F::arity > 0)>::type
	{
		returnType = typeid(typename _F::return_type).name();
		getArgTypes<_F::arity, _F, typename _F::template Argument<Is>::type...>(type_params);
	}

	template <typename _F, std::size_t... Is>
	auto getParamsFunction(int i,zs_string & returnType, std::vector<zs_string> & type_params, IndexSequence<Is...>)
	-> typename std::enable_if<(_F::arity == 0)>::type
	{
		returnType = typeid(typename _F::return_type).name();
		getArgTypes<0, _F>(type_params);
	}


	//--------------- OFFSET OF


	template <typename _C,typename T1, typename T2>
	inline size_t offsetOf(T1 T2::*member) {
	  static _C obj;
	  return size_t(&(obj.*member)) - size_t(&obj);
	}

}
