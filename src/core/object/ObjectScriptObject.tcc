/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
namespace zetscript{


	template<typename _T>
	bool 								ObjectScriptObject::elementInstanceOf(
		const String & _key
	){
		StackElement *stk;
		if((stk=getStackElementByKeyName(_key))==NULL){
			ZS_THROW_EXCEPTION("key '%s' not exist",_key.toConstChar());
		}

		return this->script_engine->isStackElementInstanceOf<_T>(stk);
	}

	template<typename _T>
	_T	ObjectScriptObject::get(const String & _key){
		StackElement *stk;
		if((stk=getStackElementByKeyName(_key))==NULL){
			ZS_THROW_EXCEPTION("key '%s' not exist",_key.toConstChar());
		}

		return this->script_engine->stackElementTo<_T>(stk);
	}

	template<typename _T>
	void	ObjectScriptObject::set(const String & _key_name, _T _value){
		StackElement stk=this->script_engine->toStackElement<_T>(_value);

		// Create shared object due 'ObjectScriptObject::set' is called from user program
		if(stk.properties == STACK_ELEMENT_PROPERTY_OBJECT){
			ScriptObject *obj = (ScriptObject *)stk.value;
			vm_create_shared_object(this->script_engine->getVirtualMachine(),obj);
		}

		this->setStackElementByKeyName(_key_name, &stk);

	}
}
