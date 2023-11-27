/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
namespace zetscript{

	class	ArrayIteratorObject;

	void    						ArrayObjectZs_push(ZetScript *_zs,ArrayObject *sv,StackElement  * stk);
	void   							ArrayObjectZs_pop(ZetScript *_zs,ArrayObject *sv);
	zs_int 							ArrayObjectZs_length(ZetScript *_zs,ArrayObject *sv);
	void 							ArrayObjectZs_insertAt(ZetScript *_zs,ArrayObject *sv, zs_int idx,StackElement  * stk);
	void 							ArrayObjectZs_eraseAt(ZetScript *_zs,ArrayObject *sv, zs_int idx);
	void 							ArrayObjectZs_clear(ZetScript *_zs,ArrayObject *sv);
	StringObject			*	ArrayObjectZs_join(ZetScript *_zs,ArrayObject *sv, zs_int idx_char);
	ArrayIteratorObject 	* 	ArrayObjectZs_iter(ZetScript *_zs,ArrayObject *so);
	bool 							ArrayObjectZs_contains(ZetScript *_zs,ArrayObject *so, StackElement *stk);
	ArrayObject 			*	ArrayObjectZs_concat(ZetScript *_zs,ArrayObject *_so1, ArrayObject *_so2);
	void 							ArrayObjectZs_extend(ZetScript *_zs,ArrayObject *_so1, ArrayObject *_so2);
	bool 							ArrayObjectZs_equal(ZetScript *_zs,ArrayObject *so, ArrayObject *stk);

}
