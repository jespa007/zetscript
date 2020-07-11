/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
#include "zetscript.h"

namespace zetscript{


	void  writeError(const char *filename, int line, const  char  *string_text, ...);

	bool ScriptClass::isToClass(){

		 return ((symbol_info.symbol_info_properties & SYMBOL_INFO_PROPERTY_C_OBJECT_REF) != 0);
	}
	//------------------------------------------------------------

	 ScriptClass::ScriptClass(ZetScript *_zs,unsigned char _idxClass):ScriptClassBase(_zs,_idxClass){

			str_class_ptr_type="";
			c_destructor = NULL;
			c_constructor=NULL;
			idx_function_member_constructor =ZS_UNDEFINED_IDX;
	}

	ScriptFunction * ScriptClass::registerFunctionMember(const std::string & file, short line, const std::string & function_name, std::vector<ParamArgInfo> args, int idx_return_type,intptr_t ref_ptr, unsigned short symbol_info_properties){

		ScriptFunction * sf = registerFunction(file, line, function_name,  args,  idx_return_type, ref_ptr,  symbol_info_properties);
		std::string class_name=symbol_info.symbol->name;

		if(function_name == class_name){
			idx_function_member_constructor = (char)local_function.size();
		}


		// check if metamethod...
		for(int i = 0; i < BYTE_CODE_METAMETHOD_MAX; i++){
			if(ZS_STRCMP(ByteCodeMetamethodToStr((ByteCodeMetamethod)i),==,function_name.c_str())){

				metamethod_operator[i].push_back(sf);

				ZS_PRINT_DEBUG("Registered metamethod %s::%s",class_name.c_str(), function_name.c_str());
				break;
			}
		}
		return sf;

	}
	//-----

	ScriptClass::~ScriptClass(){

		if ((symbol_info.symbol_info_properties & SYMBOL_INFO_PROPERTY_C_OBJECT_REF) == SYMBOL_INFO_PROPERTY_C_OBJECT_REF) {

			if (c_constructor !=NULL) {
				delete c_constructor;
				c_constructor=NULL;
			}

			if (c_destructor != NULL) {
				delete c_destructor;
				c_destructor=NULL;
			}

		}
	}
}



