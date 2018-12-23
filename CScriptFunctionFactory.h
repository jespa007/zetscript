#pragma once




namespace zetscript{

	#define SCRIPT_FUNCTION_FACTORY									zetscript::CScriptFunctionFactory::getInstance()

	#define NEW_SCRIPT_FUNCTION										CScriptFunctionFactory::getInstance()->newScriptFunction
	#define GET_SCRIPT_FUNCTION(idx) 								CScriptFunctionFactory::getInstance()->getScriptFunction(idx)
	#define IDX_MAIN_FUNCTION										0
	#define MAIN_FUNCTION											GET_SCRIPT_FUNCTION(IDX_MAIN_FUNCTION)


	class CScriptFunctionFactory{


		public:

			static CScriptFunctionFactory * getInstance();
			static void destroySingleton();

			/**
			 * Set/Get CScriptClass Node by its idx, regarding current state.
			 */
			//static void 								setVectorScriptFunctionObjectNode(vector<CScriptFunction *> 	* set_vec);
			vector<CScriptFunction *> 	*	getVectorScriptFunctionNode();

			ZETSCRIPT_MODULE_EXPORT CScriptFunction 			*	newScriptFunction(unsigned  char idxClass
																								, short idxScope
																								, const string & function_name
																								, vector<tArgumentInfo> args={}
																								, int idx_return_type=ZS_UNDEFINED_IDX
																								, intptr_t ref_ptr=0
																								, unsigned short properties=0);


			ZETSCRIPT_MODULE_EXPORT bool							checkCanRegister_C_Function(const char *f);

			ZETSCRIPT_MODULE_EXPORT CScriptFunction 			* 	getScriptFunction(int idx);

			void clear();

		private:

			static CScriptFunctionFactory *script_function_factory_singleton;

			vector<CScriptFunction *> 	vec_script_function_node;

			CScriptFunctionFactory();
			~CScriptFunctionFactory();

	};

};
