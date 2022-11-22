/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
namespace zetscript{

	template<typename _C>
	_C stk_to(ZetScript *_zs, StackElement * _stk){

		bool is_constant=false;
		StackElement stk=*_stk;
		zs_string error;
		zs_int ptr_var;
		zs_string str_script_type_ptr = typeid(_C).name();
		ScriptTypeFactory *_script_factory=_zs->getScriptTypeFactory();
		ScriptType *script_type = _script_factory->getScriptTypeFromTypeNamePtr(str_script_type_ptr);

		if(script_type == NULL){
			ZS_THROW_RUNTIME_ERROR("Type '%s' not registered",zs_rtti::demangle(str_script_type_ptr.c_str()).c_str());
		}

		if(stk_to(_zs,_stk, script_type->idx_script_type, &ptr_var,error)==false){
			ZS_THROW_RUNTIME_ERROR("Error converting StackElement to '%s': %s"
					,zs_rtti::demangle(str_script_type_ptr.c_str()).c_str()
					,error.c_str()
			);
		}
		return (_C)ptr_var;
	}

	template<typename _C>
	StackElement	to_stk(ZetScript *_zs, _C _val){
		bool is_constant=false;
		zs_string error;
		StackElement stk;
		zs_string str_script_type_ptr = typeid(_C).name();
		ScriptTypeFactory *_script_factory=_zs->getScriptTypeFactory();
		ScriptType *script_type = _script_factory->getScriptTypeFromTypeNamePtr(str_script_type_ptr);

		if(script_type == NULL){
			ZS_THROW_RUNTIME_ERROR("Type '%s' not registered",zs_rtti::demangle(str_script_type_ptr.c_str()).c_str());
		}

		return to_stk(_zs,(zs_int)_val,script_type->idx_script_type);
	}
}
