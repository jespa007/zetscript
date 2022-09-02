/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */

namespace zetscript{

	template<typename _C>
	_C ClassScriptObject::to(){
		const char * str_script_type_ptr = typeid(_C).name();
		int idx_script_type=ZS_IDX_UNDEFINED;
		if((idx_script_type=zs->getScriptTypeFactory()->getIdxScriptTypeFromTypeNamePtr(str_script_type_ptr))==ZS_IDX_UNDEFINED){
			THROW_RUNTIME_ERROR(
			"Cannot instance script object as native type '%s' because is not registered"
			,zs_rtti::demangle(typeid(_C).name()).c_str()
			);
		}

		return (_C)this->getNativeObject();
	}

}
