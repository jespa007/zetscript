/*
 *  This file is distributed under the MIT License.

 *  See LICENSE file for details.
 */
#include "zetscript.h"

namespace zetscript{

	//----------------------------------------------
	//
	// Helpers
	//
	ScriptObjectString * ScriptObjectString::newScriptObjectString(ZetScript *zs, const std::string & str){
		ScriptObjectString *so=new ScriptObjectString();
		so->init(zs);
		so->set(str);
		return so;
	}

	ScriptObjectString *ScriptObjectString::newScriptObjectStringAddStk(ZetScript *zs,StackElement *stk_result_op1,StackElement *stk_result_op2){
		ScriptObjectString *so_ref=NULL;
		// we have to create an new string variable
		if(STK_IS_SCRIPT_OBJECT_STRING(stk_result_op1)){
			so_ref=(ScriptObjectString *)stk_result_op1->value;
		}

		if(so_ref == NULL){
		   if(STK_IS_SCRIPT_OBJECT_STRING(stk_result_op2)){
			   so_ref=(ScriptObjectString *)stk_result_op2->value;
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
			if(stk_src_item->properties & STK_PROPERTY_PTR_STK){
				stk_src_item=(StackElement *)stk_src_item->value;
			}

			switch(GET_STK_PROPERTY_PRIMITIVE_TYPES(stk_src_item->properties)){
			//case STK_PROPERTY_NULL:
			//	*(*str_dst_it)="null";
			//	break;
			case STK_PROPERTY_ZS_INT:
				*(*str_dst_it)=zs_strutils::zs_int_to_str((zs_int)(stk_src_item)->value);
				break;
			case STK_PROPERTY_ZS_FLOAT:
				*(*str_dst_it)=zs_strutils::float_to_str(*((zs_float *)&((stk_src_item)->value)));
				break;
			case STK_PROPERTY_BOOL:
				*(*str_dst_it)=(stk_src_item)->value == 0?"false":"true";
				break;
			case STK_PROPERTY_FUNCTION:
				*(*str_dst_it)="function";
				break;
			case STK_PROPERTY_CLASS:
				*(*str_dst_it)="class";
				break;
			default:
				if(stk_src_item->properties==STK_PROPERTY_NULL){
					*(*str_dst_it)="null";
				}else if(stk_src_item->properties & STK_PROPERTY_SCRIPT_OBJECT){
					*(*str_dst_it)=((ScriptObjectObject *)(stk_src_item)->value)->toString();
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


	ScriptObjectString * ScriptObjectString::format(ZetScript *zs, StackElement *str, StackElement *args){
		// transform '\"' to '"','\n' to carry returns, etc
		std::string first_param=zs_strutils::unescape(str->toString());
		ScriptObjectVector *sov=NULL;

		if(args->properties & STK_PROPERTY_PTR_STK){
			args=(StackElement *)args->value;
		}

		if(args->properties & STK_PROPERTY_SCRIPT_OBJECT){
			ScriptObject *so=(ScriptObject *)args->value;
			if(so->idx_script_class == IDX_BUILTIN_TYPE_SCRIPT_OBJECT_VECTOR){
				sov=(ScriptObjectVector *)so;
			}
		}

		if(sov != NULL){
			for(unsigned i=0; i < sov->length(); i++){
				first_param=zs_strutils::replace(first_param,zs_strutils::format("{%i}",i),sov->getUserElementAt(i)->toString());
			}
		}
		//ScriptObjectString *str_in=(ScriptObjectString *)(str->var_ref);
		ScriptObjectString *str_out=ZS_NEW_OBJECT_STRING(zs);
		str_out->set(first_param);//str_in->default_str_value;
		return str_out;
	}

	//
	// Helpers
	//
	//----------------------------------------------

	ScriptObjectString::ScriptObjectString(){
		idx_script_class=IDX_BUILTIN_TYPE_SCRIPT_OBJECT_STRING;
		default_str_value = "";
		value = &default_str_value;
	}

	void ScriptObjectString::set(const std::string & s){
		*((std::string *)value) = zs_strutils::unescape(s);
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
