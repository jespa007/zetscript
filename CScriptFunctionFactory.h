#pragma once





#define IDX_MAIN_FUNCTION										0



namespace zetscript{


	class CScriptFunctionFactory{


		public:


			CScriptFunctionFactory();

			/**
			 * Set/Get CScriptClass Node by its idx, regarding current state.
			 */
			//static void 								setVectorScriptFunctionObjectNode(std::vector<CScriptFunction *> 	* set_vec);
			std::vector<CScriptFunction *> 	*	getVectorScriptFunctionNode();

			 CScriptFunction 			*	newScriptFunction(			const std::string & file
																								, short line
																								, unsigned  char idx_class
																								, short idxScope
																								, const std::string & function_name
																								, std::vector<ParamArgInfo> args={}
																								, int idx_return_type=ZS_UNDEFINED_IDX
																								, intptr_t ref_ptr=0
																								, unsigned short properties=0);


			 bool							checkCanRegister_C_Function(const char *f);

			 CScriptFunction 			* 	getScriptFunction(int idx);

			void clear();

			~CScriptFunctionFactory();

		private:

			static CScriptFunctionFactory *script_function_factory_singleton;

			std::vector<CScriptFunction *> 	vec_script_function_node;



	};

};
