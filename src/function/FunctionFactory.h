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
	class ScopeFactory;
	class FunctionFactory{

		public:

			FunctionFactory(ZetScript *_zs);

			/**
			 * Set/Get Type Node by its idx, regarding current state.
			 */
			Vector<Function *> 	*	getFunctions();

			Symbol 			*	newFunction(
				//--- Register information
				  Scope *scope
				, const char * file
				, short line
				//--- Function data
			    , TypeId _type_id
				, const String & name
				, FunctionParam **_params
				,int _params_len

				, TypeId _return_type_id=TYPE_ID_INVALID
				, zs_int ref_native_function_ptr=0
				, unsigned short properties=0
			);

			 Function 			* 	getFunction(int idx);
			 void							setFunction(int idx, Function *sf);

			void clear(int _idx_start = ZS_UNDEFINED_IDX);
			void saveState();

			~FunctionFactory();

		private:
			ZetScript 						* 	zs;
			ScopeFactory 					* 	scope_factory;
			Vector<Function *>		*   script_functions;
			int idx_clear_checkpoint;
	};

}
