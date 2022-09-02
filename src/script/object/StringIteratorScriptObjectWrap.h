/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
#pragma once

namespace zetscript{
	void 			StringIteratorScriptObjectWrap_get(ZetScript *_zs,StringIteratorScriptObject *si);
	void			StringIteratorScriptObjectWrap_next(ZetScript *_zs,StringIteratorScriptObject *si);
	bool			StringIteratorScriptObjectWrap_end(ZetScript *_zs,StringIteratorScriptObject *si);
}
