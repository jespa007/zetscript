/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
namespace zetscript{

	class	ArrayIteratorScriptObject;

	void    						ArrayScriptObjectZs_push(ScriptEngine *_se,ArrayScriptObject *sv,StackElement  * stk);
	void   							ArrayScriptObjectZs_pop(ScriptEngine *_se,ArrayScriptObject *sv);
	zs_int 							ArrayScriptObjectZs_length(ScriptEngine *_se,ArrayScriptObject *sv);
	void 							ArrayScriptObjectZs_insertAt(ScriptEngine *_se,ArrayScriptObject *sv, zs_int idx,StackElement  * stk);
	void 							ArrayScriptObjectZs_eraseAt(ScriptEngine *_se,ArrayScriptObject *sv, zs_int idx);
	void 							ArrayScriptObjectZs_clear(ScriptEngine *_se,ArrayScriptObject *sv);
	StringScriptObject			*	ArrayScriptObjectZs_join(ScriptEngine *_se,ArrayScriptObject *sv, zs_int idx_char);
	ArrayIteratorScriptObject 	* 	ArrayScriptObjectZs_iter(ScriptEngine *_se,ArrayScriptObject *so);
	bool 							ArrayScriptObjectZs_contains(ScriptEngine *_se,ArrayScriptObject *so, StackElement *stk);
	ArrayScriptObject 			*	ArrayScriptObjectZs_concat(ScriptEngine *_se,ArrayScriptObject *_so1, ArrayScriptObject *_so2);
	void 							ArrayScriptObjectZs_extend(ScriptEngine *_se,ArrayScriptObject *_so1, ArrayScriptObject *_so2);
	bool 							ArrayScriptObjectZs_equal(ScriptEngine *_se,ArrayScriptObject *so, ArrayScriptObject *stk);

}
