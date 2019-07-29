#pragma once


namespace zetscript{

	class CScriptVariable;
	class CScriptClass;
	class CZetScript;
	class CScopeFactory;
	class CScriptClassFactory{

	public:

		CScriptClassFactory(CZetScript *_zs);


		 /**
		  * Class management region
		  */

			/// register script class
			CScriptClass 						* 				registerClass(const std::string & file, short line, const std::string & class_name, const std::string & base_class_name="");

		 CScriptClass * 				getScriptClass(unsigned char idx);
		 CScriptClass * 				getScriptClass(const std::string & name);
		CScriptClass 						* 				getScriptClassBy_C_ClassPtr(const std::string & class_type);
		const char 							* 				getScriptClassName(unsigned char idx);


		bool						isIdxClassInstanceOf(unsigned char  theClass,unsigned char  class_idx);
		unsigned char				getIdxClassFromIts_C_Type(const std::string & s);
		unsigned char getIdx_C_RegisteredClass(const std::string & str_classPtr);

		 std::vector<CScriptClass *> * 	getVectorScriptClassNode();



		bool 						isClassRegistered(const std::string & v);



		intptr_t 											doCast(intptr_t obj, unsigned char src_class, unsigned char convert_class);

		void clear();

		/**
		 * Class name given this function creates the object and initializes all variables.
		 */
		CScriptVariable 		 				* 			instanceScriptVariableByClassName(const std::string & class_name);
		CScriptVariable 		 				* 			instanceScriptVariableByIdx(unsigned char  idx_class, void * value_object = NULL);

		~CScriptClassFactory();

	private:

		typedef struct{
			const char *   type_str;
			BUILT_IN_TYPE  id;
		}PrimitiveType;

		typedef struct{
			PrimitiveType 				*return_type;
			std::vector<PrimitiveType*>		params;
		}tRegisterFunction;

		std::vector<CScriptClass *> 			 		vec_script_class_node;
		CZetScript *zs;
		CScopeFactory *scope_factory;



			PrimitiveType *getPrimitiveTypeFromStr(const std::string & str);
			std::map<unsigned char, std::map<unsigned char, fntConversionType>> *  getMapTypeConversion();








			void 												registerPrimitiveTypes();





			unsigned char							getIdxScriptClass_Internal(const std::string & class_name);
			unsigned char							getIdxClassFromIts_C_TypeInternal(const std::string & c_type_str);

			void setup();

			void internal_print_error(const char *s);



			std::map<unsigned char,std::map<unsigned char,fntConversionType>> 	mapTypeConversion;
	};

}
