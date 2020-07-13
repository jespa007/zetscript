/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
#pragma once

#define IDX_MAIN_FUNCTION				0
#define NEW_SCRIPT_FUNCTION(data)		((data->script_function_factory))->newScriptFunction
#define GET_SCRIPT_FUNCTION(data,idx)	((data->script_function_factory))->getScriptFunction(idx)
#define MAIN_FUNCTION(data)				GET_SCRIPT_FUNCTION(data,IDX_MAIN_FUNCTION)

namespace zetscript{
	class ScopeFactory;
	class ScriptFunctionFactory{

		public:

			ScriptFunctionFactory(ZetScript *_zs);

			/**
			 * Set/Get ScriptClass Node by its idx, regarding current state.
			 */
			//static void 								setVectorScriptFunctionObjectNode(std::vector<ScriptFunction *> 	* set_vec);
			std::vector<ScriptFunction *> 	*	getScriptFunctions();

			ScriptFunction 			*	newScriptFunction(
				const std::string & file
				, short line
				, unsigned  char idx_class
				, short idx_scope
				, short idx_symbol
				, const std::string & function_name
				, std::vector<ParamArgInfo> args={}
				, int idx_return_type=ZS_IDX_UNDEFINED
				, intptr_t ref_ptr=0
				, unsigned short properties=0
			);

			 bool							checkCanregister_C_Function(const std::string & function_name);

			 ScriptFunction 			* 	getScriptFunction(int idx);

			void clear();

			~ScriptFunctionFactory();

		private:
			ZetScript 						* 	zs;
			ScopeFactory 					* 	scope_factory;
			std::vector<ScriptFunction *> 		script_functions;
	};

};
