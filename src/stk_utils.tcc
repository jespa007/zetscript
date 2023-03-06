/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */

namespace zetscript{
	namespace stk_utils{
		template<typename _C>
		StackElement	to_stk(ZetScript *_zs, _C _val){
			zs_string error;
			zs_string str_script_type_ptr = typeid(_C).name();
			ScriptTypeFactory *_script_factory=_zs->getScriptTypeFactory();
			ScriptType *script_type = _script_factory->getScriptTypeFromTypeNamePtr(str_script_type_ptr);

			if(script_type == NULL){
				ZS_THROW_RUNTIME_ERROR("Type '%s' not registered",zs_rtti::demangle(str_script_type_ptr.c_str()).c_str());
			}

			return to_stk(_zs,(zs_int)_val,script_type->idx_script_type);
		}
	}
}
