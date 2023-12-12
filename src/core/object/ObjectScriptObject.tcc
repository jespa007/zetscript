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

		return this->se->stackElementInstanceOf<_T>(stk);
	}

	template<typename _T>
	_T	ObjectScriptObject::get(const String & _key){
		StackElement *stk;
		if((stk=getStackElementByKeyName(_key))==NULL){
			ZS_THROW_EXCEPTION("key '%s' not exist",_key.toConstChar());
		}

		return this->se->stackElementTo<_T>(stk);
	}

	template<typename _T>
	void	ObjectScriptObject::set(const String & _key_name, _T _value){
		StackElement stk=this->se->toStackElement<_T>(_value);

		// Create shared object due 'ObjectScriptObject::set' is called from user program
		if(stk.properties == STACK_ELEMENT_PROPERTY_OBJECT){
			ScriptObject *obj = (ScriptObject *)stk.value;
			vm_create_shared_object(this->se->getVirtualMachine(),obj);
		}

		this->setStackElementByKeyName(_key_name, &stk);

	}
}
