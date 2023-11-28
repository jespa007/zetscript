/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
namespace zetscript{


	template<typename _T>
	bool 								ArrayObject::elementInstanceOf(int _pos){
		if(_pos >= stk_elements.length()){
			ZS_THROW_EXCEPTION("_pos index out of bounds (%i)",_pos);
		}

		return this->zs->stackElementInstanceOf<_T>(stk_elements.get(_pos));
	}

	template<typename _T>
	_T	ArrayObject::get(int _pos){
		if(_pos >= stk_elements.length()){
			ZS_THROW_EXCEPTION("_pos index out of bounds (%i)",_pos);
		}

		return this->zs->stackElementTo<_T>(stk_elements.get(_pos));
	}

	template<typename _T>
	void	ArrayObject::set(int _pos, _T _element){
		if(_pos >= stk_elements.length()){
			ZS_THROW_EXCEPTION("_pos index out of bounds (%i)",_pos);
		}
		auto stk=this->zs->toStackElement(_element);

		// Create shared object due 'ArrayObject::set' is called from user program
		if(stk.properties == STACK_ELEMENT_PROPERTY_OBJECT){
			Object *obj = (Object *)stk.value;
			vm_create_shared_object(this->zs->getVirtualMachine(),obj);
		}

		this->setStackElementByIndex(_pos,&stk);
	}

	template<typename _T>
	void ArrayObject::push(_T _element){
		auto stk=this->zs->toStackElement(_element);

		// Create shared object due 'ArrayObject::push' is called from user program
		if(stk.properties == STACK_ELEMENT_PROPERTY_OBJECT){
			Object *obj = (Object *)stk.value;
			vm_create_shared_object(this->zs->getVirtualMachine(),obj);
		}


		this->pushStackElement(stk);
	}
}
