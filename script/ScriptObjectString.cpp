/*
 *  This file is distributed under the MIT License.

 *  See LICENSE file for details.
 */
#include "zetscript.h"

namespace zetscript{

	ScriptObjectString * ScriptObjectString::newScriptObjectString(ZetScript *zs, const std::string & str){
		ScriptObjectString *so=new ScriptObjectString();
		so->setZetScript(zs);
		so->set(str);
		return so;
	}


	ScriptObjectString *ScriptObjectString::newScriptObjectStringAddStk(ZetScript *zs,StackElement *stk_result_op1,StackElement *stk_result_op2){
		ScriptObjectString *so_ref=NULL;
		// we have to create an new string variable
		if(STK_IS_SCRIPT_OBJECT_STRING(stk_result_op1)){
			so_ref=(ScriptObjectString *)stk_result_op1->stk_value;
		}

		if(so_ref == NULL){
		   if(STK_IS_SCRIPT_OBJECT_STRING(stk_result_op2)){
			   so_ref=(ScriptObjectString *)stk_result_op2->stk_value;
		   }else{
			   THROW_RUNTIME_ERROR("Expected one of both operants as string var");
		   }
		}


		//std::string *str;
		ScriptObjectString *so_string = ZS_NEW_OBJECT_STRING(zs);



		std::string str1;
		std::string str2;

		std::string * str_dst[]={
			   &str1,
			   &str2
		};

		StackElement * stk_src[]={
			   stk_result_op1,
			   stk_result_op2
		};

		std::string  ** str_dst_it=str_dst;
		StackElement ** stk_src_it=stk_src;

		// str1
		for(unsigned i=0; i < 2; i++){
			StackElement *stk_src_item=(*stk_src_it);
			if(stk_src_item->properties & MSK_STK_PROPERTY_PTR_STK){
				stk_src_item=(StackElement *)stk_src_item->stk_value;
			}

			switch(GET_STK_PROPERTY_PRIMITIVE_TYPES(stk_src_item->properties)){
			//case MSK_STK_PROPERTY_UNDEFINED:
			//	*(*str_dst_it)="undefined";
			//	break;
			case MSK_STK_PROPERTY_ZS_INT:
				*(*str_dst_it)=zs_strutils::zs_int_to_str((zs_int)(stk_src_item)->stk_value);
				break;
			case MSK_STK_PROPERTY_ZS_FLOAT:
				*(*str_dst_it)=zs_strutils::float_to_str(*((zs_float *)&((stk_src_item)->stk_value)));
				break;
			case MSK_STK_PROPERTY_BOOL:
				*(*str_dst_it)=(stk_src_item)->stk_value == 0?"false":"true";
				break;
			case MSK_STK_PROPERTY_FUNCTION:
				*(*str_dst_it)="function";
				break;
			case MSK_STK_PROPERTY_CLASS:
				*(*str_dst_it)="class";
				break;
			default:
				if(stk_src_item->properties==MSK_STK_PROPERTY_UNDEFINED){
					*(*str_dst_it)="undefined";
				}else if(stk_src_item->properties & MSK_STK_PROPERTY_SCRIPT_OBJECT){
					*(*str_dst_it)=((ScriptObjectObject *)(stk_src_item)->stk_value)->toString();
				}
				else{
					*(*str_dst_it)="unknow";
				}

				break;
			}

			str_dst_it++;
			stk_src_it++;
		}

		// save result
		so_string->set(str1+str2);

		return so_string;
	}

	ScriptObjectString::ScriptObjectString(){
		idx_script_class=IDX_BUILTIN_TYPE_SCRIPT_OBJECT_STRING;
		str_value = "";
		value = &str_value;
	}

	void ScriptObjectString::set(const std::string & s){
		*((std::string *)value) = s;
	}

	std::string ScriptObjectString::toString(){
		return *((std::string *)value);
	}

	size_t ScriptObjectString::length(){
		return ((std::string *)value)->size();
	}

	ScriptObjectString *ScriptObjectString::sub(ScriptObjectString *s1){
		//std::string *str;
		ScriptObjectString *so_string = ZS_NEW_OBJECT_STRING(this->zs);
		so_string->set(zs_strutils::replace(this->toString(),s1->toString(),""));
		return so_string;
	}

}
