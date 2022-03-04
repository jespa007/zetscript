/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
#pragma once

#define MAIN_SCRIPT_FUNCTION_NAME 		"@MainFunction"
#define CONSTRUCTOR_FUNCTION_NAME 		"constructor"
#define IDX_SCRIPT_FUNCTION_MAIN		0
#define NEW_SCRIPT_FUNCTION(data)		(((data)->script_function_factory))->newScriptFunction
#define GET_SCRIPT_FUNCTION(data,idx)	(((data)->script_function_factory))->getScriptFunction(idx)
#define MAIN_FUNCTION(data)				GET_SCRIPT_FUNCTION(data,IDX_SCRIPT_FUNCTION_MAIN)

namespace zetscript{
	class ScopeFactory;
	class ScriptFunctionFactory{

		public:

			ScriptFunctionFactory(ZetScript *_zs);

			/**
			 * Set/Get ScriptType Node by its idx, regarding current state.
			 */
			zs_vector 	*	getScriptFunctions();

			Symbol 			*	newScriptFunction(
				//--- Register information
				  Scope *scope
				, const char * file
				, short line
				//--- Function data
			    , int idx_script_type
				, const zs_string & name_script_function
				, ScriptFunctionParam **_params
				,int _params_len

				, int idx_script_type_return=ZS_IDX_UNDEFINED
				, zs_int ref_native_function_ptr=0
				, unsigned short properties=0
			);

			 bool							checkCanregisterNativeFunction(const zs_string & name_script_function);

			 ScriptFunction 			* 	getScriptFunction(int idx);
			 void							setScriptFunction(int idx, ScriptFunction *sf);

			void clear(int _idx_start = ZS_IDX_UNDEFINED);
			void saveState();

			~ScriptFunctionFactory();

		private:
			ZetScript 						* 	zs;
			ScopeFactory 					* 	scope_factory;
			zs_vector 						*   script_functions;
			int idx_clear_checkpoint;
	};

}
