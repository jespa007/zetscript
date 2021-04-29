/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
#pragma once

namespace zetscript{
	StackElement 	ScriptObjectStringIteratorWrap_get(ScriptObjectStringIterator *si);
	void			ScriptObjectStringIteratorWrap_next(ScriptObjectStringIterator *si);
	bool			ScriptObjectStringIteratorWrap_end(ScriptObjectStringIterator *si);
}
