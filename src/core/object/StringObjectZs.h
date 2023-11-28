/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
#pragma once

namespace zetscript{

	class	StringIteratorObject;

	zs_int 							StringObjectZs_length(ZetScript *_zs,StringObject *_so);
	bool 							StringObjectZs_contains(ZetScript *_zs,StringObject *so, String *_str);
	bool 							StringObjectZs_contains(ZetScript *_zs,StringObject *so, zs_int _ch);
	void 							StringObjectZs_clear(ZetScript *_zs,StringObject *_so);
	StringObject *			StringObjectZs_replace(ZetScript *_zs,StringObject *str_in,String *str_old, String *_str_new);
	void 							StringObjectZs_eraseAt(ZetScript *_zs,StringObject *so, zs_int _idx);
	void 							StringObjectZs_insertAt(ZetScript *_zs,StringObject *so, zs_int idx,zs_int _ch);
	void 							StringObjectZs_insertAt(ZetScript *_zs,StringObject *so, zs_int idx,String *_str);
	ArrayObject * 			StringObjectZs_split(ZetScript *_zs,StringObject *so,zs_int _ch_delim);
	ArrayObject * 			StringObjectZs_split(ZetScript *_zs,StringObject *so,String * _str_token);

	zs_int 							StringObjectZs_indexOf(ZetScript *_zs,StringObject *so,zs_int search);
	zs_int 							StringObjectZs_indexOf(ZetScript *_zs,StringObject *so,String *search);
	bool 							StringObjectZs_startsWith(ZetScript *_zs,StringObject *so,String *prefix);
	bool 							StringObjectZs_endsWith(ZetScript *_zs,StringObject *so,String *suffix);

	void 							StringObjectZs_append(ZetScript *_zs,StringObject *str_in,StringObject *_str_append);
	void 							StringObjectZs_append(ZetScript *_zs,StringObject *str_in,zs_int _ch);

	StringObject * 			StringObjectZs_toUpperCase(ZetScript *_zs,StringObject *so);
	StringObject * 			StringObjectZs_toLowerCase(ZetScript *_zs,StringObject *so);

	StringObject * 			StringObjectZs_subassignring(ZetScript *_zs,StringObject *_so, zs_int _start_index, zs_int _end_index);
	StringObject * 			StringObjectZs_subassignring(ZetScript *_zs,StringObject *_so, zs_int _start_index);


	StringIteratorObject *	StringObjectZs_iter(ZetScript *_zs,StringObject *_str_in);


}
