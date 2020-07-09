#pragma once



#define IDX_MAIN_FUNCTION										0



namespace zetscript{


	class ScriptFunctionFactory{

		public:

			ScriptFunctionFactory(ZetScript *_zs);

			/**
			 * Set/Get ScriptClass Node by its idx, regarding current state.
			 */
			//static void 								setVectorScriptFunctionObjectNode(std::vector<ScriptFunction *> 	* set_vec);
			std::vector<ScriptFunction *> 	*	GetScriptFunctions();

			ScriptFunction 			*	newScriptFunction(			const std::string & file
																								, short line
																								, unsigned  char idx_class
																								, short idx_scope
																								, const std::string & function_name
																								, std::vector<ParamArgInfo> args={}
																								, int idx_return_type=ZS_UNDEFINED_IDX
																								, intptr_t ref_ptr=0
																								, unsigned short properties=0);


			 bool							CheckCanRegister_C_Function(const std::string & function_name);

			 ScriptFunction 			* 	GetScriptFunction(int idx);

			void Clear();

			~ScriptFunctionFactory();

		private:

			ZetScript * zs;
			ScopeFactory * scope_factory;
			std::vector<ScriptFunction *> 	script_functions;

	};

};
