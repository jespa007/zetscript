/*
 *  This file is distributed under the MIT License.

 *  See LICENSE file for details.
 */
#include "zetscript.h"

namespace zetscript{

/*	typedef struct{
		int index;
		int alignment;
		const char *format_string;
	}FormatItem;*/

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
		std::string str_input=zs_strutils::unescape(str->toString());
		ScriptObjectVector *sov=NULL;
		std::string str_num_aux;
		zs_int *ptr_idx_num=NULL;

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
			// Tokenize all formatting items...
			char *str_begin=(char *)str_input.c_str();

			while((str_begin=strchr(str_begin,'{'))!=NULL){

				str_begin=str_begin+1; // ignore '{'
				char *str_end=strchr(str_begin,'}');

				if(str_end != NULL){ // analize...

					str_num_aux="";
					char *str_begin_alignment=strchr(str_begin,',');
					char *str_begin_format_string=strchr(str_begin,':');
					char *str_end_index=NULL;
					char *str_end_aligment=NULL;
					int idx_num=-1;
					int alignmen=-1;
					char *format_string=NULL;

					if(str_begin_alignment == NULL && str_begin_format_string==NULL){ // no aligment/no format string
						str_end_index=str_end;
					}else if(str_begin_alignment!=NULL && str_begin_format_string == NULL){ // aligment/no format string
						str_end_index=str_begin_alignment;
						str_end_aligment=str_end;
					}else if(str_begin_alignment==NULL && str_begin_format_string != NULL){ // no aligment/ format string
						str_end_index=str_begin_format_string;
					}else if(str_begin_alignment < str_begin_format_string){ // aligment/ format string
						str_end_index=str_begin_alignment;
						str_end_aligment=str_begin_format_string;
					}


					if(str_end_index!=NULL){ // index was found

						// try to convert str to index...
						for(;str_begin<str_end_index;){
							str_num_aux+=*str_begin++;
						}

						if((ptr_idx_num=zs_strutils::parse_int(str_num_aux))!=NULL){
							idx_num=*ptr_idx_num;
							delete ptr_idx_num;
							ptr_idx_num=NULL;

							// get alignment ...
							if(str_begin_alignment != NULL && str_end_aligment !=NULL){
								str_num_aux="";
								str_begin_alignment=str_begin_alignment+1;
								// try to convert str to index...
								for(;str_begin_alignment<str_end_aligment;){
									str_num_aux+=*str_begin_alignment++;
								}

								if((ptr_idx_num=zs_strutils::parse_int(str_num_aux))!=NULL){
									alignmen=*ptr_idx_num;
									delete ptr_idx_num;
									ptr_idx_num=NULL;
								}
							}

							// finally get the format ...
							if(str_begin_format_string != NULL){
								str_begin_format_string=str_begin_format_string+1;
								size_t format_len=str_end-str_begin_format_string;
								if(format_len>0){
									format_string=(char *)malloc(format_len*sizeof(char)+1);
									memset(format_string,0,format_len*sizeof(char)+1);
									strncpy(format_string,str_begin_format_string,format_len);
								}
							}


							if(idx_num >=0 ){

								/*FormatItem *item=(FormatItem *)malloc(sizeof(FormatItem));
								item->index=idx_num;*/
								printf("idx=>[%i] aligment=>[%i] format=>[%s]\n",idx_num,alignmen,format_string==NULL?"":format_string);

								if(format_string != NULL){
									free(format_string);
								}
							}
						}
					}
					str_begin = str_end+1;
				}
			}

			/*for(unsigned i=0; i < sov->length(); i++){

				//first_param=zs_strutils::replace(first_param,zs_strutils::format("{%i}",i),sov->getUserElementAt(i)->toString());
			}*/
		}
		//ScriptObjectString *str_in=(ScriptObjectString *)(str->var_ref);
		ScriptObjectString *str_out=ZS_NEW_OBJECT_STRING(zs);
		str_out->set(str_input);//str_in->default_str_value;
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
