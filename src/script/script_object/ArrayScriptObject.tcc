/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
namespace zetscript{


	template<typename _T>
	bool 								ArrayScriptObject::elementInstanceOf(int _pos){
		if(_pos >= stk_elements.size()){
			ZS_THROW_EXCEPTION("_pos index out of bounds (%i)",_pos);
		}

		return this->zs->stackElementInstanceOf<_T>(stk_elements.get(_pos));
	}

	template<typename _T>
	_T	ArrayScriptObject::get(int _pos){
		if(_pos >= stk_elements.size()){
			ZS_THROW_EXCEPTION("_pos index out of bounds (%i)",_pos);
		}

		return this->zs->stackElementTo<_T>(stk_elements.get(_pos));
	}

	template<typename _T>
	void	ArrayScriptObject::set(int _pos, _T _element){
		if(_pos >= stk_elements.size()){
			ZS_THROW_EXCEPTION("_pos index out of bounds (%i)",_pos);
		}
		auto stk=this->zs->toStackElement(_element);

		// Create shared object due 'ArrayScriptObject::set' is called from user program
		if(stk.properties == STACK_ELEMENT_PROPERTY_SCRIPT_OBJECT){
			ScriptObject *obj = (ScriptObject *)stk.value;
			vm_create_shared_script_object(this->zs->getVirtualMachine(),obj);
		}

		this->setStackElementByIndex(_pos,&stk);
	}

	template<typename _T>
	void ArrayScriptObject::push(_T _element){
		auto stk=this->zs->toStackElement(_element);

		// Create shared object due 'ArrayScriptObject::push' is called from user program
		if(stk.properties == STACK_ELEMENT_PROPERTY_SCRIPT_OBJECT){
			ScriptObject *obj = (ScriptObject *)stk.value;
			vm_create_shared_script_object(this->zs->getVirtualMachine(),obj);
		}


		this->pushStackElement(stk);
	}
}
