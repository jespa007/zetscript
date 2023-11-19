/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */

namespace zetscript{

	template<typename _C>
	_C ClassScriptObject::to(){
		const char * native_name = typeid(_C).name();
		int id=ZS_UNDEFINED_IDX;
		if((id=zs->getScriptTypeFactory()->getIdxScriptTypeFromTypeNamePtr(native_name))==ZS_UNDEFINED_IDX){
			ZS_THROW_RUNTIME_ERROR(
			"Cannot instance script object as native type '%s' because is not registered"
			,zs_rtti::demangle(typeid(_C).name()).c_str()
			);
		}

		return (_C)this->getNativeObject();
	}

}
