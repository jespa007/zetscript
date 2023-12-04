/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
#include "@base.h"

namespace zetscript{

	char  Character::toLower(char _ch){
		if('A' <= _ch && _ch <= 'Z'){
			 //Only if it's a upper letter
			return 'a'+(_ch-'A');
		}
		return _ch;
	}

	char  Character::toUpper(char _ch){
		if('a' <= _ch && _ch <= 'z'){
			 //Only if it's a upper letter
			return 'A'+(_ch-'a');
		}
		return _ch;
	}

	bool Character::isDigit(char _c){
		return (('0' <= _c) && (_c<='9'));
	}

	bool Character::isHexaDigit(char _c){
		return ((('0' <= _c) && (_c<='9')) || ('a'<=(Character::toLower(_c))&&(Character::toLower(_c)<='f')));
	}


}
