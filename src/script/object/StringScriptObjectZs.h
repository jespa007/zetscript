/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
#pragma once

namespace zetscript{

	class	StringIteratorScriptObject;

	zs_int 							StringScriptObject_length(ZetScript *_zs,StringScriptObject *_so);
	bool 							StringScriptObject_contains(ZetScript *_zs,StringScriptObject *so, zs_string *_str);
	bool 							StringScriptObject_contains(ZetScript *_zs,StringScriptObject *so, zs_int _ch);
	void 							StringScriptObject_clear(ZetScript *_zs,StringScriptObject *_so);
	StringScriptObject *			StringScriptObject_replace(ZetScript *_zs,StringScriptObject *str_in,zs_string *str_old, zs_string *_str_new);
	void 							StringScriptObject_eraseAt(ZetScript *_zs,StringScriptObject *so, zs_int _idx);
	void 							StringScriptObject_insertAt(ZetScript *_zs,StringScriptObject *so, zs_int idx,zs_int _ch);
	void 							StringScriptObject_insertAt(ZetScript *_zs,StringScriptObject *so, zs_int idx,zs_string *_str);
	ArrayScriptObject * 			StringScriptObject_split(ZetScript *_zs,StringScriptObject *so,zs_int _ch_delim);
	ArrayScriptObject * 			StringScriptObject_split(ZetScript *_zs,StringScriptObject *so,zs_string * _str_token);

	zs_int 							StringScriptObject_indexOf(ZetScript *_zs,StringScriptObject *so,zs_int search);
	zs_int 							StringScriptObject_indexOf(ZetScript *_zs,StringScriptObject *so,zs_string *search);
	bool 							StringScriptObject_startsWith(ZetScript *_zs,StringScriptObject *so,zs_string *prefix);
	bool 							StringScriptObject_endsWith(ZetScript *_zs,StringScriptObject *so,zs_string *suffix);

	void 							StringScriptObject_append(ZetScript *_zs,StringScriptObject *str_in,StringScriptObject *_str_append);
	void 							StringScriptObject_append(ZetScript *_zs,StringScriptObject *str_in,zs_int _ch);

	StringScriptObject * 			StringScriptObject_toUpperCase(ZetScript *_zs,StringScriptObject *so);
	StringScriptObject * 			StringScriptObject_toLowerCase(ZetScript *_zs,StringScriptObject *so);

	StringScriptObject * 			StringScriptObject_substring(ZetScript *_zs,StringScriptObject *_so, zs_int _start_index, zs_int _end_index);
	StringScriptObject * 			StringScriptObject_substring(ZetScript *_zs,StringScriptObject *_so, zs_int _start_index);


	StringIteratorScriptObject *	StringScriptObject_iter(ZetScript *_zs,StringScriptObject *_str_in);


}
