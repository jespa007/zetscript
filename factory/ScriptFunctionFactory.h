#pragma once



#define IDX_MAIN_FUNCTION										0



namespace ZetScript{


	class ScriptFunctionFactory{


		public:


			ScriptFunctionFactory(ZetScript *_zs);

			/**
			 * Set/Get ScriptClass Node by its idx, regarding current state.
			 */
			//static void 								setVectorScriptFunctionObjectNode(std::vector<ScriptFunction *> 	* set_vec);
			std::vector<ScriptFunction *> 	*	getVectorScriptFunctionNode();

			 ScriptFunction 			*	newScriptFunction(			const std::string & file
																								, short line
																								, unsigned  char idx_class
																								, short idxScope
																								, const std::string & function_name
																								, std::vector<ParamArgInfo> args={}
																								, int idx_return_type=ZS_UNDEFINED_IDX
																								, intptr_t ref_ptr=0
																								, unsigned short properties=0);


			 bool							checkCanRegister_C_Function(const std::string & function_name);

			 ScriptFunction 			* 	getScriptFunction(int idx);

			void clear();

			~ScriptFunctionFactory();

		private:

			ZetScript * zs;
			ScopeFactory * scope_factory;
			std::vector<ScriptFunction *> 	vec_script_function_node;



	};

};
