/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
#pragma once

namespace zetscript{
	void 			StringIteratorScriptObjectZs_get(ZetScript *_zs,StringIteratorScriptObject *si);
	void			StringIteratorScriptObjectZs_next(ZetScript *_zs,StringIteratorScriptObject *si);
	bool			StringIteratorScriptObjectZs_end(ZetScript *_zs,StringIteratorScriptObject *si);
}
