#pragma once

#define ZS_NEW_VOID_FUNCTION_MEMBER_POINTER_PARAM_0(o,f)						(new zetscript::CFunctionMemberPointerVoidParam0Caller<o>(this,&o::f))
#define ZS_NEW_VOID_FUNCTION_MEMBER_POINTER_PARAM_1(o,f,p1)						(new zetscript::CFunctionMemberPointerVoidParam1Caller<o,p1>(this,&o::f))
#define ZS_NEW_VOID_FUNCTION_MEMBER_POINTER_PARAM_2(o,f,p1,p2)					(new zetscript::CFunctionMemberPointerVoidParam2Caller<o,p1,p2>(this,&o::f))
#define ZS_NEW_VOID_FUNCTION_MEMBER_POINTER_PARAM_3(o,f,p1,p2,p3)				(new zetscript::CFunctionMemberPointerVoidParam3Caller<o,p1,p2,p3>(this,&o::f))
#define ZS_NEW_VOID_FUNCTION_MEMBER_POINTER_PARAM_4(o,f,p1,p2,p3,p4)			(new zetscript::CFunctionMemberPointerVoidParam4Caller<o,p1,p2,p3,p4>(this,&o::f))
#define ZS_NEW_VOID_FUNCTION_MEMBER_POINTER_PARAM_5(o,f,p1,p2,p3,p4,p5)			(new zetscript::CFunctionMemberPointerVoidParam5Caller<o,p1,p2,p3,p4,p5>(this,&o::f))
#define ZS_NEW_VOID_FUNCTION_MEMBER_POINTER_PARAM_6(o,f,p1,p2,p3,p4,p5,p6)		(new zetscript::CFunctionMemberPointerVoidParam6Caller<o,p1,p2,p3,p4,p5,p6>(this,&o::f))

#define ZS_NEW_RET_FUNCTION_MEMBER_POINTER_PARAM_0(o,f)							(new zetscript::CFunctionMemberPointerRetParam0Caller<o>(this,&o::f))
#define ZS_NEW_RET_FUNCTION_MEMBER_POINTER_PARAM_1(o,f,p1)						(new zetscript::CFunctionMemberPointerRetParam1Caller<o,p1>(this,&o::f))
#define ZS_NEW_RET_FUNCTION_MEMBER_POINTER_PARAM_2(o,f,p1,p2)					(new zetscript::CFunctionMemberPointerRetParam2Caller<o,p1,p2>(this,&o::f))
#define ZS_NEW_RET_FUNCTION_MEMBER_POINTER_PARAM_3(o,f,p1,p2,p3)				(new zetscript::CFunctionMemberPointerRetParam3Caller<o,p1,p2,p3>(this,&o::f))
#define ZS_NEW_RET_FUNCTION_MEMBER_POINTER_PARAM_4(o,f,p1,p2,p3,p4)				(new zetscript::CFunctionMemberPointerRetParam4Caller<o,p1,p2,p3,p4>(this,&o::f))
#define ZS_NEW_RET_FUNCTION_MEMBER_POINTER_PARAM_5(o,f,p1,p2,p3,p4,p5)			(new zetscript::CFunctionMemberPointerRetParam5Caller<o,p1,p2,p3,p4,p5>(this,&o::f))
#define ZS_NEW_RET_FUNCTION_MEMBER_POINTER_PARAM_6(o,f,p1,p2,p3,p4,p5,p6)		(new zetscript::CFunctionMemberPointerRetParam6Caller<o,p1,p2,p3,p4,p5,p6>(this,&o::f))

namespace zetscript{



	//-------------------------------------------------------------------------------------------------
	// VOID RET PARAM0
	class CFunctionMemberPointerVoidParam0{
		public:
			virtual void operator()()=0;
			virtual ~CFunctionMemberPointerVoidParam0(){}
	};

	// Simple pointer function
	template <class _T>
	class CFunctionPointerVoidParam0Caller: public CFunctionMemberPointerVoidParam0{

		void (*m_func) ( );
		_T *ptr;
	public:
		CFunctionPointerVoidParam0Caller(_T *obj,void (* _func) ())
		{
			m_func =_func;
			ptr = obj;
		}

		inline virtual void operator()(){
			(*m_func) ();
		}

		virtual ~CFunctionPointerVoidParam0Caller(){}
	};

	// Pointer member function
	template <class _T>
	class CFunctionMemberPointerVoidParam0Caller: public CFunctionMemberPointerVoidParam0{

		void (_T::* m_func) ( );
		_T *ptr;
	public:
		CFunctionMemberPointerVoidParam0Caller(_T *obj,void (_T::* _func) ())
		{
			m_func =_func;
			ptr = obj;
		}

		inline virtual void operator()(){
			((*ptr).*m_func) ();
		}

		virtual ~CFunctionMemberPointerVoidParam0Caller(){}
	};



	//-------------------------------------------------------------------------------------------------
	// VOID RET PARAM1
	template <typename _TPARAM1>
	class CFunctionMemberPointerVoidParam1{
		public:
			virtual void operator()(_TPARAM1 p1)=0;

			virtual ~CFunctionMemberPointerVoidParam1(){}
	};

	template <class _T,typename _TPARAM1>
	class CFunctionMemberPointerVoidParam1Caller: public CFunctionMemberPointerVoidParam1<_TPARAM1>{

		void (_T::* m_func) (_TPARAM1 p1);
		_T *ptr;
	public:
		CFunctionMemberPointerVoidParam1Caller(_T *obj,void (_T::* _func) ( _TPARAM1 p1))
		{
			m_func =_func;
			ptr = obj;
		}

		inline virtual void operator()( _TPARAM1 p1){

			((*ptr).*m_func) (p1);
		}

		virtual ~CFunctionMemberPointerVoidParam1Caller(){}
	};
	//-------------------------------------------------------------------------------------------------
	// VOID RET PARAM2
	template <typename _TPARAM1,typename _TPARAM2>
	class CFunctionMemberPointerVoidParam2{
		public:
			virtual void operator()(_TPARAM1 p1, _TPARAM2 p2)=0;

			virtual ~CFunctionMemberPointerVoidParam2(){}
	};

	template <class _T,typename _TPARAM1,typename _TPARAM2>
	class CFunctionMemberPointerVoidParam2Caller: public CFunctionMemberPointerVoidParam2<_TPARAM1,_TPARAM2>{

		void (_T::* m_func) (_TPARAM1 p1,_TPARAM2 p2);
		_T *ptr;
	public:
		CFunctionMemberPointerVoidParam2Caller(_T *obj,void (_T::* _func) ( _TPARAM1 p1,_TPARAM2 p2))
		{
			m_func =_func;
			ptr = obj;
		}

		inline virtual void operator()( _TPARAM1 p1,_TPARAM2 p2){

			((*ptr).*m_func) (p1,p2);
		}

		virtual ~CFunctionMemberPointerVoidParam2Caller(){}
	};

	//-------------------------------------------------------------------------------------------------
	// VOID RET PARAM3
	template <typename _TPARAM1,typename _TPARAM2,typename _TPARAM3>
	class CFunctionMemberPointerVoidParam3{
		public:
			virtual void operator()(_TPARAM1 p1, _TPARAM2 p2, _TPARAM3 p3)=0;

			virtual ~CFunctionMemberPointerVoidParam3(){}
	};

	template <class _T,typename _TPARAM1,typename _TPARAM2,typename _TPARAM3>
	class CFunctionMemberPointerVoidParam3Caller: public CFunctionMemberPointerVoidParam3<_TPARAM1,_TPARAM2,_TPARAM3>{

		void (_T::* m_func) (_TPARAM1 p1,_TPARAM2 p2,_TPARAM3 p3);
		_T *ptr;
	public:
		CFunctionMemberPointerVoidParam3Caller(_T *obj,void (_T::* _func) ( _TPARAM1 p1,_TPARAM2 p2,_TPARAM3 p3))
		{
			m_func =_func;
			ptr = obj;
		}

		inline virtual void operator()( _TPARAM1 p1,_TPARAM2 p2,_TPARAM3 p3){

			((*ptr).*m_func) (p1,p2,p3);
		}

		virtual ~CFunctionMemberPointerVoidParam3Caller(){}
	};
	//-------------------------------------------------------------------------------------------------
	// VOID RET PARAM4
	template <typename _TPARAM1,typename _TPARAM2,typename _TPARAM3,typename _TPARAM4>
	class CFunctionMemberPointerVoidParam4{
		public:
			virtual void operator()(_TPARAM1 p1, _TPARAM2 p2, _TPARAM3 p3,_TPARAM4 p4)=0;

			virtual ~CFunctionMemberPointerVoidParam4(){}
	};

	template <class _T,typename _TPARAM1,typename _TPARAM2,typename _TPARAM3,typename _TPARAM4>
	class CFunctionMemberPointerVoidParam4Caller: public CFunctionMemberPointerVoidParam4<_TPARAM1,_TPARAM2,_TPARAM3,_TPARAM4>{

		void (_T::* m_func) (_TPARAM1 p1,_TPARAM2 p2,_TPARAM3 p3,_TPARAM4 p4);
		_T *ptr;
	public:
		CFunctionMemberPointerVoidParam4Caller(_T *obj,void (_T::* _func) ( _TPARAM1 p1,_TPARAM2 p2,_TPARAM3 p3,_TPARAM4 p4))
		{
			m_func =_func;
			ptr = obj;
		}

		inline virtual void operator()( _TPARAM1 p1,_TPARAM2 p2,_TPARAM3 p3,_TPARAM4 p4){

			((*ptr).*m_func) (p1,p2,p3,p4);
		}

		virtual ~CFunctionMemberPointerVoidParam4Caller(){}
	};
	//-------------------------------------------------------------------------------------------------
	// VOID RET PARAM5
	template <typename _TPARAM1,typename _TPARAM2,typename _TPARAM3,typename _TPARAM4,typename _TPARAM5>
	class CFunctionMemberPointerVoidParam5{
		public:
			virtual void operator()(_TPARAM1 p1, _TPARAM2 p2, _TPARAM3 p3,_TPARAM4 p4,_TPARAM4 p5)=0;

			virtual ~CFunctionMemberPointerVoidParam5(){}
	};

	template <class _T,typename _TPARAM1,typename _TPARAM2,typename _TPARAM3,typename _TPARAM4,typename _TPARAM5>
	class CFunctionMemberPointerVoidParam5Caller: public CFunctionMemberPointerVoidParam5<_TPARAM1,_TPARAM2,_TPARAM3,_TPARAM4,_TPARAM4>{

		void (_T::* m_func) (_TPARAM1 p1,_TPARAM2 p2,_TPARAM3 p3,_TPARAM4 p4,_TPARAM4 p5);
		_T *ptr;
	public:
		CFunctionMemberPointerVoidParam5Caller(_T *obj,void (_T::* _func) ( _TPARAM1 p1,_TPARAM2 p2,_TPARAM3 p3,_TPARAM4 p4,_TPARAM4 p5))
		{
			m_func =_func;
			ptr = obj;
		}

		inline virtual void operator()( _TPARAM1 p1,_TPARAM2 p2,_TPARAM3 p3,_TPARAM4 p4,_TPARAM4 p5){

			((*ptr).*m_func) (p1,p2,p3,p4,p5);
		}

		virtual ~CFunctionMemberPointerVoidParam5Caller(){}
	};
	//-------------------------------------------------------------------------------------------------
	// VOID RET PARAM6
	template <typename _TPARAM1,typename _TPARAM2,typename _TPARAM3,typename _TPARAM4,typename _TPARAM5,typename _TPARAM6>
	class CFunctionMemberPointerVoidParam6{
		public:
			virtual void operator()(_TPARAM1 p1, _TPARAM2 p2, _TPARAM3 p3,_TPARAM4 p4,_TPARAM4 p5,_TPARAM4 p6)=0;

			virtual ~CFunctionMemberPointerVoidParam6(){}
	};

	template <class _T,typename _TPARAM1,typename _TPARAM2,typename _TPARAM3,typename _TPARAM4,typename _TPARAM5,typename _TPARAM6>
	class CFunctionMemberPointerVoidParam6Caller: public CFunctionMemberPointerVoidParam6<_TPARAM1,_TPARAM2,_TPARAM3,_TPARAM4,_TPARAM4, _TPARAM6>{

		void (_T::* m_func) (_TPARAM1 p1,_TPARAM2 p2,_TPARAM3 p3,_TPARAM4 p4,_TPARAM4 p5,_TPARAM4 p6);
		_T *ptr;
	public:
		CFunctionMemberPointerVoidParam6Caller(_T *obj,void (_T::* _func) ( _TPARAM1 p1,_TPARAM2 p2,_TPARAM3 p3,_TPARAM4 p4,_TPARAM4 p5,_TPARAM4 p6))
		{
			m_func =_func;
			ptr = obj;
		}

		inline virtual void operator()( _TPARAM1 p1,_TPARAM2 p2,_TPARAM3 p3,_TPARAM4 p4,_TPARAM4 p5,_TPARAM4 p6){

			((*ptr).*m_func) (p1,p2,p3,p4,p5,p6);
		}

		virtual ~CFunctionMemberPointerVoidParam6Caller(){}
	};

	//-------------------------------------------------------------------------------------------------
	// RET PARAM0
	template <class _R>
	class CFunctionMemberPointerRetParam0{
		public:
			virtual _R operator()()=0;
			virtual ~CFunctionMemberPointerRetParam0(){}
	};

	template <class _R,class _T>
	class CFunctionMemberPointerRetParam0Caller: public CFunctionMemberPointerRetParam0<_R>{

		void (_T::* m_func) ( );
		_T *ptr;
	public:
		CFunctionMemberPointerRetParam0Caller(_T *obj,void (_T::* _func) ())
		{
			m_func =_func;
			ptr = obj;
		}

		inline virtual _R operator()(){
			return ((*ptr).*m_func) ();
		}

		virtual ~CFunctionMemberPointerRetParam0Caller(){}
	};

	//-------------------------------------------------------------------------------------------------
	// VOID RET PARAM1
	template <typename _R, typename _TPARAM1>
	class CFunctionMemberPointerRetParam1{
		public:
			virtual _R operator()(_TPARAM1 p1)=0;

			virtual ~CFunctionMemberPointerRetParam1(){}
	};

	template <typename _R,class _T,typename _TPARAM1>
	class CFunctionMemberPointerRetParam1Caller: public CFunctionMemberPointerRetParam1<_R,_TPARAM1>{

		_R (_T::* m_func) (_TPARAM1 p1);
		_T *ptr;
	public:
		CFunctionMemberPointerRetParam1Caller(_T *obj,void (_T::* _func) ( _TPARAM1 p1))
		{
			m_func =_func;
			ptr = obj;
		}

		inline virtual _R operator()( _TPARAM1 p1){

			return ((*ptr).*m_func) (p1);
		}

		virtual ~CFunctionMemberPointerRetParam1Caller(){}
	};
	//-------------------------------------------------------------------------------------------------
	// VOID RET PARAM2
	template <typename _R,typename _TPARAM1,typename _TPARAM2>
	class CFunctionMemberPointerRetParam2{
		public:
			virtual _R operator()(_TPARAM1 p1, _TPARAM2 p2)=0;

			virtual ~CFunctionMemberPointerRetParam2(){}
	};

	template <typename _R,class _T,typename _TPARAM1,typename _TPARAM2>
	class CFunctionMemberPointerRetParam2Caller: public CFunctionMemberPointerRetParam2<_R,_TPARAM1,_TPARAM2>{

		_R (_T::* m_func) (_TPARAM1 p1,_TPARAM2 p2);
		_T *ptr;
	public:
		CFunctionMemberPointerRetParam2Caller(_T *obj,_R (_T::* _func) ( _TPARAM1 p1,_TPARAM2 p2))
		{
			m_func =_func;
			ptr = obj;
		}

		inline virtual _R operator()( _TPARAM1 p1,_TPARAM2 p2){

			return ((*ptr).*m_func) (p1,p2);
		}

		virtual ~CFunctionMemberPointerRetParam2Caller(){}
	};

	//-------------------------------------------------------------------------------------------------
	// VOID RET PARAM3
	template <typename _R,typename _TPARAM1,typename _TPARAM2,typename _TPARAM3>
	class CFunctionMemberPointerRetParam3{
		public:
			virtual _R operator()(_TPARAM1 p1, _TPARAM2 p2, _TPARAM3 p3)=0;

			virtual ~CFunctionMemberPointerRetParam3(){}
	};

	template <typename _R,class _T,typename _TPARAM1,typename _TPARAM2,typename _TPARAM3>
	class CFunctionMemberPointerRetParam3Caller: public CFunctionMemberPointerRetParam3<_R,_TPARAM1,_TPARAM2,_TPARAM3>{

		_R (_T::* m_func) (_TPARAM1 p1,_TPARAM2 p2,_TPARAM3 p3);
		_T *ptr;
	public:
		CFunctionMemberPointerRetParam3Caller(_T *obj,_R (_T::* _func) ( _TPARAM1 p1,_TPARAM2 p2,_TPARAM3 p3))
		{
			m_func =_func;
			ptr = obj;
		}

		inline virtual _R operator()( _TPARAM1 p1,_TPARAM2 p2,_TPARAM3 p3){

			return ((*ptr).*m_func) (p1,p2,p3);
		}

		virtual ~CFunctionMemberPointerRetParam3Caller(){}
	};
	//-------------------------------------------------------------------------------------------------
	// VOID RET PARAM4
	template <typename _R,typename _TPARAM1,typename _TPARAM2,typename _TPARAM3,typename _TPARAM4>
	class CFunctionMemberPointerRetParam4{
		public:
			virtual _R operator()(_TPARAM1 p1, _TPARAM2 p2, _TPARAM3 p3,_TPARAM4 p4)=0;

			virtual ~CFunctionMemberPointerRetParam4(){}
	};

	template <typename _R,class _T,typename _TPARAM1,typename _TPARAM2,typename _TPARAM3,typename _TPARAM4>
	class CFunctionMemberPointerRetParam4Caller: public CFunctionMemberPointerRetParam4<_R,_TPARAM1,_TPARAM2,_TPARAM3,_TPARAM4>{

		_R (_T::* m_func) (_TPARAM1 p1,_TPARAM2 p2,_TPARAM3 p3,_TPARAM4 p4);
		_T *ptr;
	public:
		CFunctionMemberPointerRetParam4Caller(_T *obj,_R (_T::* _func) ( _TPARAM1 p1,_TPARAM2 p2,_TPARAM3 p3,_TPARAM4 p4))
		{
			m_func =_func;
			ptr = obj;
		}

		inline virtual _R operator()( _TPARAM1 p1,_TPARAM2 p2,_TPARAM3 p3,_TPARAM4 p4){

			return ((*ptr).*m_func) (p1,p2,p3,p4);
		}

		virtual ~CFunctionMemberPointerRetParam4Caller(){}
	};
	//-------------------------------------------------------------------------------------------------
	// VOID RET PARAM5
	template <typename _R,typename _TPARAM1,typename _TPARAM2,typename _TPARAM3,typename _TPARAM4,typename _TPARAM5>
	class CFunctionMemberPointerRetParam5{
		public:
			virtual _R operator()(_TPARAM1 p1, _TPARAM2 p2, _TPARAM3 p3,_TPARAM4 p4,_TPARAM4 p5)=0;

			virtual ~CFunctionMemberPointerRetParam5(){}
	};

	template <typename _R,class _T,typename _TPARAM1,typename _TPARAM2,typename _TPARAM3,typename _TPARAM4,typename _TPARAM5>
	class CFunctionMemberPointerRetParam5Caller: public CFunctionMemberPointerVoidParam5<_TPARAM1,_TPARAM2,_TPARAM3,_TPARAM4,_TPARAM4>{

		_R (_T::* m_func) (_TPARAM1 p1,_TPARAM2 p2,_TPARAM3 p3,_TPARAM4 p4,_TPARAM4 p5);
		_T *ptr;
	public:
		CFunctionMemberPointerRetParam5Caller(_T *obj,_R (_T::* _func) ( _TPARAM1 p1,_TPARAM2 p2,_TPARAM3 p3,_TPARAM4 p4,_TPARAM4 p5))
		{
			m_func =_func;
			ptr = obj;
		}

		inline virtual _R operator()( _TPARAM1 p1,_TPARAM2 p2,_TPARAM3 p3,_TPARAM4 p4,_TPARAM4 p5){

			return ((*ptr).*m_func) (p1,p2,p3,p4,p5);
		}

		virtual ~CFunctionMemberPointerRetParam5Caller(){}
	};
	//-------------------------------------------------------------------------------------------------
	// VOID RET PARAM6
	template <typename _R,typename _TPARAM1,typename _TPARAM2,typename _TPARAM3,typename _TPARAM4,typename _TPARAM5,typename _TPARAM6>
	class CFunctionMemberPointerRetParam6{
		public:
			virtual _R operator()(_TPARAM1 p1, _TPARAM2 p2, _TPARAM3 p3,_TPARAM4 p4,_TPARAM4 p5,_TPARAM4 p6)=0;

			virtual ~CFunctionMemberPointerRetParam6(){}
	};

	template <typename _R,class _T,typename _TPARAM1,typename _TPARAM2,typename _TPARAM3,typename _TPARAM4,typename _TPARAM5,typename _TPARAM6>
	class CFunctionMemberPointerRetParam6Caller: public CFunctionMemberPointerRetParam6<_R,_TPARAM1,_TPARAM2,_TPARAM3,_TPARAM4,_TPARAM4, _TPARAM6>{

		_R (_T::* m_func) (_TPARAM1 p1,_TPARAM2 p2,_TPARAM3 p3,_TPARAM4 p4,_TPARAM4 p5,_TPARAM4 p6);
		_T *ptr;
	public:
		CFunctionMemberPointerRetParam6Caller(_T *obj,_R (_T::* _func) ( _TPARAM1 p1,_TPARAM2 p2,_TPARAM3 p3,_TPARAM4 p4,_TPARAM4 p5,_TPARAM4 p6))
		{
			m_func =_func;
			ptr = obj;
		}

		inline virtual _R operator()( _TPARAM1 p1,_TPARAM2 p2,_TPARAM3 p3,_TPARAM4 p4,_TPARAM4 p5,_TPARAM4 p6){

			return ((*ptr).*m_func) (p1,p2,p3,p4,p5,p6);
		}

		virtual ~CFunctionMemberPointerRetParam6Caller(){}
	};

}
