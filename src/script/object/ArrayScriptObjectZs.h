/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
namespace zetscript{

	class	ArrayIteratorScriptObject;

	void    						ArrayScriptObject_push(ZetScript *_zs,ArrayScriptObject *sv,StackElement  * stk);
	void   							ArrayScriptObject_pop(ZetScript *_zs,ArrayScriptObject *sv);
	zs_int 							ArrayScriptObject_length(ZetScript *_zs,ArrayScriptObject *sv);
	void 							ArrayScriptObject_insertAt(ZetScript *_zs,ArrayScriptObject *sv, zs_int idx,StackElement  * stk);
	void 							ArrayScriptObject_eraseAt(ZetScript *_zs,ArrayScriptObject *sv, zs_int idx);
	void 							ArrayScriptObject_clear(ZetScript *_zs,ArrayScriptObject *sv);
	StringScriptObject			*	ArrayScriptObject_join(ZetScript *_zs,ArrayScriptObject *sv, zs_int idx_char);
	ArrayIteratorScriptObject 	* 	ArrayScriptObject_iter(ZetScript *_zs,ArrayScriptObject *so);
	bool 							ArrayScriptObject_contains(ZetScript *_zs,ArrayScriptObject *so, StackElement *stk);
	ArrayScriptObject 			*	ArrayScriptObject_concat(ZetScript *_zs,ArrayScriptObject *_so1, ArrayScriptObject *_so2);
	void 							ArrayScriptObject_extend(ZetScript *_zs,ArrayScriptObject *_so1, ArrayScriptObject *_so2);
	bool 							ArrayScriptObject_equal(ZetScript *_zs,ArrayScriptObject *so, ArrayScriptObject *stk);

}
