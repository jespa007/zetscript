/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
#include "zetscript.h"

namespace zetscript{

	zs_int StringScriptObjectZs_length(ScriptEngine *_se,StringScriptObject *so){
		ZS_UNUSUED_PARAM(_se);
		return so->str_ptr->length();
	}

	bool StringScriptObjectZs_contains(ScriptEngine *_se,StringScriptObject *so, String *str){
		ZS_UNUSUED_PARAM(_se);
		return (*so->str_ptr).contains(
				*str
		);
	}

	bool StringScriptObjectZs_contains(ScriptEngine *_se,StringScriptObject *so, zs_int ch){
		ZS_UNUSUED_PARAM(_se);
		return strchr(so->str_ptr->toConstChar(),ch) != NULL;
	}

	void StringScriptObjectZs_clear(ScriptEngine *_se,StringScriptObject *so){
		ZS_UNUSUED_PARAM(_se);
		so->str_ptr->clear();
	}

	StringScriptObject *StringScriptObjectZs_replace(ScriptEngine *_se,StringScriptObject *str_in,String *str_old, String *str_new){
		StringScriptObject *str_out=ZS_NEW_STRING_SCRIPT_OBJECT(_se);
		str_out->set((str_in->str_ptr)->replace(*str_old,*str_new));
		return str_out;
	}

	void StringScriptObjectZs_eraseAt(ScriptEngine *_se,StringScriptObject *so, zs_int idx){
		ZS_UNUSUED_PARAM(_se);
		String *str=so->str_ptr;
		str->erase(idx);
	}

	void StringScriptObjectZs_insertAt(ScriptEngine *_se,StringScriptObject *so, zs_int idx,zs_int ch){
		ZS_UNUSUED_PARAM(_se);
		String *str=so->str_ptr;
		str->insert(idx,ch);
	}

	void StringScriptObjectZs_insertAt(ScriptEngine *_se,StringScriptObject *so, zs_int idx,String * _str){
		ZS_UNUSUED_PARAM(_se);
		String *str=so->str_ptr;
		str->insert(idx,*_str);
	}

	ArrayScriptObject * StringScriptObjectZs_split(ScriptEngine *_se,StringScriptObject *so,zs_int ch_delim){
		VirtualMachine *vm=_se->getVirtualMachine();
		ArrayScriptObject *sv=ZS_NEW_ARRAY_SCRIPT_OBJECT(_se);

		auto v=so->toString().split(ch_delim);

		for(int i=0; i<v.length(); i++){

			StackElement *stk=sv->newSlot();
			StringScriptObject *so_partial=ZS_NEW_STRING_SCRIPT_OBJECT(_se);
			so_partial->set(v.get(i));

			// create and share pointer
			vm_create_shared_object(vm,so_partial);
			vm_share_object(vm,so_partial);

			stk->value=(zs_int)so_partial;
			stk->properties = STACK_ELEMENT_PROPERTY_OBJECT;
		}

		return sv;
	}

	ArrayScriptObject * StringScriptObjectZs_split(ScriptEngine *_se,StringScriptObject *so, String * str_token){
		VirtualMachine *vm=_se->getVirtualMachine();
		ArrayScriptObject *sv=ZS_NEW_ARRAY_SCRIPT_OBJECT(_se);

		auto v=so->toString().split(*str_token);

		for(int i=0; i<v.length(); i++){
			StackElement *stk=sv->newSlot();
			StringScriptObject *so_partial=ZS_NEW_STRING_SCRIPT_OBJECT(_se);
			so_partial->set(v.get(i));

			// create and share pointer
			vm_create_shared_object(vm,so_partial);
			vm_share_object(vm,so_partial);

			stk->value=(zs_int)so_partial;
			stk->properties = STACK_ELEMENT_PROPERTY_OBJECT;

			// deallocate allocated string
			//delete (String *)v.items[i];

		}

		return sv;
	}

	zs_int StringScriptObjectZs_indexOf(ScriptEngine *_se,StringScriptObject *so,zs_int search){
		ZS_UNUSUED_PARAM(_se);
		String *str=so->str_ptr;
		String pattern="";
		pattern+=(char)+search;
		return str->find(pattern, 0);
	}

	zs_int StringScriptObjectZs_indexOf(ScriptEngine *_se,StringScriptObject *so,String *search){
		ZS_UNUSUED_PARAM(_se);
		String *str=so->str_ptr;
		return str->find(*search);
	}

	bool StringScriptObjectZs_startsWith(ScriptEngine *_se,StringScriptObject *so,String *prefix){
		ZS_UNUSUED_PARAM(_se);
		String *str=so->str_ptr;
		return str->startsWith(*prefix);
	}

	bool StringScriptObjectZs_endsWith(ScriptEngine *_se,StringScriptObject *so,String *suffix){
		ZS_UNUSUED_PARAM(_se);
		return so->str_ptr->endsWith(*suffix);
	}

	void 							StringScriptObjectZs_append(ScriptEngine *_se,StringScriptObject *str_in,StringScriptObject *str_append){
		ZS_UNUSUED_PARAM(_se);
		(str_in->str_ptr)->append(*(str_append->str_ptr));
	}

	void 							StringScriptObjectZs_append(ScriptEngine *_se,StringScriptObject *str_in,zs_int _ch){
		ZS_UNUSUED_PARAM(_se);
		(str_in->str_ptr)->append(_ch);
	}

	StringScriptObject * 	StringScriptObjectZs_toLowerCase(ScriptEngine *_se,StringScriptObject *so){
		StringScriptObject *str_out=ZS_NEW_STRING_SCRIPT_OBJECT(_se);
		str_out->set(so->toString().toLowerCase());
		return str_out;
	}

	StringScriptObject * 	StringScriptObjectZs_toUpperCase(ScriptEngine *_se,StringScriptObject *so){
		StringScriptObject *str_out=ZS_NEW_STRING_SCRIPT_OBJECT(_se);
		str_out->set(so->toString().toUpperCase());
		return str_out;
	}

	StringScriptObject * 	StringScriptObjectZs_substring(ScriptEngine *_se,StringScriptObject *_so, zs_int _start_index, zs_int _end_index){
		StringScriptObject *str_out=ZS_NEW_STRING_SCRIPT_OBJECT(_se);
		String result="";
		auto str_in=_so->getPtr();

		if(_start_index<0){
			ZS_THROW_RUNTIME_ERROR("_start_index negative (_start_index:%i)", _start_index);
		}
		if(_end_index < 0){
			_end_index=str_in->length()+_end_index;
		}
		if(_end_index>=str_in->length()){
			_end_index=str_in->length()-1;
		}

		if(_start_index<=_end_index){
			result=str_in->getSubstring(_start_index,_end_index-_start_index+1);
		}


		str_out->set(result);
		return str_out;
	}

	StringScriptObject * 	StringScriptObjectZs_substring(ScriptEngine *_se,StringScriptObject *_so, zs_int _start_index){
		return StringScriptObjectZs_substring(_se, _so,  _start_index,-1);
	}

	StringIteratorScriptObject * StringScriptObjectZs_iter(ScriptEngine *_se,StringScriptObject *so){
		ZS_UNUSUED_PARAM(_se);
		return ZS_NEW_STRING_ITERATOR_SCRIPT_OBJECT(_se,so);
	}
}
