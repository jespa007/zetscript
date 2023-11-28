/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
#include "zetscript.h"

namespace zetscript{
	void	 ObjectIteratorObjectZs_next(ZetScript *_zs,ObjectIteratorObject *oi){
		ZS_UNUSUED_PARAM(_zs);
		oi->next();
	}

	void	ObjectIteratorObjectZs_get(ZetScript *_zs,ObjectIteratorObject *oi){
		ZS_UNUSUED_PARAM(_zs);
		oi->get();
	}


	bool	 ObjectIteratorObjectZs_end(ZetScript *_zs,ObjectIteratorObject *oi){
		ZS_UNUSUED_PARAM(_zs);
		return oi->end();
	}
}
