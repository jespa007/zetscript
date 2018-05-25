/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */

#pragma once

// Util to capture args by ...
#define ZETSCRIPT_CAPTURE_VARIABLE_ARGS(text_out, text_in)\
{va_list  ap;\
va_start(ap,  text_in);\
vsprintf(text_out,  text_in,  ap);\
va_end(ap);}

#define STR_EXPAND(tok) #tok
#define STR(tok) STR_EXPAND(tok)

#define STRCMP(a, R, b) (strcmp(a,b) R 0)

namespace zetscript{

	class CZetScriptUtils{


	};

};
