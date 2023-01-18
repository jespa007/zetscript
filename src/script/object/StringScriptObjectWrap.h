/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
#pragma once

namespace zetscript{

	class	StringIteratorScriptObject;

	zs_int 							StringScriptObjectWrap_length(ZetScript *_zs,StringScriptObject *_so);
	bool 							StringScriptObjectWrap_contains(ZetScript *_zs,StringScriptObject *so, zs_string *_str);
	bool 							StringScriptObjectWrap_contains(ZetScript *_zs,StringScriptObject *so, zs_int _ch);
	void 							StringScriptObjectWrap_clear(ZetScript *_zs,StringScriptObject *_so);
	StringScriptObject *			StringScriptObjectWrap_replace(ZetScript *_zs,StringScriptObject *str_in,zs_string *str_old, zs_string *_str_new);
	void 							StringScriptObjectWrap_eraseAt(ZetScript *_zs,StringScriptObject *so, zs_int _idx);
	void 							StringScriptObjectWrap_insertAt(ZetScript *_zs,StringScriptObject *so, zs_int idx,zs_int _ch);
	void 							StringScriptObjectWrap_insertAt(ZetScript *_zs,StringScriptObject *so, zs_int idx,zs_string *_str);
	VectorScriptObject * 			StringScriptObjectWrap_split(ZetScript *_zs,StringScriptObject *so,zs_int _ch_delim);
	VectorScriptObject * 			StringScriptObjectWrap_split(ZetScript *_zs,StringScriptObject *so,zs_string * _str_token);

	zs_int 							StringScriptObjectWrap_indexOf(ZetScript *_zs,StringScriptObject *so,zs_int search);
	zs_int 							StringScriptObjectWrap_indexOf(ZetScript *_zs,StringScriptObject *so,zs_string *search);
	bool 							StringScriptObjectWrap_startsWith(ZetScript *_zs,StringScriptObject *so,zs_string *prefix);
	bool 							StringScriptObjectWrap_endsWith(ZetScript *_zs,StringScriptObject *so,zs_string *suffix);

	void 							StringScriptObjectWrap_append(ZetScript *_zs,StringScriptObject *str_in,StringScriptObject *_str_append);
	StringScriptObject * 			StringScriptObjectWrap_toUpperCase(ZetScript *_zs,StringScriptObject *so);
	StringScriptObject * 			StringScriptObjectWrap_toLowerCase(ZetScript *_zs,StringScriptObject *so);

	StringScriptObject * 			StringScriptObjectWrap_substring(ZetScript *_zs,StringScriptObject *_so, zs_int _start_index, zs_int _end_index);
	StringScriptObject * 			StringScriptObjectWrap_substring(ZetScript *_zs,StringScriptObject *_so, zs_int _start_index);


	StringIteratorScriptObject *	StringScriptObjectWrap_iter(ZetScript *_zs,StringScriptObject *_str_in);


}
