#pragma once

#define ZS_NEW_VOID_FUNCTION_MEMBER_POINTER_PARAM_0(o,f)						(new zetscript::CMemberFunctionPointerVoidParam0Caller<o>(this,&o::f))
#define ZS_NEW_VOID_FUNCTION_MEMBER_POINTER_PARAM_1(o,f,p1)						(new zetscript::CMemberFunctionPointerVoidParam1Caller<o,p1>(this,&o::f))
#define ZS_NEW_VOID_FUNCTION_MEMBER_POINTER_PARAM_2(o,f,p1,p2)					(new zetscript::CMemberFunctionPointerVoidParam2Caller<o,p1,p2>(this,&o::f))
#define ZS_NEW_VOID_FUNCTION_MEMBER_POINTER_PARAM_3(o,f,p1,p2,p3)				(new zetscript::CMemberFunctionPointerVoidParam3Caller<o,p1,p2,p3>(this,&o::f))
#define ZS_NEW_VOID_FUNCTION_MEMBER_POINTER_PARAM_4(o,f,p1,p2,p3,p4)			(new zetscript::CMemberFunctionPointerVoidParam4Caller<o,p1,p2,p3,p4>(this,&o::f))
#define ZS_NEW_VOID_FUNCTION_MEMBER_POINTER_PARAM_5(o,f,p1,p2,p3,p4,p5)			(new zetscript::CMemberFunctionPointerVoidParam5Caller<o,p1,p2,p3,p4,p5>(this,&o::f))
#define ZS_NEW_VOID_FUNCTION_MEMBER_POINTER_PARAM_6(o,f,p1,p2,p3,p4,p5,p6)		(new zetscript::CMemberFunctionPointerVoidParam6Caller<o,p1,p2,p3,p4,p5,p6>(this,&o::f))

#define ZS_NEW_RET_FUNCTION_MEMBER_POINTER_PARAM_0(o,f)							(new zetscript::CMemberFunctionPointerRetParam0Caller<o>(this,&o::f))
#define ZS_NEW_RET_FUNCTION_MEMBER_POINTER_PARAM_1(o,f,p1)						(new zetscript::CMemberFunctionPointerRetParam1Caller<o,p1>(this,&o::f))
#define ZS_NEW_RET_FUNCTION_MEMBER_POINTER_PARAM_2(o,f,p1,p2)					(new zetscript::CMemberFunctionPointerRetParam2Caller<o,p1,p2>(this,&o::f))
#define ZS_NEW_RET_FUNCTION_MEMBER_POINTER_PARAM_3(o,f,p1,p2,p3)				(new zetscript::CMemberFunctionPointerRetParam3Caller<o,p1,p2,p3>(this,&o::f))
#define ZS_NEW_RET_FUNCTION_MEMBER_POINTER_PARAM_4(o,f,p1,p2,p3,p4)				(new zetscript::CMemberFunctionPointerRetParam4Caller<o,p1,p2,p3,p4>(this,&o::f))
#define ZS_NEW_RET_FUNCTION_MEMBER_POINTER_PARAM_5(o,f,p1,p2,p3,p4,p5)			(new zetscript::CMemberFunctionPointerRetParam5Caller<o,p1,p2,p3,p4,p5>(this,&o::f))
#define ZS_NEW_RET_FUNCTION_MEMBER_POINTER_PARAM_6(o,f,p1,p2,p3,p4,p5,p6)		(new zetscript::CMemberFunctionPointerRetParam6Caller<o,p1,p2,p3,p4,p5,p6>(this,&o::f))

namespace zetscript{



	//-------------------------------------------------------------------------------------------------
	// VOID RET PARAM0
	class CMemberFunctionPointerVoidParam0{
		public:
			virtual void operator()()=0;
			virtual ~CMemberFunctionPointerVoidParam0(){}
	};

	// Simple pointer function
	template <class _T>
	class CFunctionPointerVoidParam0Caller: public CMemberFunctionPointerVoidParam0{

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
	class CMemberFunctionPointerVoidParam0Caller: public CMemberFunctionPointerVoidParam0{

		void (_T::* m_func) ( );
		_T *ptr;
	public:
		CMemberFunctionPointerVoidParam0Caller(_T *obj,void (_T::* _func) ())
		{
			m_func =_func;
			ptr = obj;
		}

		inline virtual void operator()(){
			((*ptr).*m_func) ();
		}

		virtual ~CMemberFunctionPointerVoidParam0Caller(){}
	};



	//-------------------------------------------------------------------------------------------------
	// VOID RET PARAM1
	template <typename _TPARAM1>
	class CMemberFunctionPointerVoidParam1{
		public:
			virtual void operator()(_TPARAM1 p1)=0;

			virtual ~CMemberFunctionPointerVoidParam1(){}
	};

	template <class _T,typename _TPARAM1>
	class CMemberFunctionPointerVoidParam1Caller: public CMemberFunctionPointerVoidParam1<_TPARAM1>{

		void (_T::* m_func) (_TPARAM1 p1);
		_T *ptr;
	public:
		CMemberFunctionPointerVoidParam1Caller(_T *obj,void (_T::* _func) ( _TPARAM1 p1))
		{
			m_func =_func;
			ptr = obj;
		}

		inline virtual void operator()( _TPARAM1 p1){

			((*ptr).*m_func) (p1);
		}

		virtual ~CMemberFunctionPointerVoidParam1Caller(){}
	};
	//-------------------------------------------------------------------------------------------------
	// VOID RET PARAM2
	template <typename _TPARAM1,typename _TPARAM2>
	class CMemberFunctionPointerVoidParam2{
		public:
			virtual void operator()(_TPARAM1 p1, _TPARAM2 p2)=0;

			virtual ~CMemberFunctionPointerVoidParam2(){}
	};

	template <class _T,typename _TPARAM1,typename _TPARAM2>
	class CMemberFunctionPointerVoidParam2Caller: public CMemberFunctionPointerVoidParam2<_TPARAM1,_TPARAM2>{

		void (_T::* m_func) (_TPARAM1 p1,_TPARAM2 p2);
		_T *ptr;
	public:
		CMemberFunctionPointerVoidParam2Caller(_T *obj,void (_T::* _func) ( _TPARAM1 p1,_TPARAM2 p2))
		{
			m_func =_func;
			ptr = obj;
		}

		inline virtual void operator()( _TPARAM1 p1,_TPARAM2 p2){

			((*ptr).*m_func) (p1,p2);
		}

		virtual ~CMemberFunctionPointerVoidParam2Caller(){}
	};

	//-------------------------------------------------------------------------------------------------
	// VOID RET PARAM3
	template <typename _TPARAM1,typename _TPARAM2,typename _TPARAM3>
	class CMemberFunctionPointerVoidParam3{
		public:
			virtual void operator()(_TPARAM1 p1, _TPARAM2 p2, _TPARAM3 p3)=0;

			virtual ~CMemberFunctionPointerVoidParam3(){}
	};

	template <class _T,typename _TPARAM1,typename _TPARAM2,typename _TPARAM3>
	class CMemberFunctionPointerVoidParam3Caller: public CMemberFunctionPointerVoidParam3<_TPARAM1,_TPARAM2,_TPARAM3>{

		void (_T::* m_func) (_TPARAM1 p1,_TPARAM2 p2,_TPARAM3 p3);
		_T *ptr;
	public:
		CMemberFunctionPointerVoidParam3Caller(_T *obj,void (_T::* _func) ( _TPARAM1 p1,_TPARAM2 p2,_TPARAM3 p3))
		{
			m_func =_func;
			ptr = obj;
		}

		inline virtual void operator()( _TPARAM1 p1,_TPARAM2 p2,_TPARAM3 p3){

			((*ptr).*m_func) (p1,p2,p3);
		}

		virtual ~CMemberFunctionPointerVoidParam3Caller(){}
	};
	//-------------------------------------------------------------------------------------------------
	// VOID RET PARAM4
	template <typename _TPARAM1,typename _TPARAM2,typename _TPARAM3,typename _TPARAM4>
	class CMemberFunctionPointerVoidParam4{
		public:
			virtual void operator()(_TPARAM1 p1, _TPARAM2 p2, _TPARAM3 p3,_TPARAM4 p4)=0;

			virtual ~CMemberFunctionPointerVoidParam4(){}
	};

	template <class _T,typename _TPARAM1,typename _TPARAM2,typename _TPARAM3,typename _TPARAM4>
	class CMemberFunctionPointerVoidParam4Caller: public CMemberFunctionPointerVoidParam4<_TPARAM1,_TPARAM2,_TPARAM3,_TPARAM4>{

		void (_T::* m_func) (_TPARAM1 p1,_TPARAM2 p2,_TPARAM3 p3,_TPARAM4 p4);
		_T *ptr;
	public:
		CMemberFunctionPointerVoidParam4Caller(_T *obj,void (_T::* _func) ( _TPARAM1 p1,_TPARAM2 p2,_TPARAM3 p3,_TPARAM4 p4))
		{
			m_func =_func;
			ptr = obj;
		}

		inline virtual void operator()( _TPARAM1 p1,_TPARAM2 p2,_TPARAM3 p3,_TPARAM4 p4){

			((*ptr).*m_func) (p1,p2,p3,p4);
		}

		virtual ~CMemberFunctionPointerVoidParam4Caller(){}
	};
	//-------------------------------------------------------------------------------------------------
	// VOID RET PARAM5
	template <typename _TPARAM1,typename _TPARAM2,typename _TPARAM3,typename _TPARAM4,typename _TPARAM5>
	class CMemberFunctionPointerVoidParam5{
		public:
			virtual void operator()(_TPARAM1 p1, _TPARAM2 p2, _TPARAM3 p3,_TPARAM4 p4,_TPARAM4 p5)=0;

			virtual ~CMemberFunctionPointerVoidParam5(){}
	};

	template <class _T,typename _TPARAM1,typename _TPARAM2,typename _TPARAM3,typename _TPARAM4,typename _TPARAM5>
	class CMemberFunctionPointerVoidParam5Caller: public CMemberFunctionPointerVoidParam5<_TPARAM1,_TPARAM2,_TPARAM3,_TPARAM4,_TPARAM4>{

		void (_T::* m_func) (_TPARAM1 p1,_TPARAM2 p2,_TPARAM3 p3,_TPARAM4 p4,_TPARAM4 p5);
		_T *ptr;
	public:
		CMemberFunctionPointerVoidParam5Caller(_T *obj,void (_T::* _func) ( _TPARAM1 p1,_TPARAM2 p2,_TPARAM3 p3,_TPARAM4 p4,_TPARAM4 p5))
		{
			m_func =_func;
			ptr = obj;
		}

		inline virtual void operator()( _TPARAM1 p1,_TPARAM2 p2,_TPARAM3 p3,_TPARAM4 p4,_TPARAM4 p5){

			((*ptr).*m_func) (p1,p2,p3,p4,p5);
		}

		virtual ~CMemberFunctionPointerVoidParam5Caller(){}
	};
	//-------------------------------------------------------------------------------------------------
	// VOID RET PARAM6
	template <typename _TPARAM1,typename _TPARAM2,typename _TPARAM3,typename _TPARAM4,typename _TPARAM5,typename _TPARAM6>
	class CMemberFunctionPointerVoidParam6{
		public:
			virtual void operator()(_TPARAM1 p1, _TPARAM2 p2, _TPARAM3 p3,_TPARAM4 p4,_TPARAM4 p5,_TPARAM4 p6)=0;

			virtual ~CMemberFunctionPointerVoidParam6(){}
	};

	template <class _T,typename _TPARAM1,typename _TPARAM2,typename _TPARAM3,typename _TPARAM4,typename _TPARAM5,typename _TPARAM6>
	class CMemberFunctionPointerVoidParam6Caller: public CMemberFunctionPointerVoidParam6<_TPARAM1,_TPARAM2,_TPARAM3,_TPARAM4,_TPARAM4, _TPARAM6>{

		void (_T::* m_func) (_TPARAM1 p1,_TPARAM2 p2,_TPARAM3 p3,_TPARAM4 p4,_TPARAM4 p5,_TPARAM4 p6);
		_T *ptr;
	public:
		CMemberFunctionPointerVoidParam6Caller(_T *obj,void (_T::* _func) ( _TPARAM1 p1,_TPARAM2 p2,_TPARAM3 p3,_TPARAM4 p4,_TPARAM4 p5,_TPARAM4 p6))
		{
			m_func =_func;
			ptr = obj;
		}

		inline virtual void operator()( _TPARAM1 p1,_TPARAM2 p2,_TPARAM3 p3,_TPARAM4 p4,_TPARAM4 p5,_TPARAM4 p6){

			((*ptr).*m_func) (p1,p2,p3,p4,p5,p6);
		}

		virtual ~CMemberFunctionPointerVoidParam6Caller(){}
	};

	//-------------------------------------------------------------------------------------------------
	// RET PARAM0
	template <class _R>
	class CMemberFunctionPointerRetParam0{
		public:
			virtual _R operator()()=0;
			virtual ~CMemberFunctionPointerRetParam0(){}
	};

	template <class _R,class _T>
	class CMemberFunctionPointerRetParam0Caller: public CMemberFunctionPointerRetParam0<_R>{

		void (_T::* m_func) ( );
		_T *ptr;
	public:
		CMemberFunctionPointerRetParam0Caller(_T *obj,void (_T::* _func) ())
		{
			m_func =_func;
			ptr = obj;
		}

		inline virtual _R operator()(){
			return ((*ptr).*m_func) ();
		}

		virtual ~CMemberFunctionPointerRetParam0Caller(){}
	};

	//-------------------------------------------------------------------------------------------------
	// VOID RET PARAM1
	template <typename _R, typename _TPARAM1>
	class CMemberFunctionPointerRetParam1{
		public:
			virtual _R operator()(_TPARAM1 p1)=0;

			virtual ~CMemberFunctionPointerRetParam1(){}
	};

	template <typename _R,class _T,typename _TPARAM1>
	class CMemberFunctionPointerRetParam1Caller: public CMemberFunctionPointerRetParam1<_R,_TPARAM1>{

		_R (_T::* m_func) (_TPARAM1 p1);
		_T *ptr;
	public:
		CMemberFunctionPointerRetParam1Caller(_T *obj,void (_T::* _func) ( _TPARAM1 p1))
		{
			m_func =_func;
			ptr = obj;
		}

		inline virtual _R operator()( _TPARAM1 p1){

			return ((*ptr).*m_func) (p1);
		}

		virtual ~CMemberFunctionPointerRetParam1Caller(){}
	};
	//-------------------------------------------------------------------------------------------------
	// VOID RET PARAM2
	template <typename _R,typename _TPARAM1,typename _TPARAM2>
	class CMemberFunctionPointerRetParam2{
		public:
			virtual _R operator()(_TPARAM1 p1, _TPARAM2 p2)=0;

			virtual ~CMemberFunctionPointerRetParam2(){}
	};

	template <typename _R,class _T,typename _TPARAM1,typename _TPARAM2>
	class CMemberFunctionPointerRetParam2Caller: public CMemberFunctionPointerRetParam2<_R,_TPARAM1,_TPARAM2>{

		_R (_T::* m_func) (_TPARAM1 p1,_TPARAM2 p2);
		_T *ptr;
	public:
		CMemberFunctionPointerRetParam2Caller(_T *obj,_R (_T::* _func) ( _TPARAM1 p1,_TPARAM2 p2))
		{
			m_func =_func;
			ptr = obj;
		}

		inline virtual _R operator()( _TPARAM1 p1,_TPARAM2 p2){

			return ((*ptr).*m_func) (p1,p2);
		}

		virtual ~CMemberFunctionPointerRetParam2Caller(){}
	};

	//-------------------------------------------------------------------------------------------------
	// VOID RET PARAM3
	template <typename _R,typename _TPARAM1,typename _TPARAM2,typename _TPARAM3>
	class CMemberFunctionPointerRetParam3{
		public:
			virtual _R operator()(_TPARAM1 p1, _TPARAM2 p2, _TPARAM3 p3)=0;

			virtual ~CMemberFunctionPointerRetParam3(){}
	};

	template <typename _R,class _T,typename _TPARAM1,typename _TPARAM2,typename _TPARAM3>
	class CMemberFunctionPointerRetParam3Caller: public CMemberFunctionPointerRetParam3<_R,_TPARAM1,_TPARAM2,_TPARAM3>{

		_R (_T::* m_func) (_TPARAM1 p1,_TPARAM2 p2,_TPARAM3 p3);
		_T *ptr;
	public:
		CMemberFunctionPointerRetParam3Caller(_T *obj,_R (_T::* _func) ( _TPARAM1 p1,_TPARAM2 p2,_TPARAM3 p3))
		{
			m_func =_func;
			ptr = obj;
		}

		inline virtual _R operator()( _TPARAM1 p1,_TPARAM2 p2,_TPARAM3 p3){

			return ((*ptr).*m_func) (p1,p2,p3);
		}

		virtual ~CMemberFunctionPointerRetParam3Caller(){}
	};
	//-------------------------------------------------------------------------------------------------
	// VOID RET PARAM4
	template <typename _R,typename _TPARAM1,typename _TPARAM2,typename _TPARAM3,typename _TPARAM4>
	class CMemberFunctionPointerRetParam4{
		public:
			virtual _R operator()(_TPARAM1 p1, _TPARAM2 p2, _TPARAM3 p3,_TPARAM4 p4)=0;

			virtual ~CMemberFunctionPointerRetParam4(){}
	};

	template <typename _R,class _T,typename _TPARAM1,typename _TPARAM2,typename _TPARAM3,typename _TPARAM4>
	class CMemberFunctionPointerRetParam4Caller: public CMemberFunctionPointerRetParam4<_R,_TPARAM1,_TPARAM2,_TPARAM3,_TPARAM4>{

		_R (_T::* m_func) (_TPARAM1 p1,_TPARAM2 p2,_TPARAM3 p3,_TPARAM4 p4);
		_T *ptr;
	public:
		CMemberFunctionPointerRetParam4Caller(_T *obj,_R (_T::* _func) ( _TPARAM1 p1,_TPARAM2 p2,_TPARAM3 p3,_TPARAM4 p4))
		{
			m_func =_func;
			ptr = obj;
		}

		inline virtual _R operator()( _TPARAM1 p1,_TPARAM2 p2,_TPARAM3 p3,_TPARAM4 p4){

			return ((*ptr).*m_func) (p1,p2,p3,p4);
		}

		virtual ~CMemberFunctionPointerRetParam4Caller(){}
	};
	//-------------------------------------------------------------------------------------------------
	// VOID RET PARAM5
	template <typename _R,typename _TPARAM1,typename _TPARAM2,typename _TPARAM3,typename _TPARAM4,typename _TPARAM5>
	class CMemberFunctionPointerRetParam5{
		public:
			virtual _R operator()(_TPARAM1 p1, _TPARAM2 p2, _TPARAM3 p3,_TPARAM4 p4,_TPARAM4 p5)=0;

			virtual ~CMemberFunctionPointerRetParam5(){}
	};

	template <typename _R,class _T,typename _TPARAM1,typename _TPARAM2,typename _TPARAM3,typename _TPARAM4,typename _TPARAM5>
	class CMemberFunctionPointerRetParam5Caller: public CMemberFunctionPointerVoidParam5<_TPARAM1,_TPARAM2,_TPARAM3,_TPARAM4,_TPARAM4>{

		_R (_T::* m_func) (_TPARAM1 p1,_TPARAM2 p2,_TPARAM3 p3,_TPARAM4 p4,_TPARAM4 p5);
		_T *ptr;
	public:
		CMemberFunctionPointerRetParam5Caller(_T *obj,_R (_T::* _func) ( _TPARAM1 p1,_TPARAM2 p2,_TPARAM3 p3,_TPARAM4 p4,_TPARAM4 p5))
		{
			m_func =_func;
			ptr = obj;
		}

		inline virtual _R operator()( _TPARAM1 p1,_TPARAM2 p2,_TPARAM3 p3,_TPARAM4 p4,_TPARAM4 p5){

			return ((*ptr).*m_func) (p1,p2,p3,p4,p5);
		}

		virtual ~CMemberFunctionPointerRetParam5Caller(){}
	};
	//-------------------------------------------------------------------------------------------------
	// VOID RET PARAM6
	template <typename _R,typename _TPARAM1,typename _TPARAM2,typename _TPARAM3,typename _TPARAM4,typename _TPARAM5,typename _TPARAM6>
	class CMemberFunctionPointerRetParam6{
		public:
			virtual _R operator()(_TPARAM1 p1, _TPARAM2 p2, _TPARAM3 p3,_TPARAM4 p4,_TPARAM4 p5,_TPARAM4 p6)=0;

			virtual ~CMemberFunctionPointerRetParam6(){}
	};

	template <typename _R,class _T,typename _TPARAM1,typename _TPARAM2,typename _TPARAM3,typename _TPARAM4,typename _TPARAM5,typename _TPARAM6>
	class CMemberFunctionPointerRetParam6Caller: public CMemberFunctionPointerRetParam6<_R,_TPARAM1,_TPARAM2,_TPARAM3,_TPARAM4,_TPARAM4, _TPARAM6>{

		_R (_T::* m_func) (_TPARAM1 p1,_TPARAM2 p2,_TPARAM3 p3,_TPARAM4 p4,_TPARAM4 p5,_TPARAM4 p6);
		_T *ptr;
	public:
		CMemberFunctionPointerRetParam6Caller(_T *obj,_R (_T::* _func) ( _TPARAM1 p1,_TPARAM2 p2,_TPARAM3 p3,_TPARAM4 p4,_TPARAM4 p5,_TPARAM4 p6))
		{
			m_func =_func;
			ptr = obj;
		}

		inline virtual _R operator()( _TPARAM1 p1,_TPARAM2 p2,_TPARAM3 p3,_TPARAM4 p4,_TPARAM4 p5,_TPARAM4 p6){

			return ((*ptr).*m_func) (p1,p2,p3,p4,p5,p6);
		}

		virtual ~CMemberFunctionPointerRetParam6Caller(){}
	};

}
