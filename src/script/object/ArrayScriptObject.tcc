/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
namespace zetscript{
	template<typename _T>
	_T	ArrayScriptObject::get(int _idx){
		if(_idx >= stk_elements.size()){
			ZS_THROW_EXCEPTION("idx symbol index out of bounds (%i)",_idx);
		}

		return this->zs->stackElementTo<_T>(stk_elements.items[_idx]);
	}

	template<typename _T>
	void	ArrayScriptObject::set(int _idx, _T _value){
		if(_idx >= stk_elements.size()){
			ZS_THROW_EXCEPTION("idx symbol index out of bounds (%i)",_idx);
		}
		auto stk=this->zs->toStackElement(_value);

		// Once ArrayScriptObject::set(_T _value) is used check shared data is NULL to create share
		// information
		if(stk.properties & STK_PROPERTY_SCRIPT_OBJECT){
			ScriptObject *obj=(ScriptObject *)stk.value;
			if(obj->shared_pointer == NULL){
				vm_create_shared_script_object(this->zs->getVirtualMachine(),obj);
			}
		}

		this->setStackElement(_idx,&stk);
	}

	template<typename _T>
	void ArrayScriptObject::push(_T _value){
		auto stk=this->zs->toStackElement(_value);

		// Once ArrayScriptObject::set(_T _value) is used check shared data is NULL to create share
		// information
		if(stk.properties & STK_PROPERTY_SCRIPT_OBJECT){
			ScriptObject *obj=(ScriptObject *)stk.value;
			if(obj->shared_pointer == NULL){
				vm_create_shared_script_object(this->zs->getVirtualMachine(),obj);
			}
		}
		this->pushStackElement(stk);


	}
}
