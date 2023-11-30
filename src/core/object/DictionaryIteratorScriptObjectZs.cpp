/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
#include "zetscript.h"

namespace zetscript{
	void	 DictionaryIteratorScriptObjectZs_next(ZetScript *_zs,DictionaryIteratorScriptObject *oi){
		ZS_UNUSUED_PARAM(_zs);
		oi->next();
	}

	void	DictionaryIteratorScriptObjectZs_get(ZetScript *_zs,DictionaryIteratorScriptObject *oi){
		ZS_UNUSUED_PARAM(_zs);
		oi->get();
	}


	bool	 DictionaryIteratorScriptObjectZs_end(ZetScript *_zs,DictionaryIteratorScriptObject *oi){
		ZS_UNUSUED_PARAM(_zs);
		return oi->end();
	}
}
