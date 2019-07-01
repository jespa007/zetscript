#pragma once

#include "CNativeFunction.h"
#include "helper/RegisterFunctionHelper.h"

#define MAX_REGISTER_CLASSES 100

#define SCRIPT_CLASS_FACTORY					zetscript::CScriptClassFactory::getInstance()

#define ZS_INVALID_CLASS						((unsigned char)ZS_UNDEFINED_IDX)

#define register_C_Function(text,s) 			zetscript::CScriptClassFactory::setFilenameLine(__FILE__, __LINE__);zetscript::CScriptClassFactory::getInstance()->register_C_FunctionInt(text,s)
#define register_C_Variable(text,s) 			zetscript::CScriptClassFactory::setFilenameLine(__FILE__, __LINE__);zetscript::CScriptClassFactory::getInstance()->register_C_VariableInt(text,&s,typeid(decltype(&s)).name())

#define register_C_Class 						zetscript::CScriptClassFactory::setFilenameLine(__FILE__, __LINE__);zetscript::CScriptClassFactory::getInstance()->register_C_ClassInt
#define register_C_SingletonClass				zetscript::CScriptClassFactory::setFilenameLine(__FILE__, __LINE__);zetscript::CScriptClassFactory::getInstance()->register_C_SingletonClassInt

#define register_C_VariableMember				zetscript::CScriptClassFactory::setFilenameLine(__FILE__, __LINE__);zetscript::CScriptClassFactory::getInstance()->register_C_VariableMemberInt
#define register_C_FunctionMember				zetscript::CScriptClassFactory::setFilenameLine(__FILE__, __LINE__);zetscript::CScriptClassFactory::getInstance()->register_C_FunctionMemberInt //<o>(s,&f)
#define class_C_baseof							zetscript::CScriptClassFactory::getInstance()->class_C_baseofInt //<o>(s,&f)


#define register_C_StaticFunctionMember			zetscript::CScriptClassFactory::setFilenameLine(__FILE__, __LINE__);zetscript::CScriptClassFactory::getInstance()->register_C_StaticFunctionMemberInt

#define NEW_CLASS_VAR_BY_IDX(idx) 				(zetscript::CScriptClassFactory::getInstance()->instanceScriptVariableByIdx(idx))

#define GET_SCRIPT_CLASS(idx) 					(zetscript::CScriptClassFactory::getInstance()->getScriptClass(idx))
#define GET_SCRIPT_CLASS_NAME(idx) 				(zetscript::CScriptClassFactory::getInstance()->getScriptClassName(idx))


#define SCRIPT_CLASS_MAIN						(zetscript::CScriptClassFactory::getInstance()->getScriptClass(IDX_CLASS_MAIN))    // 0 is the main class
#define SCRIPT_CLASS_STRING						(zetscript::CScriptClassFactory::getInstance()->getScriptClass(IDX_CLASS_STRING))
#define SCRIPT_CLASS_STRUCT						(zetscript::CScriptClassFactory::getInstance()->getScriptClass(IDX_CLASS_STRUCT))
#define SCRIPT_CLASS_VECTOR						(zetscript::CScriptClassFactory::getInstance()->getScriptClass(IDX_CLASS_VECTOR))
#define SCRIPT_CLASS_FUNCTOR					(zetscript::CScriptClassFactory::getInstance()->getScriptClass(IDX_CLASS_FUNCTOR))

#define GET_SCRIPT_CLASS_INFO_BY_C_PTR_NAME(s)	(zetscript::CScriptClassFactory::getInstance()->getScriptClassBy_C_ClassPtr(s))    // 0 is the main class

#define GET_IDX_2_CLASS_C_STR(idx) 				(CScriptClassFactory::getInstance()->getScriptClass(idx)->classPtrType)

#define register_C_BaseSymbols(o)		   		(zetscript::CScriptClassFactory::getInstance()->register_C_BaseSymbolsInt(o))

#define VOID_TYPE_STR							zetscript::CScriptClassFactory::getInstance()->m_VOID_TYPE_STR			// 	typeid(void).name()
#define INT_PTR_TYPE_STR						zetscript::CScriptClassFactory::getInstance()->m_INT_PTR_TYPE_STR			//	typeid(int *).name()
#define FLOAT_PTR_TYPE_STR						zetscript::CScriptClassFactory::getInstance()->m_FLOAT_PTR_TYPE_STR		//	typeid(float *).name()
#define STRING_PTR_TYPE_STR						zetscript::CScriptClassFactory::getInstance()->m_STRING_PTR_TYPE_STR		//	typeid(std::string *).name()
#define CONST_CHAR_PTR_TYPE_STR					zetscript::CScriptClassFactory::getInstance()->m_CONST_CHAR_PTR_TYPE_STR	//	typeid(std::string *).name()
#define BOOL_PTR_TYPE_STR						zetscript::CScriptClassFactory::getInstance()->m_BOOL_PTR_TYPE_STR		//	typeid(bool *).name()
#define INT_TYPE_STR							zetscript::CScriptClassFactory::getInstance()->m_INT_TYPE_STR				//	typeid(int).name()
#define UNSIGNED_INT_TYPE_STR					zetscript::CScriptClassFactory::getInstance()->m_UNSIGNED_INT_TYPE_STR	//	typeid(unsigned int).name()
#define INTPTR_T_TYPE_STR						zetscript::CScriptClassFactory::getInstance()->m_INTPTR_T_TYPE_STR		//	typeid(intptr_t).name()

#define FLOAT_TYPE_STR							zetscript::CScriptClassFactory::getInstance()->m_FLOAT_TYPE_STR			//	typeid(int).name()
#define BOOL_TYPE_STR							zetscript::CScriptClassFactory::getInstance()->m_BOOL_TYPE_STR			//	typeid(bool).name()
#define STACK_ELEMENT_STR						zetscript::CScriptClassFactory::getInstance()->m_STACK_ELEMENT_STR			//	typeid(bool).name()

#define DO_CAST									zetscript::CScriptClassFactory::getInstance()->doCast
#define GET_IDX_CLASS_FROM_ITS_C_TYPE			zetscript::CScriptClassFactory::getInstance()->getIdxClassFromIts_C_Type
#define INSTANCE_SCRIPT_VARIABLE_BY_IDX			zetscript::CScriptClassFactory::getInstance()->instanceScriptVariableByIdx
#define GET_METAMETHOD							zetscript::CScriptClassFactory::getInstance()->getMetamethod
#define IS_IDX_CLASS_INSTANCEOF					zetscript::CScriptClassFactory::getInstance()->isIdxClassInstanceOf

namespace zetscript{

	class CScriptClassFactory{


		public:

			typedef struct{
				const char *   type_str;
				BUILT_IN_TYPE  id;
			}tPrimitiveType;

			typedef struct{
				tPrimitiveType 				*return_type;
				std::vector<tPrimitiveType*>		params;
			}tRegisterFunction;


			static CScriptClassFactory * getInstance();
			static void destroySingleton();

			static void 				setFilenameLine(const char *file, short line);
			static tStackElement 		C_REF_InfoVariable_2_StackElement(tVariableSymbolInfo *ir_var, void *ptr_variable);

			//----

			std::string  m_VOID_TYPE_STR;			// 	typeid(void).name()
			std::string  m_INT_PTR_TYPE_STR;			//	typeid(int *).name()
			std::string  m_FLOAT_PTR_TYPE_STR;		//	typeid(float *).name()
			std::string  m_STRING_PTR_TYPE_STR;		//	typeid(std::string *).name()
			std::string  m_CONST_CHAR_PTR_TYPE_STR;	//	typeid(std::string *).name()
			std::string  m_BOOL_PTR_TYPE_STR;		//	typeid(bool *).name()
			std::string  m_INT_TYPE_STR;				//	typeid(int).name()
			std::string  m_UNSIGNED_INT_TYPE_STR;	//	typeid(unsigned int).name()
			std::string  m_INTPTR_T_TYPE_STR;		//	typeid(intptr_t).name()

			std::string  m_FLOAT_TYPE_STR;			//	typeid(int).name()
			std::string  m_BOOL_TYPE_STR;			//	typeid(bool).name()
			std::string  m_STACK_ELEMENT_STR;		//	typeid(bool).name()


			tPrimitiveType *getPrimitiveTypeFromStr(const std::string & str);
			 std::map<unsigned char, std::map<unsigned char, fntConversionType>> *  getMapTypeConversion();




			 unsigned char				getIdxClassFromIts_C_Type(const std::string & s);
			 void 						setPrintOutCallback(void (*)(const char *));
			 const char * 				getMetamethod(METAMETHOD_OPERATOR op);
			 std::vector<CScriptClass *> * 	getVectorScriptClassNode();

			/// register script class
			CScriptClass 						* 				registerClass(const std::string & file, short line, const std::string & class_name, const std::string & base_class_name="");


			 CScriptClass * 				getScriptClass(unsigned char idx);
			 CScriptClass * 				getScriptClass(const std::string & name);
			CScriptClass 						* 				getScriptClassBy_C_ClassPtr(const std::string & class_type);
			const char 							* 				getScriptClassName(unsigned char idx);


			 bool						isIdxClassInstanceOf(unsigned char  theClass,unsigned char  class_idx);
			  bool 						isClassRegistered(const std::string & v);

			void 												registerPrimitiveTypes();
			void 												register_C_BaseSymbolsInt(bool );

			void clear();

			/**
			 * Class name given this function creates the object and initializes all variables.
			 */
			CScriptVariable 		 				* 			instanceScriptVariableByClassName(const std::string & class_name);
			CScriptVariable 		 				* 			instanceScriptVariableByIdx(unsigned char  idx_class, void * value_object = NULL);

			intptr_t 											doCast(intptr_t obj, unsigned char src_class, unsigned char convert_class);




			/**
			 * REGISTER C Stuff
			 */

			/**
			 * Register C function
			 */
			template <typename F>
			static bool register_C_FunctionInt( const char * function_name,F function_ptr);

			/**
			 * Register C variable
			 */
			 tVariableSymbolInfo * register_C_VariableInt(const std::string & var_str,void * var_ptr, const std::string & var_type);
			 unsigned char getIdx_C_RegisteredClass(const std::string & str_classPtr);


			/**
			 * Register C Class. Return index registered class
			 */
			template<class _T>
			 CScriptClass * register_C_SingletonClassInt(const std::string & class_name);

			/**
			 * Register C Class. Return index registered class
			 */
			template<class _T>
			CScriptClass * register_C_ClassInt(const std::string & class_name);


			template<class _T, class _B>
			bool class_C_baseofInt();

			/**
			 * Register C Member function Class
			 */
			template < typename _C, typename _R, class _T, typename..._A>
			bool register_C_FunctionMemberInt(const char *function_name,_R (_T:: *function_type)(_A...) );

			/**
			 * Register C Member function Class
			 */
			template <typename _T, typename _F>
			bool register_C_StaticFunctionMemberInt(const char *function_name,_F function_type);


			/**
			 * Register C Member var
			 */
			template <typename _C, typename _R,typename _T>
			bool register_C_VariableMemberInt(const char *var_name, _R _T::*var_pointer);


		private:

			static CScriptClassFactory *script_class_factory;
			static const char * __registered_file__;
			static int __registered_line__;

			/**
			 * Vector of script classes. This std::vector is removed when zetscript reevaluates all scrips.
			 */
			std::vector<CScriptClass *> 			 		vec_script_class_node;


			/*
			 * register_c_base_symbols it tells to register functions/variable member already registered on base classes. Only works if class is not polymorphic (i.e there's no any virtual functions involved)
			 */
			bool register_c_base_symbols;

			std::map<unsigned char,std::map<unsigned char,fntConversionType>> 	mapTypeConversion;


			static 									void  print(const char *s);
			static 									void (* print_out_callback)(const char *);




			unsigned char							getIdxScriptClass_Internal(const std::string & class_name);
			unsigned char							getIdxClassFromIts_C_TypeInternal(const std::string & c_type_str);

			void setup();

			CScriptClassFactory();

			~CScriptClassFactory();

	};

};

#include "CScriptClassFactory.inc"
