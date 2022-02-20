/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
#pragma once

namespace zetscript{
	void 			ScriptObjectIteratorStringWrap_get(ZetScript *_zs,ScriptObjectIteratorString *si);
	void			ScriptObjectIteratorStringWrap_next(ZetScript *_zs,ScriptObjectIteratorString *si);
	bool			ScriptObjectIteratorStringWrap_end(ZetScript *_zs,ScriptObjectIteratorString *si);
}
