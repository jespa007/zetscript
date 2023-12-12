/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
namespace zetscript{

	class	ArrayIteratorScriptObject;

	void    						ArrayScriptObjectZs_push(ScriptEngine *_zs,ArrayScriptObject *sv,StackElement  * stk);
	void   							ArrayScriptObjectZs_pop(ScriptEngine *_zs,ArrayScriptObject *sv);
	zs_int 							ArrayScriptObjectZs_length(ScriptEngine *_zs,ArrayScriptObject *sv);
	void 							ArrayScriptObjectZs_insertAt(ScriptEngine *_zs,ArrayScriptObject *sv, zs_int idx,StackElement  * stk);
	void 							ArrayScriptObjectZs_eraseAt(ScriptEngine *_zs,ArrayScriptObject *sv, zs_int idx);
	void 							ArrayScriptObjectZs_clear(ScriptEngine *_zs,ArrayScriptObject *sv);
	StringScriptObject			*	ArrayScriptObjectZs_join(ScriptEngine *_zs,ArrayScriptObject *sv, zs_int idx_char);
	ArrayIteratorScriptObject 	* 	ArrayScriptObjectZs_iter(ScriptEngine *_zs,ArrayScriptObject *so);
	bool 							ArrayScriptObjectZs_contains(ScriptEngine *_zs,ArrayScriptObject *so, StackElement *stk);
	ArrayScriptObject 			*	ArrayScriptObjectZs_concat(ScriptEngine *_zs,ArrayScriptObject *_so1, ArrayScriptObject *_so2);
	void 							ArrayScriptObjectZs_extend(ScriptEngine *_zs,ArrayScriptObject *_so1, ArrayScriptObject *_so2);
	bool 							ArrayScriptObjectZs_equal(ScriptEngine *_zs,ArrayScriptObject *so, ArrayScriptObject *stk);

}
