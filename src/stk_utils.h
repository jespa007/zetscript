/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
#pragma once

namespace zetscript{
	namespace stk_utils{

		zs_string		stk_to_str(ZetScript *_zs, StackElement *_stk,const zs_string & _format="");
		const char		*stk_to_str(ZetScript *_zs,char *_str_out,  StackElement *_stk,const zs_string & _format="");

		zs_string		stk_to_typeof_str(ZetScript *_zs, StackElement *_stk);
		const char		*stk_to_typeof_str(ZetScript *_zs,char *_str_out, StackElement *_stk);

		void			stk_assign(ZetScript *_zs,StackElement *_stk_dst, const StackElement *_stk_src);
		StackElement 	to_stk(ZetScript *_zs, zs_int ptr_var, short idx_builtin_type_var);
		bool			stk_to(ZetScript *_zs, StackElement * _stack_element, int _idx_builtin_type, zs_int *_ptr_var, zs_string  & _error);

		template<typename _C>
		StackElement	to_stk(ZetScript *_zs, _C _val);
	}
}

