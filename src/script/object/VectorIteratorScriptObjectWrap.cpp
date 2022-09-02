/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
#include "zetscript.h"

namespace zetscript{

	void	 VectorIteratorScriptObjectWrap_get(ZetScript *_zs,VectorIteratorScriptObject *vi){
		ZS_UNUSUED_PARAM(_zs);
		vi->get();
	}

	void	 VectorIteratorScriptObjectWrap_next(ZetScript *_zs,VectorIteratorScriptObject *vi){
		ZS_UNUSUED_PARAM(_zs);
		vi->next();
	}

	bool	 VectorIteratorScriptObjectWrap_end(ZetScript *_zs,VectorIteratorScriptObject *vi){
		ZS_UNUSUED_PARAM(_zs);
		return vi->end();
	}
}
