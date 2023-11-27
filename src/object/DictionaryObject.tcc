/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
namespace zetscript{


	template<typename _T>
	bool 								DictionaryObject::elementInstanceOf(
		const String & _key
	){
		StackElement *stk;
		if((stk=getStackElementByKeyName(_key))==NULL){
			ZS_THROW_EXCEPTION("key '%s' not exist",_key.toConstChar());
		}

		return this->zs->stackElementInstanceOf<_T>(stk);
	}

	template<typename _T>
	_T	DictionaryObject::get(const String & _key){
		StackElement *stk;
		if((stk=getStackElementByKeyName(_key))==NULL){
			ZS_THROW_EXCEPTION("key '%s' not exist",_key.toConstChar());
		}

		return this->zs->stackElementTo<_T>(stk);
	}

	template<typename _T>
	void	DictionaryObject::set(const String & _key_name, _T _value){
		StackElement stk=this->zs->toStackElement<_T>(_value);

		// Create shared object due 'DictionaryObject::set' is called from user program
		if(stk.properties == STACK_ELEMENT_PROPERTY_OBJECT){
			Object *obj = (Object *)stk.value;
			vm_create_shared_object(this->zs->getVirtualMachine(),obj);
		}

		this->setStackElementByKeyName(_key_name, &stk);

	}
}
