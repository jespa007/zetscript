/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
namespace zetscript{

	class	ArrayIteratorScriptObject;

	void    						ArrayScriptObjectWrap_push(ZetScript *_zs,ArrayScriptObject *sv,StackElement  * stk);
	void   							ArrayScriptObjectWrap_pop(ZetScript *_zs,ArrayScriptObject *sv);
	zs_int 							ArrayScriptObjectWrap_length(ZetScript *_zs,ArrayScriptObject *sv);
	void 							ArrayScriptObjectWrap_insertAt(ZetScript *_zs,ArrayScriptObject *sv, zs_int idx,StackElement  * stk);
	void 							ArrayScriptObjectWrap_eraseAt(ZetScript *_zs,ArrayScriptObject *sv, zs_int idx);
	void 							ArrayScriptObjectWrap_clear(ZetScript *_zs,ArrayScriptObject *sv);
	StringScriptObject			*	ArrayScriptObjectWrap_join(ZetScript *_zs,ArrayScriptObject *sv, zs_int idx_char);
	ArrayIteratorScriptObject 	* 	ArrayScriptObjectWrap_iter(ZetScript *_zs,ArrayScriptObject *so);
	bool 							ArrayScriptObjectWrap_contains(ZetScript *_zs,ArrayScriptObject *so, StackElement *stk);
	ArrayScriptObject 			*	ArrayScriptObjectWrap_concat(ZetScript *_zs,ArrayScriptObject *_so1, ArrayScriptObject *_so2);
	void 							ArrayScriptObjectWrap_extend(ZetScript *_zs,ArrayScriptObject *_so1, ArrayScriptObject *_so2);
	bool 							ArrayScriptObjectWrap_equal(ZetScript *_zs,ArrayScriptObject *so, ArrayScriptObject *stk);

}
