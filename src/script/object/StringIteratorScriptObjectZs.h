/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
#pragma once

namespace zetscript{
	void 			StringIteratorScriptObject_get(ZetScript *_zs,StringIteratorScriptObject *si);
	void			StringIteratorScriptObject_next(ZetScript *_zs,StringIteratorScriptObject *si);
	bool			StringIteratorScriptObject_end(ZetScript *_zs,StringIteratorScriptObject *si);
}
