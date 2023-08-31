/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
namespace zetscript{
	template<typename _T>
	_T	ObjectScriptObject::get(const zs_string & _key){
		StackElement *stk;
		if((stk=getStackElementByKeyName(_key))==NULL){
			ZS_THROW_EXCEPTION("key '%s' not exist",_key.c_str());
		}

		return this->zs->stackElementTo<_T>(stk);
	}

	template<typename _T>
	void	ObjectScriptObject::set(const zs_string & _key_name, _T _value){
		StackElement stk=this->zs->toStackElement<_T>(_value);

		// Create shared object due 'ObjectScriptObject::set' is called from user program
		if(stk.properties == ZS_STK_PROPERTY_SCRIPT_OBJECT){
			ScriptObject *obj = (ScriptObject *)stk.value;
			vm_create_shared_script_object(this->zs->getVirtualMachine(),obj);
		}

		this->setStackElementByKeyName(_key_name, &stk);

	}
}
