/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
#include "zetscript.h"

namespace zetscript{

	void	 StringIteratorObjectZs_get(ZetScript *_zs,StringIteratorObject *si){
		ZS_UNUSUED_PARAM(_zs);
		si->get();
	}

	void	 StringIteratorObjectZs_next(ZetScript *_zs,StringIteratorObject *si){
		ZS_UNUSUED_PARAM(_zs);
		si->next();
	}

	bool	 StringIteratorObjectZs_end(ZetScript *_zs,StringIteratorObject *si){
		ZS_UNUSUED_PARAM(_zs);
		return si->end();
	}


}
