/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
#pragma once

namespace zetscript{
	void 			StringIteratorObjectZs_get(ZetScript *_zs,StringIteratorObject *si);
	void			StringIteratorObjectZs_next(ZetScript *_zs,StringIteratorObject *si);
	bool			StringIteratorObjectZs_end(ZetScript *_zs,StringIteratorObject *si);
}
