/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
#pragma once

#define MAIN_SCRIPT_FUNCTION_NAME 		"@MainFunction"
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
			 * Set/Get ScriptClass Node by its idx, regarding current state.
			 */
			//static void 								setVectorScriptFunctionObjectNode(std::vector<ScriptFunction *> 	* set_vec);
			zs_vector 	*	getScriptFunctions();

			Symbol 			*	newScriptFunction(
				//--- Register information
				  Scope *scope
				, const char * file
				, short line
				//--- Function data
			    , int idx_class
				, short idx_local_function
				, const std::string & function_name
				, std::vector<ScriptFunctionArg> args
				, int idx_return_type=ZS_IDX_UNDEFINED
				, zs_int ref_native_function_ptr=0
				, unsigned short properties=0
			);

			 bool							checkCanregisterNativeFunction(const std::string & function_name);

			 ScriptFunction 			* 	getScriptFunction(int idx);
			 void	setScriptFunction(short idx, ScriptFunction *sf);

			void clear(int _idx_start = ZS_IDX_UNDEFINED);
			void setClearCheckpoint();

			~ScriptFunctionFactory();

		private:
			ZetScript 						* 	zs;
			ScopeFactory 					* 	scope_factory;
			zs_vector 						*   script_functions;
			int idx_clear_checkpoint;
	};

};
