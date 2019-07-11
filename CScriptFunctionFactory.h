#pragma once



#define SCRIPT_FUNCTION_FACTORY									zetscript::CScriptFunctionFactory::getInstance()

#define NEW_SCRIPT_FUNCTION										zetscript::CScriptFunctionFactory::getInstance()->newScriptFunction
#define GET_SCRIPT_FUNCTION(idx) 								zetscript::CScriptFunctionFactory::getInstance()->getScriptFunction(idx)
#define IDX_MAIN_FUNCTION										0
#define MAIN_FUNCTION											GET_SCRIPT_FUNCTION(IDX_MAIN_FUNCTION)


namespace zetscript{


	class CScriptFunctionFactory{


		public:

			static CScriptFunctionFactory * getInstance();
			static void destroySingleton();

			/**
			 * Set/Get CScriptClass Node by its idx, regarding current state.
			 */
			//static void 								setVectorScriptFunctionObjectNode(std::vector<CScriptFunction *> 	* set_vec);
			std::vector<CScriptFunction *> 	*	getVectorScriptFunctionNode();

			 CScriptFunction 			*	newScriptFunction(			const std::string & file
																								, short line
																								, unsigned  char idxClass
																								, short idxScope
																								, const std::string & function_name
																								, std::vector<tArgumentInfo> args={}
																								, int idx_return_type=ZS_UNDEFINED_IDX
																								, intptr_t ref_ptr=0
																								, unsigned short properties=0);


			 bool							checkCanRegister_C_Function(const char *f);

			 CScriptFunction 			* 	getScriptFunction(int idx);

			void clear();

		private:

			static CScriptFunctionFactory *script_function_factory_singleton;

			std::vector<CScriptFunction *> 	vec_script_function_node;

			CScriptFunctionFactory();
			~CScriptFunctionFactory();

	};

};
