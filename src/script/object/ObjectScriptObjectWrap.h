/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
namespace zetscript{
	class	ObjectIteratorScriptObject;

	ArrayScriptObject *			ObjectScriptObjectWrap_keys(ZetScript *_zs,ObjectScriptObject *o1);
	bool 							ObjectScriptObjectWrap_contains(ZetScript *_zs,ObjectScriptObject *o1, zs_string * key);
	void 							ObjectScriptObjectWrap_clear(ZetScript *_zs,ObjectScriptObject *o1);
	void 							ObjectScriptObjectWrap_erase(ZetScript *_zs,ObjectScriptObject *o1, zs_string * key);
	ObjectIteratorScriptObject * 	ObjectScriptObjectWrap_iter(ZetScript *_zs,ObjectScriptObject *oo);
	void						 	ObjectScriptObjectWrap_extend(ZetScript *_zs,ObjectScriptObject *o1,ObjectScriptObject *o2);

}
