/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
#pragma once

#define ZS_MAIN_SCRIPT_FUNCTION_NAME 		"@MainFunction"
#define ZS_CONSTRUCTOR_FUNCTION_NAME 		"constructor"
#define ZS_IDX_SCRIPT_FUNCTION_MAIN			0
#define ZS_NEW_SCRIPT_FUNCTION(data)		(((data)->script_function_factory))->newScriptFunction
#define ZS_GET_SCRIPT_FUNCTION(data,idx)	(((data)->script_function_factory))->getScriptFunction(idx)
#define ZS_MAIN_FUNCTION(data)				ZS_GET_SCRIPT_FUNCTION(data,ZS_IDX_SCRIPT_FUNCTION_MAIN)

namespace zetscript{
	class ScopeFactory;
	class ScriptFunctionFactory{

		public:

			ScriptFunctionFactory(ZetScript *_zs);

			/**
			 * Set/Get ScriptType Node by its idx, regarding current state.
			 */
			zs_vector<ScriptFunction *> 	*	getScriptFunctions();

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

				, int idx_script_type_return=ZS_UNDEFINED_IDX
				, zs_int ref_native_function_ptr=0
				, unsigned short properties=0
			);

			 ScriptFunction 			* 	getScriptFunction(int idx);
			 void							setScriptFunction(int idx, ScriptFunction *sf);

			void clear(int _idx_start = ZS_UNDEFINED_IDX);
			void saveState();

			~ScriptFunctionFactory();

		private:
			ZetScript 						* 	zs;
			ScopeFactory 					* 	scope_factory;
			zs_vector<ScriptFunction *>		*   script_functions;
			int idx_clear_checkpoint;
	};

}
