/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
#pragma once

namespace zetscript{

	class	StringIteratorScriptObject;

	zs_int 							StringScriptObjectWrap_size(ZetScript *_zs,StringScriptObject *so);
	bool 							StringScriptObjectWrap_contains(ZetScript *_zs,StringScriptObject *so, zs_string *str);
	bool 							StringScriptObjectWrap_contains(ZetScript *_zs,StringScriptObject *so, zs_int ch);
	void 							StringScriptObjectWrap_clear(ZetScript *_zs,StringScriptObject *so);
	StringScriptObject *			StringScriptObjectWrap_replace(ZetScript *_zs,StringScriptObject *str_in,zs_string *str_old, zs_string *str_new);
	void 							StringScriptObjectWrap_eraseAt(ZetScript *_zs,StringScriptObject *so, zs_int idx);
	void 							StringScriptObjectWrap_insertAt(ZetScript *_zs,StringScriptObject *so, zs_int idx,zs_int ch);
	VectorScriptObject * 			StringScriptObjectWrap_split(ZetScript *_zs,StringScriptObject *so,zs_int ch_delim);
	VectorScriptObject * 			StringScriptObjectWrap_split(ZetScript *_zs,StringScriptObject *so,zs_string * str_token);

	zs_int 							StringScriptObjectWrap_indexOf(ZetScript *_zs,StringScriptObject *so,zs_int search);
	zs_int 							StringScriptObjectWrap_indexOf(ZetScript *_zs,StringScriptObject *so,zs_string *search);
	bool 							StringScriptObjectWrap_startsWith(ZetScript *_zs,StringScriptObject *so,zs_string *prefix);
	bool 							StringScriptObjectWrap_endsWith(ZetScript *_zs,StringScriptObject *so,zs_string *suffix);
	StringScriptObject * 			StringScriptObjectWrap_substring(ZetScript *_zs,StringScriptObject *str_in,zs_int start,zs_int end);

	void 							StringScriptObjectWrap_append(ZetScript *_zs,StringScriptObject *str_in,StringScriptObject *str_append);

	StringIteratorScriptObject *	StringScriptObjectWrap_iter(ZetScript *_zs,StringScriptObject *str_in);


}
