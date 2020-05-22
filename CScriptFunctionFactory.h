#pragma once





#define IDX_MAIN_FUNCTION										0



namespace zetscript{


	class CScriptFunctionFactory{


		public:


			CScriptFunctionFactory(CZetScript *_zs);

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


			 bool							checkCanRegister_C_Function(const std::string & function_name);

			 CScriptFunction 			* 	getScriptFunction(int idx);

			void clear();

			~CScriptFunctionFactory();

		private:

			CZetScript * zs;
			CScopeFactory * scope_factory;
			std::vector<CScriptFunction *> 	vec_script_function_node;



	};

};
