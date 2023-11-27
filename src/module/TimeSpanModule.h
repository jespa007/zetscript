/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
#pragma once

namespace zetscript{

	// member functions
	TimeSpan * 		TimeSpanModule_new(ZetScript *_zs);

	zs_int 				TimeSpanModule_get_seconds(ZetScript *_zs,TimeSpan *_this);
	zs_int 				TimeSpanModule_get_minutes(ZetScript *_zs,TimeSpan *_this);
	zs_int 				TimeSpanModule_get_hours(ZetScript *_zs,TimeSpan *_this);

	zs_int 				TimeSpanModule_get_days(ZetScript *_zs,TimeSpan *_this);

	void		 		TimeSpanModule_delete(ZetScript *_zs,TimeSpan *_this);

}
