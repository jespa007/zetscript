/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
#pragma once

namespace zetscript{
	void 			StringIteratorScriptObjectZs_get(ScriptEngine *_zs,StringIteratorScriptObject *si);
	void			StringIteratorScriptObjectZs_next(ScriptEngine *_zs,StringIteratorScriptObject *si);
	bool			StringIteratorScriptObjectZs_end(ScriptEngine *_zs,StringIteratorScriptObject *si);
}
