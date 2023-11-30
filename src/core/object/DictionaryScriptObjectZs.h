/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
namespace zetscript{
	class	DictionaryIteratorScriptObject;

	ArrayScriptObject *				DictionaryScriptObjectZs_keys(ZetScript *_zs,DictionaryScriptObject *o1);
	bool 							DictionaryScriptObjectZs_contains(ZetScript *_zs,DictionaryScriptObject *o1, String * key);
	void 							DictionaryScriptObjectZs_clear(ZetScript *_zs,DictionaryScriptObject *o1);
	void 							DictionaryScriptObjectZs_erase(ZetScript *_zs,DictionaryScriptObject *o1, String * key);
	DictionaryIteratorScriptObject * 	DictionaryScriptObjectZs_iter(ZetScript *_zs,DictionaryScriptObject *oo);
	void						 	DictionaryScriptObjectZs_extend(ZetScript *_zs,DictionaryScriptObject *o1,DictionaryScriptObject *o2);

}
