/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
#include "zetscript.h"

namespace zetscript{


	void  write_error(const char *filename, int line, const  char  *string_text, ...);

	bool CScriptClass::isToClass(){

		 return ((symbol_info.symbol_info_properties & SYMBOL_INFO_PROPERTY_C_OBJECT_REF) != 0);
	}
	//------------------------------------------------------------

	 CScriptClass::CScriptClass(CZetScript *_zs,unsigned char _idxClass):CScriptClassBase(_zs,_idxClass){

			str_class_ptr_type="";
			c_destructor = NULL;
			c_constructor=NULL;
			idx_function_member_constructor =ZS_UNDEFINED_IDX;
	}

	CScriptFunction * CScriptClass::registerMemberFunction(const std::string & file, short line, const std::string & function_name, std::vector<ParamArgInfo> args, int idx_return_type,intptr_t ref_ptr, unsigned short symbol_info_properties){

		CScriptFunction * sf = registerFunction(file, line, function_name,  args,  idx_return_type, ref_ptr,  symbol_info_properties);
		std::string class_name=symbol_info.symbol->name;

		if(function_name == class_name){
			idx_function_member_constructor = (char)local_function.size();
		}


		// check if metamethod...
		for(int i = 0; i < MAX_METAMETHOD_OPERATORS; i++){
			if(ZS_STRCMP(CByteCode::metamethodOpCodeToStr((METAMETHOD_OP_CODE)i),==,function_name.c_str())){

				metamethod_operator[i].push_back(sf);

				zs_print_debug_cr("Registered metamethod %s::%s",class_name.c_str(), function_name.c_str());
				break;
			}
		}
		return sf;

	}
	//-----

	CScriptClass::~CScriptClass(){

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



