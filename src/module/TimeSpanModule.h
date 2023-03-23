/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
#pragma once

namespace zetscript{

	// member functions
	zs_timespan * 		TimeSpanModule_new(ZetScript *_zs);

	zs_int 				TimeSpanModule_get_seconds(ZetScript *_zs,zs_timespan *_this);
	zs_int 				TimeSpanModule_get_minutes(ZetScript *_zs,zs_timespan *_this);
	zs_int 				TimeSpanModule_get_hours(ZetScript *_zs,zs_timespan *_this);

	zs_int 				TimeSpanModule_get_days(ZetScript *_zs,zs_timespan *_this);

	void		 		TimeSpanModule_delete(ZetScript *_zs,zs_timespan *_this);

}
