/*
 *  This file is distributed under the MIT License.

 *  See LICENSE file for details.
 */
#include "zetscript.h"

namespace zetscript{

/*	typedef struct{
		int index;
		int padding;
		const char *format_string;
	}FormatItem;*/

	//----------------------------------------------
	//
	// Helpers
	//
	ScriptObjectString * ScriptObjectString::newScriptObjectString(ZetScript *zs, const std::string & _str){
		ScriptObjectString *so=new ScriptObjectString();
		so->init(zs);
		so->set(_str);

		return so;
	}

	ScriptObjectString *ScriptObjectString::newShareableScriptObjectString(ZetScript	*_zs){
		VirtualMachine *virtual_machine = _zs->getVirtualMachine();

		ScriptObjectString *so= new ScriptObjectString();
		so->init(_zs);
		// share this variable++
		vm_create_shared_script_object(virtual_machine,(ScriptObject *)so);
		vm_share_script_object(virtual_machine,(ScriptObject *)so);
		return so;
	}

	ScriptObjectString *ScriptObjectString::newScriptObjectStringAddStk(ZetScript *_zs,StackElement *stk_result_op1,StackElement *stk_result_op2){
		ScriptObjectString *so_ref=NULL;
		// we have to create an new string variable
		if(STK_IS_SCRIPT_OBJECT_STRING(stk_result_op1)){
			so_ref=(ScriptObjectString *)stk_result_op1->value;
		}

		if(so_ref == NULL){
		   if(STK_IS_SCRIPT_OBJECT_STRING(stk_result_op2)){
			   so_ref=(ScriptObjectString *)stk_result_op2->value;
		   }else{
			   THROW_RUNTIME_ERRORF("Expected one of both operants as string var");
		   }
		}

		//std::string *str;
		ScriptObjectString *so_string = ZS_NEW_OBJECT_STRING(_zs);

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
		for(int i=0; i < 2; i++){
			//char aux[100]={0};
			StackElement *stk_src_item=(*stk_src_it);
			if(stk_src_item->properties & STK_PROPERTY_PTR_STK){
				stk_src_item=(StackElement *)stk_src_item->value;
			}

			if(stk_src_item->properties & STK_PROPERTY_SCRIPT_OBJECT){
				*(*str_dst_it)=((ScriptObject *)stk_src_item->value)->toString();
			}else{
				*(*str_dst_it)=stk_to_str(_zs, stk_src_item);
			}

			str_dst_it++;
			stk_src_it++;
		}

		// save result
		so_string->set(str1+str2);

		return so_string;
	}


	ScriptObjectString * ScriptObjectString::format(ZetScript *zs, StackElement *stk_str_obj, StackElement *args){
		// transform '\"' to '"','\n' to carry returns, etc
		std::string str_input;

		if(stk_str_obj->properties & STK_PROPERTY_SCRIPT_OBJECT){
			str_input=zs_strutils::unescape(((ScriptObject *)stk_str_obj->value)->toString());
		}
		else{
			str_input=stk_to_str(zs, stk_str_obj);
		}

		std::string str_result;
		ScriptObjectVector *sov=NULL;
		std::string str_num_aux;
		bool error=false;
		char str_error[512]={0};


		zs_int *ptr_idx_num=NULL;


		if(args->properties & STK_PROPERTY_PTR_STK){
			args=(StackElement *)args->value;
		}

		if(args->properties & STK_PROPERTY_SCRIPT_OBJECT){
			ScriptObject *so=(ScriptObject *)args->value;
			if(so->idx_script_type == IDX_TYPE_SCRIPT_OBJECT_VECTOR){
				sov=(ScriptObjectVector *)so;
			}
		}

		if(sov == NULL){ // no params, direct result
			str_result=str_input;
		}else{
			// Tokenize all formatting items...
			char *str_start=(char *)str_input.c_str();
			char *str_it=(char *)str_input.c_str();
			char *str_begin=str_it;
			char *str_end=NULL;
			bool ok=true;

			while(ok && !error){
				ok=(str_begin=strchr(str_it,'{'))!=NULL;
				if(ok){
					ok&=(str_end=strchr(str_begin+1,'}'))!=NULL;
				}

				if(ok){
					str_result+=str_input.substr(str_it-str_start,str_begin-str_it);

					str_begin=str_begin+1; // ignore '{'

					str_num_aux="";
					char *str_begin_padding=strchr(str_begin,',');
					char *str_begin_format_string=strchr(str_begin,':');
					char *str_end_index=NULL;
					char *str_end_aligment=NULL;

					if(str_begin_padding >= str_end ) str_begin_padding=NULL;
					if(str_begin_format_string >= str_end ) str_begin_format_string=NULL;


					int idx_num=-1;
					int padding=0;
					char *ptr_str_format_string=NULL;

					if(str_begin_padding == NULL && str_begin_format_string==NULL){ // no aligment/no format string
						str_end_index=str_end;
					}else if(str_begin_padding!=NULL && str_begin_format_string == NULL){ // aligment/no format string
						str_end_index=str_begin_padding;
						str_end_aligment=str_end;
					}else if(str_begin_padding==NULL && str_begin_format_string != NULL){ // no aligment/ format string
						str_end_index=str_begin_format_string;
					}else if(str_begin_padding < str_begin_format_string){ // aligment/ format string
						str_end_index=str_begin_padding;
						str_end_aligment=str_begin_format_string;
					}


					if(str_end_index!=NULL){ // index was found
						char *str_it_idx=str_begin;
						// try to convert str to index...
						for(;str_it_idx<str_end_index;){
							str_num_aux+=*str_it_idx++;
						}

						//----------------------------------------------------------
						// INDEX ...
						if((ptr_idx_num=zs_strutils::parse_zs_int(str_num_aux))!=NULL){
							idx_num=*ptr_idx_num;
							delete ptr_idx_num;
							ptr_idx_num=NULL;

							//----------------------------------------------------------
							// ALIGNMENT ...
							if(str_begin_padding != NULL && str_end_aligment !=NULL){
								str_num_aux="";
								str_begin_padding=str_begin_padding+1;
								// try to convert str to index...
								for(;str_begin_padding<str_end_aligment;){
									str_num_aux+=*str_begin_padding++;
								}

								if((ptr_idx_num=zs_strutils::parse_zs_int(str_num_aux))!=NULL){
									padding=*ptr_idx_num;
									delete ptr_idx_num;
									ptr_idx_num=NULL;
								}
							}
							// ALIGNMENT ...
							//----------------------------------------------------------
							// FORMAT ...
							if(str_begin_format_string != NULL){
								str_begin_format_string=str_begin_format_string+1;
								size_t format_len=str_end-str_begin_format_string;
								if(format_len>0){
									ptr_str_format_string=(char *)ZS_MALLOC(format_len*sizeof(char)+1);
									strncpy(ptr_str_format_string,str_begin_format_string,format_len);
								}
							}
							// FORMAT ...
							//----------------------------------------------------------

							if(idx_num >=0 && idx_num<(int)sov->length()){ // print
								std::string str_format_results="";
								StackElement *stk_arg=sov->getUserElementAt(idx_num);

								if(stk_arg->properties & STK_PROPERTY_SCRIPT_OBJECT){
									str_format_results=((ScriptObject *)stk_arg->value)->toString();
								}else{
									str_format_results=stk_to_str(zs,stk_arg,ptr_str_format_string);
								}

								// set padding
								if(ptr_str_format_string != NULL){
									int length_padding=padding-(int)strlen(ptr_str_format_string);
									if(length_padding>0){//left padding
										for(int i=0; i < length_padding;i++){
											str_result.append(' ');
										}
									}
								}

								str_result+=str_format_results;

							}else{ // copy parameter between '{}'
								char *str_from=(str_begin-1);
								str_result+=str_input.substr(str_from-str_start,str_end-str_from+1);
								//error=true;
								//str_error="Index must be greater than or equal to zero and less than the size of the argument list.";
							}

							if(ptr_str_format_string != NULL){
								free(ptr_str_format_string);
								ptr_str_format_string=NULL;
							}
						}else{
							error=true;
							sprintf(str_error,"Cannot convert '%s' to number",str_num_aux.c_str());
							break;
						}
						// INDEX ...
						//----------------------------------------------------------
					}
					str_it = str_end+1; // ignore '}'
				}else{ // copy current position up to end
					str_end=str_start+str_input.length();
					str_result+=str_input.substr(str_it-str_start,str_end-str_it);
				}
			}
		}

		if(error){
			vm_set_error(zs->getVirtualMachine(),str_error);
			return NULL;
		}

		//ScriptObjectString *str_in=(ScriptObjectString *)(str->var_ref);
		ScriptObjectString *str_out=ZS_NEW_OBJECT_STRING(zs);
		str_out->set(str_result);//str_in->default_str_value;


		return str_out;
	}

	//
	// Helpers
	//
	//----------------------------------------------

	ScriptObjectString::ScriptObjectString(){
		idx_script_type=IDX_TYPE_SCRIPT_OBJECT_STRING;
		default_str_value = "";
		value = &default_str_value;
	}

	void ScriptObjectString::set(const std::string & _s){
		*((std::string *)value) = zs_strutils::unescape(_s);
	}

	std::string ScriptObjectString::toString(){
		return *((std::string *)value);
	}

	int ScriptObjectString::length(){
		return ((std::string *)value)->length();
	}

	ScriptObjectString *ScriptObjectString::sub(ScriptObjectString *s1){
		//std::string *str;
		ScriptObjectString *so_string = ZS_NEW_OBJECT_STRING(this->zs);
		so_string->set(zs_strutils::replace(this->toString(),s1->toString(),""));
		return so_string;
	}

}
