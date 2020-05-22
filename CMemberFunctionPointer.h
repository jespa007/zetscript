#pragma once

#define ZS_NEW_VOID_FUNCTION_MEMBER_POINTER_PARAM_0(class_type,o,f)	(new zetscript::CMemberFunctionPointerVoidParam0Caller<class_type>(o,f))
#define ZS_NEW_VOID_FUNCTION_MEMBER_POINTER_PARAM_1(class_type,o,f)	(new zetscript::CMemberFunctionPointerVoidParam1Caller<class_type>(o,f))
#define ZS_NEW_VOID_FUNCTION_MEMBER_POINTER_PARAM_2(class_type,o,f)	(new zetscript::CMemberFunctionPointerVoidParam2Caller<class_type>(o,f))
#define ZS_NEW_VOID_FUNCTION_MEMBER_POINTER_PARAM_3(class_type,o,f)	(new zetscript::CMemberFunctionPointerVoidParam3Caller<class_type>(o,f))
#define ZS_NEW_VOID_FUNCTION_MEMBER_POINTER_PARAM_4(class_type,o,f)	(new zetscript::CMemberFunctionPointerVoidParam4Caller<class_type>(o,f))
#define ZS_NEW_VOID_FUNCTION_MEMBER_POINTER_PARAM_5(class_type,o,f)	(new zetscript::CMemberFunctionPointerVoidParam5Caller<class_type>(o,f))
#define ZS_NEW_VOID_FUNCTION_MEMBER_POINTER_PARAM_6(class_type,o,f)	(new zetscript::CMemberFunctionPointerVoidParam6Caller<class_type>(o,f))

#define ZS_NEW_RET_FUNCTION_MEMBER_POINTER_PARAM_0(class_type,o,f)	(new zetscript::CMemberFunctionPointerRetParam0Caller<class_type>(o,f))
#define ZS_NEW_RET_FUNCTION_MEMBER_POINTER_PARAM_1(class_type,o,f)	(new zetscript::CMemberFunctionPointerRetParam1Caller<class_type>(o,f))
#define ZS_NEW_RET_FUNCTION_MEMBER_POINTER_PARAM_2(class_type,o,f)	(new zetscript::CMemberFunctionPointerRetParam2Caller<class_type>(o,f))
#define ZS_NEW_RET_FUNCTION_MEMBER_POINTER_PARAM_3(class_type,o,f)	(new zetscript::CMemberFunctionPointerRetParam3Caller<class_type>(o,f))
#define ZS_NEW_RET_FUNCTION_MEMBER_POINTER_PARAM_4(class_type,o,f)	(new zetscript::CMemberFunctionPointerRetParam4Caller<class_type>(o,f))
#define ZS_NEW_RET_FUNCTION_MEMBER_POINTER_PARAM_5(class_type,o,f)	(new zetscript::CMemberFunctionPointerRetParam5Caller<class_type>(o,f))
#define ZS_NEW_RET_FUNCTION_MEMBER_POINTER_PARAM_6(class_type,o,f)	(new zetscript::CMemberFunctionPointerRetParam6Caller<class_type>(o,f))

namespace zetscript{


	class CMemberFunctionPointer{
	};

	//-------------------------------------------------------------------------------------------------
	// VOID/RET PARAM0
	class CMemberFunctionPointerVoidParam0:public CMemberFunctionPointer{
		public:
			virtual void operator()()=0;
			virtual ~CMemberFunctionPointerVoidParam0(){}
	};

	// Pointer member function
	template <class _T,class _S>
	class CMemberFunctionPointerVoidParam0Caller: public CMemberFunctionPointerVoidParam0{

		void (_S::* m_func) ( );
		_T *ptr;
	public:
		CMemberFunctionPointerVoidParam0Caller(_T *obj,void (_S::* _func) ())
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
	// VOID/RET PARAM1

	class CMemberFunctionPointerVoidParam1:public CMemberFunctionPointer{
		public:
			virtual void operator()(intptr_t p1)=0;

			virtual ~CMemberFunctionPointerVoidParam1(){}
	};

	template <class _T,class _S>
	class CMemberFunctionPointerVoidParam1Caller: public CMemberFunctionPointerVoidParam1{

		void (_S::* m_func) (intptr_t p1);
		_T *ptr;
	public:
		CMemberFunctionPointerVoidParam1Caller(_T *obj,void (_S::* _func) ( intptr_t p1))
		{
			m_func =_func;
			ptr = obj;
		}

		inline virtual void operator()( intptr_t p1){

			((*ptr).*m_func) (p1);
		}

		virtual ~CMemberFunctionPointerVoidParam1Caller(){}
	};
	//-------------------------------------------------------------------------------------------------
	// VOID/RET PARAM2

	class CMemberFunctionPointerVoidParam2:public CMemberFunctionPointer{
		public:
			virtual void operator()(intptr_t p1, intptr_t p2)=0;

			virtual ~CMemberFunctionPointerVoidParam2(){}
	};

	template <class _T,class _S>
	class CMemberFunctionPointerVoidParam2Caller: public CMemberFunctionPointerVoidParam2{

		void (_S::* m_func) (intptr_t p1,intptr_t p2);
		_T *ptr;
	public:
		CMemberFunctionPointerVoidParam2Caller(_T *obj,void (_S::* _func) ( intptr_t p1,intptr_t p2))
		{
			m_func =_func;
			ptr = obj;
		}

		inline virtual void operator()( intptr_t p1,intptr_t p2){

			((*ptr).*m_func) (p1,p2);
		}

		virtual ~CMemberFunctionPointerVoidParam2Caller(){}
	};

	//-------------------------------------------------------------------------------------------------
	// VOID/RET PARAM3

	class CMemberFunctionPointerVoidParam3:public CMemberFunctionPointer{
		public:
			virtual void operator()(intptr_t p1, intptr_t p2, intptr_t p3)=0;

			virtual ~CMemberFunctionPointerVoidParam3(){}
	};

	template <class _T,class _S>
	class CMemberFunctionPointerVoidParam3Caller: public CMemberFunctionPointerVoidParam3{

		void (_S::* m_func) (intptr_t p1,intptr_t p2,intptr_t p3);
		_T *ptr;
	public:
		CMemberFunctionPointerVoidParam3Caller(_T *obj,void (_S::* _func) ( intptr_t p1,intptr_t p2,intptr_t p3))
		{
			m_func =_func;
			ptr = obj;
		}

		inline virtual void operator()( intptr_t p1,intptr_t p2,intptr_t p3){

			((*ptr).*m_func) (p1,p2,p3);
		}

		virtual ~CMemberFunctionPointerVoidParam3Caller(){}
	};
	//-------------------------------------------------------------------------------------------------
	// VOID/RET PARAM4

	class CMemberFunctionPointerVoidParam4:public CMemberFunctionPointer{
		public:
			virtual void operator()(intptr_t p1, intptr_t p2, intptr_t p3,intptr_t p4)=0;

			virtual ~CMemberFunctionPointerVoidParam4(){}
	};

	template <class _T,class _S>
	class CMemberFunctionPointerVoidParam4Caller: public CMemberFunctionPointerVoidParam4{

		void (_S::* m_func) (intptr_t p1,intptr_t p2,intptr_t p3,intptr_t p4);
		_T *ptr;
	public:
		CMemberFunctionPointerVoidParam4Caller(_T *obj,void (_S::* _func) ( intptr_t p1,intptr_t p2,intptr_t p3,intptr_t p4))
		{
			m_func =_func;
			ptr = obj;
		}

		inline virtual void operator()( intptr_t p1,intptr_t p2,intptr_t p3,intptr_t p4){

			((*ptr).*m_func) (p1,p2,p3,p4);
		}

		virtual ~CMemberFunctionPointerVoidParam4Caller(){}
	};
	//-------------------------------------------------------------------------------------------------
	// VOID/RET PARAM5
	class CMemberFunctionPointerVoidParam5:public CMemberFunctionPointer{
		public:
			virtual void operator()(intptr_t p1, intptr_t p2, intptr_t p3,intptr_t p4,intptr_t p5)=0;

			virtual ~CMemberFunctionPointerVoidParam5(){}
	};

	template <class _T,class _S>
	class CMemberFunctionPointerVoidParam5Caller: public CMemberFunctionPointerVoidParam5{

		void (_S::* m_func) (intptr_t p1,intptr_t p2,intptr_t p3,intptr_t p4,intptr_t p5);
		_T *ptr;
	public:
		CMemberFunctionPointerVoidParam5Caller(_T *obj,void (_S::* _func) ( intptr_t p1,intptr_t p2,intptr_t p3,intptr_t p4,intptr_t p5))
		{
			m_func =_func;
			ptr = obj;
		}

		inline virtual void operator()( intptr_t p1,intptr_t p2,intptr_t p3,intptr_t p4,intptr_t p5){

			((*ptr).*m_func) (p1,p2,p3,p4,p5);
		}

		virtual ~CMemberFunctionPointerVoidParam5Caller(){}
	};
	//-------------------------------------------------------------------------------------------------
	// VOID/RET PARAM6
	class CMemberFunctionPointerVoidParam6:public CMemberFunctionPointer{
		public:
			virtual void operator()(intptr_t p1, intptr_t p2, intptr_t p3,intptr_t p4,intptr_t p5,intptr_t p6)=0;

			virtual ~CMemberFunctionPointerVoidParam6(){}
	};

	template <class _T,class _S>
	class CMemberFunctionPointerVoidParam6Caller: public CMemberFunctionPointerVoidParam6 {

		void (_S::* m_func) (intptr_t p1,intptr_t p2,intptr_t p3,intptr_t p4,intptr_t p5,intptr_t p6);
		_T *ptr;
	public:
		CMemberFunctionPointerVoidParam6Caller(_T *obj,void (_S::* _func) ( intptr_t p1,intptr_t p2,intptr_t p3,intptr_t p4,intptr_t p5,intptr_t p6))
		{
			m_func =_func;
			ptr = obj;
		}

		inline virtual void operator()( intptr_t p1,intptr_t p2,intptr_t p3,intptr_t p4,intptr_t p5,intptr_t p6){

			((*ptr).*m_func) (p1,p2,p3,p4,p5,p6);
		}

		virtual ~CMemberFunctionPointerVoidParam6Caller(){}
	};

	//-------------------------------------------------------------------------------------------------
	// RET PARAM0

	class CMemberFunctionPointerRetParam0:public CMemberFunctionPointer{
		public:
			virtual intptr_t operator()()=0;
			virtual bool ret_bool()=0;
			virtual float ret_float()=0;

			virtual ~CMemberFunctionPointerRetParam0(){}
	};

	template <class _T,class _S>
	class CMemberFunctionPointerRetParam0Caller: public CMemberFunctionPointerRetParam0{

		intptr_t (_S::* m_func) ( );
		_T *ptr;
	public:
		CMemberFunctionPointerRetParam0Caller(_T *obj,intptr_t (_S::* _func) ())
		{
			m_func =_func;
			ptr = obj;
		}

		inline virtual intptr_t operator()(){
			return ((*ptr).*m_func) ();
		}

		inline virtual float ret_float( ){

			return ((*ptr).*((float (_S::*)())(m_func)))();

		}

		inline virtual bool ret_bool( ){

			return ((*ptr).*((bool (_S::*)())(m_func)))();

		}

		virtual ~CMemberFunctionPointerRetParam0Caller(){}
	};

	//-------------------------------------------------------------------------------------------------
	// VOID/RET PARAM1
	class CMemberFunctionPointerRetParam1:public CMemberFunctionPointer{
		public:
			virtual intptr_t operator()(intptr_t p1)=0;
			virtual bool ret_bool(intptr_t p1)=0;
			virtual float ret_float(intptr_t p1)=0;

			virtual ~CMemberFunctionPointerRetParam1(){}
	};

	template <class _T,class _S>
	class CMemberFunctionPointerRetParam1Caller: public CMemberFunctionPointerRetParam1{

		intptr_t (_S::* m_func) (intptr_t p1);
		_T *ptr;
	public:
		CMemberFunctionPointerRetParam1Caller(_T *obj,intptr_t (_S::* _func) ( intptr_t p1))
		{
			m_func =_func;
			ptr = obj;
		}

		inline virtual intptr_t operator()( intptr_t p1){

			return ((*ptr).*m_func) (p1);
		}

		inline virtual float ret_float( intptr_t p1){

			return ((*ptr).*((float (_S::*)(intptr_t))(m_func)))(p1);

		}

		inline virtual bool ret_bool( intptr_t p1){

			return ((*ptr).*((bool (_S::*)(intptr_t))(m_func)))(p1);

		}


		virtual ~CMemberFunctionPointerRetParam1Caller(){}
	};
	//-------------------------------------------------------------------------------------------------
	// VOID/RET PARAM2
	class CMemberFunctionPointerRetParam2:public CMemberFunctionPointer{
		public:
			virtual intptr_t operator()(intptr_t p1, intptr_t p2)=0;
			virtual bool ret_bool(intptr_t p1, intptr_t p2)=0;
			virtual float ret_float(intptr_t p1, intptr_t p2)=0;



			virtual ~CMemberFunctionPointerRetParam2(){}
	};

	template <class _T,class _S>
	class CMemberFunctionPointerRetParam2Caller: public CMemberFunctionPointerRetParam2{

		intptr_t (_S::* m_func) (intptr_t p1,intptr_t p2);
		_T *ptr;
	public:
		CMemberFunctionPointerRetParam2Caller(_T *obj,intptr_t (_S::* _func) ( intptr_t p1,intptr_t p2))
		{
			m_func =_func;
			ptr = obj;
		}

		inline virtual intptr_t operator()( intptr_t p1,intptr_t p2){

			return ((*ptr).*m_func) (p1,p2);
		}

		inline virtual float ret_float( intptr_t p1,intptr_t p2){

			return ((*ptr).*((float (_S::*)(intptr_t,intptr_t))(m_func)))(p1,p2);

		}

		inline virtual bool ret_bool(intptr_t p1,intptr_t p2 ){

			return ((*ptr).*((bool (_S::*)(intptr_t,intptr_t))(m_func)))(p1,p2);

		}

		virtual ~CMemberFunctionPointerRetParam2Caller(){}
	};

	//-------------------------------------------------------------------------------------------------
	// VOID/RET PARAM3
	class CMemberFunctionPointerRetParam3:public CMemberFunctionPointer{
		public:
			virtual intptr_t operator()(intptr_t p1, intptr_t p2, intptr_t p3)=0;
			virtual bool ret_bool(intptr_t p1, intptr_t p2, intptr_t p3)=0;
			virtual float ret_float(intptr_t p1, intptr_t p2, intptr_t p3)=0;


			virtual ~CMemberFunctionPointerRetParam3(){}
	};

	template <class _T,class _S>
	class CMemberFunctionPointerRetParam3Caller: public CMemberFunctionPointerRetParam3{

		intptr_t (_S::* m_func) (intptr_t p1,intptr_t p2,intptr_t p3);

		_T *ptr;
	public:
		CMemberFunctionPointerRetParam3Caller(_T *obj,intptr_t (_S::* _func) ( intptr_t p1,intptr_t p2,intptr_t p3))
		{
			m_func =_func;
			ptr = obj;
		}

		inline virtual intptr_t operator()( intptr_t p1,intptr_t p2,intptr_t p3){

			return ((*ptr).*m_func) (p1,p2,p3);
		}

		inline virtual float ret_float(intptr_t p1,intptr_t p2,intptr_t p3 ){

			return ((*ptr).*((float (_S::*)(intptr_t,intptr_t,intptr_t))(m_func)))(p1,p2,p3);

		}

		inline virtual bool ret_bool(intptr_t p1,intptr_t p2,intptr_t p3 ){

			return ((*ptr).*((bool (_S::*)(intptr_t,intptr_t,intptr_t))(m_func)))(p1,p2,p3);

		}

		virtual ~CMemberFunctionPointerRetParam3Caller(){}
	};
	//-------------------------------------------------------------------------------------------------
	// VOID/RET PARAM4
	class CMemberFunctionPointerRetParam4:public CMemberFunctionPointer{
		public:
			virtual intptr_t operator()(intptr_t p1, intptr_t p2, intptr_t p3,intptr_t p4)=0;
			virtual bool ret_bool(intptr_t p1, intptr_t p2, intptr_t p3,intptr_t p4)=0;
			virtual float ret_float(intptr_t p1, intptr_t p2, intptr_t p3,intptr_t p4)=0;


			virtual ~CMemberFunctionPointerRetParam4(){}
	};

	template <class _T,class _S>
	class CMemberFunctionPointerRetParam4Caller: public CMemberFunctionPointerRetParam4{

		intptr_t (_S::* m_func) (intptr_t p1,intptr_t p2,intptr_t p3,intptr_t p4);
		_T *ptr;
	public:
		CMemberFunctionPointerRetParam4Caller(_T *obj,intptr_t (_S::* _func) ( intptr_t p1,intptr_t p2,intptr_t p3,intptr_t p4))
		{
			m_func =_func;
			ptr = obj;
		}

		inline virtual intptr_t operator()( intptr_t p1,intptr_t p2,intptr_t p3,intptr_t p4){

			return ((*ptr).*m_func) (p1,p2,p3,p4);
		}

		inline virtual float ret_float( intptr_t p1,intptr_t p2,intptr_t p3,intptr_t p4){

			return ((*ptr).*((float (_S::*)(intptr_t,intptr_t,intptr_t,intptr_t))(m_func)))(p1,p2,p3,p4);

		}

		inline virtual bool ret_bool( intptr_t p1,intptr_t p2,intptr_t p3,intptr_t p4){

			return ((*ptr).*((bool (_S::*)(intptr_t,intptr_t,intptr_t,intptr_t))(m_func)))(p1,p2,p3,p4);

		}

		virtual ~CMemberFunctionPointerRetParam4Caller(){}
	};
	//-------------------------------------------------------------------------------------------------
	// VOID/RET PARAM5
	class CMemberFunctionPointerRetParam5:public CMemberFunctionPointer{
		public:
			virtual intptr_t operator()(intptr_t p1, intptr_t p2, intptr_t p3,intptr_t p4,intptr_t p5)=0;
			virtual bool ret_bool(intptr_t p1, intptr_t p2, intptr_t p3,intptr_t p4,intptr_t p5)=0;
			virtual float ret_float(intptr_t p1, intptr_t p2, intptr_t p3,intptr_t p4,intptr_t p5)=0;


			virtual ~CMemberFunctionPointerRetParam5(){}
	};

	template <class _T,class _S>
	class CMemberFunctionPointerRetParam5Caller: public CMemberFunctionPointerRetParam5{

		intptr_t (_S::* m_func) (intptr_t p1,intptr_t p2,intptr_t p3,intptr_t p4,intptr_t p5);
		_T *ptr;
	public:
		CMemberFunctionPointerRetParam5Caller(_T *obj,intptr_t (_S::* _func) ( intptr_t p1,intptr_t p2,intptr_t p3,intptr_t p4,intptr_t p5))
		{
			m_func =_func;
			ptr = obj;
		}

		inline virtual intptr_t operator()( intptr_t p1,intptr_t p2,intptr_t p3,intptr_t p4,intptr_t p5){

			return ((*ptr).*m_func) (p1,p2,p3,p4,p5);
		}

		inline virtual float ret_float(  intptr_t p1,intptr_t p2,intptr_t p3,intptr_t p4,intptr_t p5){

			return ((*ptr).*((float (_S::*)(intptr_t,intptr_t,intptr_t,intptr_t,intptr_t))(m_func)))(p1,p2,p3,p4,p5);

		}

		inline virtual bool ret_bool(  intptr_t p1,intptr_t p2,intptr_t p3,intptr_t p4,intptr_t p5){

			return ((*ptr).*((bool (_S::*)(intptr_t,intptr_t,intptr_t,intptr_t,intptr_t))(m_func)))(p1,p2,p3,p4,p5);

		}

		virtual ~CMemberFunctionPointerRetParam5Caller(){}
	};
	//-------------------------------------------------------------------------------------------------
	// VOID/RET PARAM6
	class CMemberFunctionPointerRetParam6:public CMemberFunctionPointer{
		public:
			virtual intptr_t operator()(intptr_t p1, intptr_t p2, intptr_t p3,intptr_t p4,intptr_t p5,intptr_t p6)=0;
			virtual bool ret_bool(intptr_t p1, intptr_t p2, intptr_t p3,intptr_t p4,intptr_t p5,intptr_t p6)=0;
			virtual float ret_float(intptr_t p1, intptr_t p2, intptr_t p3,intptr_t p4,intptr_t p5,intptr_t p6)=0;


			virtual ~CMemberFunctionPointerRetParam6(){}
	};

	template <class _T,class _S>
	class CMemberFunctionPointerRetParam6Caller: public CMemberFunctionPointerRetParam6{

		intptr_t (_S::* m_func) (intptr_t p1,intptr_t p2,intptr_t p3,intptr_t p4,intptr_t p5,intptr_t p6);
		_T *ptr;
	public:
		CMemberFunctionPointerRetParam6Caller(_T *obj,intptr_t (_S::* _func) ( intptr_t p1,intptr_t p2,intptr_t p3,intptr_t p4,intptr_t p5,intptr_t p6))
		{
			m_func =_func;
			ptr = obj;
		}

		inline virtual intptr_t operator()( intptr_t p1,intptr_t p2,intptr_t p3,intptr_t p4,intptr_t p5,intptr_t p6){

			return ((*ptr).*m_func) (p1,p2,p3,p4,p5,p6);
		}

		inline virtual float ret_float( intptr_t p1,intptr_t p2,intptr_t p3,intptr_t p4,intptr_t p5,intptr_t p6){

			return ((*ptr).*((float (_S::*)(intptr_t,intptr_t,intptr_t,intptr_t,intptr_t,intptr_t))(m_func)))(p1,p2,p3,p4,p5,p6);

		}

		inline virtual bool ret_bool(intptr_t p1,intptr_t p2,intptr_t p3,intptr_t p4,intptr_t p5,intptr_t p6 ){

			return ((*ptr).*((bool (_S::*)(intptr_t,intptr_t,intptr_t,intptr_t,intptr_t,intptr_t))(m_func)))(p1,p2,p3,p4,p5,p6);

		}

		virtual ~CMemberFunctionPointerRetParam6Caller(){}
	};

}
