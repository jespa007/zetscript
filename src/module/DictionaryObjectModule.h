/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
namespace zetscript{
	class	ObjectIteratorObject;

	ArrayObject *				ObjectObjectZs_keys(ZetScript *_zs,ObjectObject *o1);
	bool 							ObjectObjectZs_contains(ZetScript *_zs,ObjectObject *o1, String * key);
	void 							ObjectObjectZs_clear(ZetScript *_zs,ObjectObject *o1);
	void 							ObjectObjectZs_erase(ZetScript *_zs,ObjectObject *o1, String * key);
	ObjectIteratorObject * 	ObjectObjectZs_iter(ZetScript *_zs,ObjectObject *oo);
	void						 	ObjectObjectZs_extend(ZetScript *_zs,ObjectObject *o1,ObjectObject *o2);

}
