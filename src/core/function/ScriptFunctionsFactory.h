/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
#pragma once

#define ZS_MAIN_FUNCTION_NAME 		"@MainFunction"
#define ZS_CONSTRUCTOR_FUNCTION_NAME 		"constructor"
#define ZS_IDX_FUNCTION_MAIN			0
#define ZS_NEW_FUNCTION(data)		(((data)->script_function_factory))->newFunction
#define ZS_GET_FUNCTION(data,idx)	(((data)->script_function_factory))->getFunction(idx)
#define ZS_MAIN_FUNCTION(data)				ZS_GET_FUNCTION(data,ZS_IDX_FUNCTION_MAIN)

namespace zetscript{
	class ScriptScopesFactory;
	class ScriptFunctionsFactory{

		public:

			ScriptFunctionsFactory(ScriptEngine *_se);

			/**
			 * Set/Get ScriptType Node by its idx, regarding current state.
			 */
			Vector<ScriptFunction *> 	*	getFunctions();

			Symbol 			*	newFunction(
				//--- Register information
				  ScriptScope *scope
				, const char * file
				, short line
				//--- ScriptFunction data
			    , ScriptTypeId _script_type_id
				, const String & name
				, FunctionParam **_params
				,int _params_len

				, ScriptTypeId _return_script_type_id=SCRIPT_TYPE_ID_INVALID
				, zs_int ref_native_function_ptr=0
				, unsigned short properties=0
			);

			 ScriptFunction 			* 	getFunction(int idx);
			 void							setFunction(int idx, ScriptFunction *sf);

			void clear(int _idx_start = ZS_UNDEFINED_IDX);
			void saveState();

			~ScriptFunctionsFactory();

		private:
			ScriptEngine 						* 	zs;
			ScriptScopesFactory 					* 	scope_factory;
			Vector<ScriptFunction *>		*   script_functions;
			int idx_clear_checkpoint;
	};

}
