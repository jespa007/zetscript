/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
#pragma once

namespace zetscript{

/**
 * Stores the basic information to build a object through built AST structure
 */

	class  ScriptClass:public ScriptClassBase{

	public:

		//------------- VARIABLES STRUCT ---------------
		char  idx_function_member_constructor;

		std::function<void * ()>			* 	c_constructor;
		std::function<void (void *  p)> 	*	c_destructor;
		std::string 							str_class_ptr_type; // type_id().name();
		std::vector<unsigned char > 			idx_base_class;

		std::vector<ScriptFunction * > 			metamethod_operator[BYTE_CODE_METAMETHOD_MAX]; // overrided metamethod

		 ScriptClass(ZetScript *_zs,unsigned char _idx_class);

		virtual ScriptFunction				* 	registerFunctionMember(const std::string & file, short line,const std::string & function_name, std::vector<ParamArgInfo> args={}, int idx_return_type=ZS_UNDEFINED_IDX,intptr_t ref_ptr=0, unsigned short symbol_info_properties=0);

		bool isToClass();

		virtual ~ScriptClass();
	};
}


