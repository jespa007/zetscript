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
	StringScriptObject * StringScriptObject::newStringScriptObject(ZetScript *_zs, const zs_string & _str){
		StringScriptObject *so=new StringScriptObject(_zs);
		so->set(_str);

		return so;
	}

	StringScriptObject *StringScriptObject::newStringScriptObjectAddStk(ZetScript *_zs,StackElement *stk_result_op1,StackElement *stk_result_op2){
		StringScriptObject *so_ref=NULL;
		// we have to create an new string variable
		if(STK_IS_SCRIPT_OBJECT_STRING(stk_result_op1)){
			so_ref=(StringScriptObject *)stk_result_op1->value;
		}

		if(so_ref == NULL){
		   if(STK_IS_SCRIPT_OBJECT_STRING(stk_result_op2)){
			   so_ref=(StringScriptObject *)stk_result_op2->value;
		   }else{
			   ZS_THROW_RUNTIME_ERRORF("Expected one of both operants as string var");
		   }
		}

		//zs_string *str;
		StringScriptObject *so_string = ZS_NEW_STRING_OBJECT(_zs);

		zs_string str1;
		zs_string str2;

		zs_string * str_dst[]={
			   &str1,
			   &str2
		};

		StackElement * stk_src[]={
			   stk_result_op1,
			   stk_result_op2
		};

		zs_string  ** str_dst_it=str_dst;
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


	StringScriptObject * StringScriptObject::format(
			ZetScript *_zs,
			StackElement *_stk
			, StackElement *_stk_args
	){

		zs_string str_input;
		zs_string str_result;
		StackElement stk=*_stk;
		VectorScriptObject *sov=NULL;
		zs_string str_num_aux;
		bool error=false;
		char str_error[512]={0};
		zs_int *ptr_idx_num=NULL;



		if(stk.properties & STK_PROPERTY_CONTAINER_SLOT){
			stk.value=(zs_int)(((ContainerSlot *)stk.value)->getSrcContainerRef());
			stk.properties=STK_PROPERTY_SCRIPT_OBJECT;
		}

		if(stk.properties & STK_PROPERTY_SCRIPT_OBJECT){
			// transform '\"' to '"','\n' to carry returns, etc
			str_input=zs_strutils::unescape(((ScriptObject *)stk.value)->toString());
		}
		else{
			str_input=stk_to_str(_zs, &stk);
		}

		if(_stk_args->properties & STK_PROPERTY_PTR_STK){
			_stk_args=(StackElement *)_stk_args->value;
		}

		if(_stk_args->properties & STK_PROPERTY_SCRIPT_OBJECT){
			ScriptObject *so=(ScriptObject *)_stk_args->value;
			if(so->idx_script_type == IDX_TYPE_SCRIPT_OBJECT_VECTOR){
				sov=(VectorScriptObject *)so;
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
								char *str_begin_padding_it=str_begin_padding+1;
								// try to convert str to index...
								for(;str_begin_padding_it<str_end_aligment;){
									str_num_aux+=*str_begin_padding_it++;
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
								zs_string str_format_results="";
								StackElement stk_arg=*sov->getUserElementAt(idx_num);

								if(stk_arg.properties & STK_PROPERTY_CONTAINER_SLOT){
									stk_arg.value=(zs_int)(((ContainerSlot *)stk_arg.value)->getSrcContainerRef());
									stk_arg.properties=STK_PROPERTY_SCRIPT_OBJECT;
								}

								if(stk_arg.properties & STK_PROPERTY_SCRIPT_OBJECT){
									str_format_results=((ScriptObject *)stk_arg.value)->toString();
								}else{
									str_format_results=stk_to_str(
										_zs
										,&stk_arg
										,ptr_str_format_string
									);
								}

								// set padding
								if(padding>0){
									int length_padding=padding-str_format_results.length();
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
			vm_set_error(_zs->getVirtualMachine(),str_error);
			return NULL;
		}

		StringScriptObject *str_out=ZS_NEW_STRING_OBJECT(_zs);
		str_out->set(str_result);//str_in->default_str_value;


		return str_out;
	}

	//
	// Helpers
	//
	//----------------------------------------------

	StringScriptObject::StringScriptObject(
			ZetScript *_zs
	):ScriptObject(_zs,IDX_TYPE_SCRIPT_OBJECT_STRING){
		default_str_value = "";
		str_ptr = &default_str_value;
	}

	void StringScriptObject::set(const zs_string & _s){
		*str_ptr = zs_strutils::unescape(_s);
	}

	const zs_string & StringScriptObject::get(){
		return *str_ptr;
	}

	const char *StringScriptObject::getConstChar(){
		return str_ptr->c_str();
	}

	zs_string StringScriptObject::toString(){
		return *str_ptr;
	}

	int StringScriptObject::length(){
		return str_ptr->length();
	}

	StringScriptObject *StringScriptObject::sub(StringScriptObject *s1){
		//zs_string *str;
		StringScriptObject *so_string = ZS_NEW_STRING_OBJECT(this->zs);
		so_string->set(zs_strutils::replace(this->toString(),s1->toString(),""));
		return so_string;
	}

}
