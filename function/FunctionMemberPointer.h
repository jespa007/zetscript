#pragma once

#define ZS_NEW_VOID_FUNCTION_MEMBER_POINTER_PARAM_0(class_type,o,f)	(new ZetScript::CFunctionMemberPointerVoidParam0Caller<class_type>(o,f))
#define ZS_NEW_VOID_FUNCTION_MEMBER_POINTER_PARAM_1(class_type,o,f)	(new ZetScript::CFunctionMemberPointerVoidParam1Caller<class_type>(o,f))
#define ZS_NEW_VOID_FUNCTION_MEMBER_POINTER_PARAM_2(class_type,o,f)	(new ZetScript::CFunctionMemberPointerVoidParam2Caller<class_type>(o,f))
#define ZS_NEW_VOID_FUNCTION_MEMBER_POINTER_PARAM_3(class_type,o,f)	(new ZetScript::CFunctionMemberPointerVoidParam3Caller<class_type>(o,f))
#define ZS_NEW_VOID_FUNCTION_MEMBER_POINTER_PARAM_4(class_type,o,f)	(new ZetScript::CFunctionMemberPointerVoidParam4Caller<class_type>(o,f))
#define ZS_NEW_VOID_FUNCTION_MEMBER_POINTER_PARAM_5(class_type,o,f)	(new ZetScript::CFunctionMemberPointerVoidParam5Caller<class_type>(o,f))
#define ZS_NEW_VOID_FUNCTION_MEMBER_POINTER_PARAM_6(class_type,o,f)	(new ZetScript::CFunctionMemberPointerVoidParam6Caller<class_type>(o,f))

#define ZS_NEW_RET_FUNCTION_MEMBER_POINTER_PARAM_0(class_type,o,f)	(new ZetScript::CFunctionMemberPointerRetParam0Caller<class_type>(o,f))
#define ZS_NEW_RET_FUNCTION_MEMBER_POINTER_PARAM_1(class_type,o,f)	(new ZetScript::CFunctionMemberPointerRetParam1Caller<class_type>(o,f))
#define ZS_NEW_RET_FUNCTION_MEMBER_POINTER_PARAM_2(class_type,o,f)	(new ZetScript::CFunctionMemberPointerRetParam2Caller<class_type>(o,f))
#define ZS_NEW_RET_FUNCTION_MEMBER_POINTER_PARAM_3(class_type,o,f)	(new ZetScript::CFunctionMemberPointerRetParam3Caller<class_type>(o,f))
#define ZS_NEW_RET_FUNCTION_MEMBER_POINTER_PARAM_4(class_type,o,f)	(new ZetScript::CFunctionMemberPointerRetParam4Caller<class_type>(o,f))
#define ZS_NEW_RET_FUNCTION_MEMBER_POINTER_PARAM_5(class_type,o,f)	(new ZetScript::CFunctionMemberPointerRetParam5Caller<class_type>(o,f))
#define ZS_NEW_RET_FUNCTION_MEMBER_POINTER_PARAM_6(class_type,o,f)	(new ZetScript::CFunctionMemberPointerRetParam6Caller<class_type>(o,f))

namespace zetscript{


	class CFunctionMemberPointer{
	};

	//-------------------------------------------------------------------------------------------------
	// VOID/BYTE_CODE_RET PARAM0
	class CFunctionMemberPointerVoidParam0:public CFunctionMemberPointer{
		public:
			virtual void operator()()=0;
			virtual ~CFunctionMemberPointerVoidParam0(){}
	};

	// Pointer member function
	template <class _T,class _S>
	class CFunctionMemberPointerVoidParam0Caller: public CFunctionMemberPointerVoidParam0{

		void (_S::* m_func) ( );
		_T *ptr;
	public:
		CFunctionMemberPointerVoidParam0Caller(_T *obj,void (_S::* _func) ())
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
	// VOID/BYTE_CODE_RET PARAM1

	class CFunctionMemberPointerVoidParam1:public CFunctionMemberPointer{
		public:
			virtual void operator()(intptr_t p1)=0;

			virtual ~CFunctionMemberPointerVoidParam1(){}
	};

	template <class _T,class _S>
	class CFunctionMemberPointerVoidParam1Caller: public CFunctionMemberPointerVoidParam1{

		void (_S::* m_func) (intptr_t p1);
		_T *ptr;
	public:
		CFunctionMemberPointerVoidParam1Caller(_T *obj,void (_S::* _func) ( intptr_t p1))
		{
			m_func =_func;
			ptr = obj;
		}

		inline virtual void operator()( intptr_t p1){

			((*ptr).*m_func) (p1);
		}

		virtual ~CFunctionMemberPointerVoidParam1Caller(){}
	};
	//-------------------------------------------------------------------------------------------------
	// VOID/BYTE_CODE_RET PARAM2

	class CFunctionMemberPointerVoidParam2:public CFunctionMemberPointer{
		public:
			virtual void operator()(intptr_t p1, intptr_t p2)=0;

			virtual ~CFunctionMemberPointerVoidParam2(){}
	};

	template <class _T,class _S>
	class CFunctionMemberPointerVoidParam2Caller: public CFunctionMemberPointerVoidParam2{

		void (_S::* m_func) (intptr_t p1,intptr_t p2);
		_T *ptr;
	public:
		CFunctionMemberPointerVoidParam2Caller(_T *obj,void (_S::* _func) ( intptr_t p1,intptr_t p2))
		{
			m_func =_func;
			ptr = obj;
		}

		inline virtual void operator()( intptr_t p1,intptr_t p2){

			((*ptr).*m_func) (p1,p2);
		}

		virtual ~CFunctionMemberPointerVoidParam2Caller(){}
	};

	//-------------------------------------------------------------------------------------------------
	// VOID/BYTE_CODE_RET PARAM3

	class CFunctionMemberPointerVoidParam3:public CFunctionMemberPointer{
		public:
			virtual void operator()(intptr_t p1, intptr_t p2, intptr_t p3)=0;

			virtual ~CFunctionMemberPointerVoidParam3(){}
	};

	template <class _T,class _S>
	class CFunctionMemberPointerVoidParam3Caller: public CFunctionMemberPointerVoidParam3{

		void (_S::* m_func) (intptr_t p1,intptr_t p2,intptr_t p3);
		_T *ptr;
	public:
		CFunctionMemberPointerVoidParam3Caller(_T *obj,void (_S::* _func) ( intptr_t p1,intptr_t p2,intptr_t p3))
		{
			m_func =_func;
			ptr = obj;
		}

		inline virtual void operator()( intptr_t p1,intptr_t p2,intptr_t p3){

			((*ptr).*m_func) (p1,p2,p3);
		}

		virtual ~CFunctionMemberPointerVoidParam3Caller(){}
	};
	//-------------------------------------------------------------------------------------------------
	// VOID/BYTE_CODE_RET PARAM4

	class CFunctionMemberPointerVoidParam4:public CFunctionMemberPointer{
		public:
			virtual void operator()(intptr_t p1, intptr_t p2, intptr_t p3,intptr_t p4)=0;

			virtual ~CFunctionMemberPointerVoidParam4(){}
	};

	template <class _T,class _S>
	class CFunctionMemberPointerVoidParam4Caller: public CFunctionMemberPointerVoidParam4{

		void (_S::* m_func) (intptr_t p1,intptr_t p2,intptr_t p3,intptr_t p4);
		_T *ptr;
	public:
		CFunctionMemberPointerVoidParam4Caller(_T *obj,void (_S::* _func) ( intptr_t p1,intptr_t p2,intptr_t p3,intptr_t p4))
		{
			m_func =_func;
			ptr = obj;
		}

		inline virtual void operator()( intptr_t p1,intptr_t p2,intptr_t p3,intptr_t p4){

			((*ptr).*m_func) (p1,p2,p3,p4);
		}

		virtual ~CFunctionMemberPointerVoidParam4Caller(){}
	};
	//-------------------------------------------------------------------------------------------------
	// VOID/BYTE_CODE_RET PARAM5
	class CFunctionMemberPointerVoidParam5:public CFunctionMemberPointer{
		public:
			virtual void operator()(intptr_t p1, intptr_t p2, intptr_t p3,intptr_t p4,intptr_t p5)=0;

			virtual ~CFunctionMemberPointerVoidParam5(){}
	};

	template <class _T,class _S>
	class CFunctionMemberPointerVoidParam5Caller: public CFunctionMemberPointerVoidParam5{

		void (_S::* m_func) (intptr_t p1,intptr_t p2,intptr_t p3,intptr_t p4,intptr_t p5);
		_T *ptr;
	public:
		CFunctionMemberPointerVoidParam5Caller(_T *obj,void (_S::* _func) ( intptr_t p1,intptr_t p2,intptr_t p3,intptr_t p4,intptr_t p5))
		{
			m_func =_func;
			ptr = obj;
		}

		inline virtual void operator()( intptr_t p1,intptr_t p2,intptr_t p3,intptr_t p4,intptr_t p5){

			((*ptr).*m_func) (p1,p2,p3,p4,p5);
		}

		virtual ~CFunctionMemberPointerVoidParam5Caller(){}
	};
	//-------------------------------------------------------------------------------------------------
	// VOID/BYTE_CODE_RET PARAM6
	class CFunctionMemberPointerVoidParam6:public CFunctionMemberPointer{
		public:
			virtual void operator()(intptr_t p1, intptr_t p2, intptr_t p3,intptr_t p4,intptr_t p5,intptr_t p6)=0;

			virtual ~CFunctionMemberPointerVoidParam6(){}
	};

	template <class _T,class _S>
	class CFunctionMemberPointerVoidParam6Caller: public CFunctionMemberPointerVoidParam6 {

		void (_S::* m_func) (intptr_t p1,intptr_t p2,intptr_t p3,intptr_t p4,intptr_t p5,intptr_t p6);
		_T *ptr;
	public:
		CFunctionMemberPointerVoidParam6Caller(_T *obj,void (_S::* _func) ( intptr_t p1,intptr_t p2,intptr_t p3,intptr_t p4,intptr_t p5,intptr_t p6))
		{
			m_func =_func;
			ptr = obj;
		}

		inline virtual void operator()( intptr_t p1,intptr_t p2,intptr_t p3,intptr_t p4,intptr_t p5,intptr_t p6){

			((*ptr).*m_func) (p1,p2,p3,p4,p5,p6);
		}

		virtual ~CFunctionMemberPointerVoidParam6Caller(){}
	};

	//-------------------------------------------------------------------------------------------------
	// BYTE_CODE_RET PARAM0

	class CFunctionMemberPointerRetParam0:public CFunctionMemberPointer{
		public:
			virtual intptr_t operator()()=0;
			virtual bool ret_bool()=0;
			virtual float ret_float()=0;

			virtual ~CFunctionMemberPointerRetParam0(){}
	};

	template <class _T,class _S>
	class CFunctionMemberPointerRetParam0Caller: public CFunctionMemberPointerRetParam0{

		intptr_t (_S::* m_func) ( );
		_T *ptr;
	public:
		CFunctionMemberPointerRetParam0Caller(_T *obj,intptr_t (_S::* _func) ())
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

		virtual ~CFunctionMemberPointerRetParam0Caller(){}
	};

	//-------------------------------------------------------------------------------------------------
	// VOID/BYTE_CODE_RET PARAM1
	class CFunctionMemberPointerRetParam1:public CFunctionMemberPointer{
		public:
			virtual intptr_t operator()(intptr_t p1)=0;
			virtual bool ret_bool(intptr_t p1)=0;
			virtual float ret_float(intptr_t p1)=0;

			virtual ~CFunctionMemberPointerRetParam1(){}
	};

	template <class _T,class _S>
	class CFunctionMemberPointerRetParam1Caller: public CFunctionMemberPointerRetParam1{

		intptr_t (_S::* m_func) (intptr_t p1);
		_T *ptr;
	public:
		CFunctionMemberPointerRetParam1Caller(_T *obj,intptr_t (_S::* _func) ( intptr_t p1))
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


		virtual ~CFunctionMemberPointerRetParam1Caller(){}
	};
	//-------------------------------------------------------------------------------------------------
	// VOID/BYTE_CODE_RET PARAM2
	class CFunctionMemberPointerRetParam2:public CFunctionMemberPointer{
		public:
			virtual intptr_t operator()(intptr_t p1, intptr_t p2)=0;
			virtual bool ret_bool(intptr_t p1, intptr_t p2)=0;
			virtual float ret_float(intptr_t p1, intptr_t p2)=0;



			virtual ~CFunctionMemberPointerRetParam2(){}
	};

	template <class _T,class _S>
	class CFunctionMemberPointerRetParam2Caller: public CFunctionMemberPointerRetParam2{

		intptr_t (_S::* m_func) (intptr_t p1,intptr_t p2);
		_T *ptr;
	public:
		CFunctionMemberPointerRetParam2Caller(_T *obj,intptr_t (_S::* _func) ( intptr_t p1,intptr_t p2))
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

		virtual ~CFunctionMemberPointerRetParam2Caller(){}
	};

	//-------------------------------------------------------------------------------------------------
	// VOID/BYTE_CODE_RET PARAM3
	class CFunctionMemberPointerRetParam3:public CFunctionMemberPointer{
		public:
			virtual intptr_t operator()(intptr_t p1, intptr_t p2, intptr_t p3)=0;
			virtual bool ret_bool(intptr_t p1, intptr_t p2, intptr_t p3)=0;
			virtual float ret_float(intptr_t p1, intptr_t p2, intptr_t p3)=0;


			virtual ~CFunctionMemberPointerRetParam3(){}
	};

	template <class _T,class _S>
	class CFunctionMemberPointerRetParam3Caller: public CFunctionMemberPointerRetParam3{

		intptr_t (_S::* m_func) (intptr_t p1,intptr_t p2,intptr_t p3);

		_T *ptr;
	public:
		CFunctionMemberPointerRetParam3Caller(_T *obj,intptr_t (_S::* _func) ( intptr_t p1,intptr_t p2,intptr_t p3))
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

		virtual ~CFunctionMemberPointerRetParam3Caller(){}
	};
	//-------------------------------------------------------------------------------------------------
	// VOID/BYTE_CODE_RET PARAM4
	class CFunctionMemberPointerRetParam4:public CFunctionMemberPointer{
		public:
			virtual intptr_t operator()(intptr_t p1, intptr_t p2, intptr_t p3,intptr_t p4)=0;
			virtual bool ret_bool(intptr_t p1, intptr_t p2, intptr_t p3,intptr_t p4)=0;
			virtual float ret_float(intptr_t p1, intptr_t p2, intptr_t p3,intptr_t p4)=0;


			virtual ~CFunctionMemberPointerRetParam4(){}
	};

	template <class _T,class _S>
	class CFunctionMemberPointerRetParam4Caller: public CFunctionMemberPointerRetParam4{

		intptr_t (_S::* m_func) (intptr_t p1,intptr_t p2,intptr_t p3,intptr_t p4);
		_T *ptr;
	public:
		CFunctionMemberPointerRetParam4Caller(_T *obj,intptr_t (_S::* _func) ( intptr_t p1,intptr_t p2,intptr_t p3,intptr_t p4))
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

		virtual ~CFunctionMemberPointerRetParam4Caller(){}
	};
	//-------------------------------------------------------------------------------------------------
	// VOID/BYTE_CODE_RET PARAM5
	class CFunctionMemberPointerRetParam5:public CFunctionMemberPointer{
		public:
			virtual intptr_t operator()(intptr_t p1, intptr_t p2, intptr_t p3,intptr_t p4,intptr_t p5)=0;
			virtual bool ret_bool(intptr_t p1, intptr_t p2, intptr_t p3,intptr_t p4,intptr_t p5)=0;
			virtual float ret_float(intptr_t p1, intptr_t p2, intptr_t p3,intptr_t p4,intptr_t p5)=0;


			virtual ~CFunctionMemberPointerRetParam5(){}
	};

	template <class _T,class _S>
	class CFunctionMemberPointerRetParam5Caller: public CFunctionMemberPointerRetParam5{

		intptr_t (_S::* m_func) (intptr_t p1,intptr_t p2,intptr_t p3,intptr_t p4,intptr_t p5);
		_T *ptr;
	public:
		CFunctionMemberPointerRetParam5Caller(_T *obj,intptr_t (_S::* _func) ( intptr_t p1,intptr_t p2,intptr_t p3,intptr_t p4,intptr_t p5))
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

		virtual ~CFunctionMemberPointerRetParam5Caller(){}
	};
	//-------------------------------------------------------------------------------------------------
	// VOID/BYTE_CODE_RET PARAM6
	class CFunctionMemberPointerRetParam6:public CFunctionMemberPointer{
		public:
			virtual intptr_t operator()(intptr_t p1, intptr_t p2, intptr_t p3,intptr_t p4,intptr_t p5,intptr_t p6)=0;
			virtual bool ret_bool(intptr_t p1, intptr_t p2, intptr_t p3,intptr_t p4,intptr_t p5,intptr_t p6)=0;
			virtual float ret_float(intptr_t p1, intptr_t p2, intptr_t p3,intptr_t p4,intptr_t p5,intptr_t p6)=0;


			virtual ~CFunctionMemberPointerRetParam6(){}
	};

	template <class _T,class _S>
	class CFunctionMemberPointerRetParam6Caller: public CFunctionMemberPointerRetParam6{

		intptr_t (_S::* m_func) (intptr_t p1,intptr_t p2,intptr_t p3,intptr_t p4,intptr_t p5,intptr_t p6);
		_T *ptr;
	public:
		CFunctionMemberPointerRetParam6Caller(_T *obj,intptr_t (_S::* _func) ( intptr_t p1,intptr_t p2,intptr_t p3,intptr_t p4,intptr_t p5,intptr_t p6))
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

		virtual ~CFunctionMemberPointerRetParam6Caller(){}
	};

}