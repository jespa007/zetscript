/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
namespace zetscript{
	class	ScriptObjectIteratorObject;

	ScriptObjectVector *			ScriptObjectObjectWrap_keys(ZetScript *_zs,ScriptObjectObject *o1);
	bool 							ScriptObjectObjectWrap_contains(ZetScript *_zs,ScriptObjectObject *o1, zs_string * key);
	void 							ScriptObjectObjectWrap_clear(ZetScript *_zs,ScriptObjectObject *o1);
	void 							ScriptObjectObjectWrap_erase(ZetScript *_zs,ScriptObjectObject *o1, zs_string * key);
	ScriptObjectIteratorObject * 	ScriptObjectObjectWrap_iter(ZetScript *_zs,ScriptObjectObject *oo);
	void						 	ScriptObjectObjectWrap_append(ZetScript *_zs,ScriptObjectObject *o1,ScriptObjectObject *o2);


}
