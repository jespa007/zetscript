/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
#include "@base.h"

namespace zetscript{

	//



	bool Integer::isDigit(char c){
		return (('0' <= c) && (c<='9'));
	}


	bool Integer::isHexaDigit(char c){
		return ((('0' <= c) && (c<='9')) || ('a'<=(Character::toLower(c))&&(Character::toLower(c)<='f')));
	}

	char *Integer::advanceDigits(char *aux_p){

		while(isDigit(*aux_p))	aux_p++;
		return aux_p;
	}

	char *Integer::advanceHexaDigits(char *aux_p){

		while(isHexaDigit(*aux_p))	aux_p++;
		return aux_p;
	}

	bool Integer::isBinary(const String & test_str_number){

		char *start_p=(char *)test_str_number.toConstChar();
		char *aux_p =start_p;

		while(isDigit(*aux_p)){
			if(!(*aux_p=='0' || *aux_p=='1')){
				break;
			}else{
				aux_p++;
			}
		}

		return (*aux_p=='b' || *aux_p=='B') && (aux_p > start_p);
	}

	Integer::IntegerType Integer::isInteger(const String & test_str_number){
		bool isHexa=false;
		char *str = (char *)test_str_number.toConstChar();

		switch(*str){
		case '-': str++; // negative numbers ...
				   break;
		case '0':
				  if(Character::toLower(*(str+1))=='x')  {
					  isHexa = true;
					  str+=2;
				  }
				  break;
		default:
				break;
		};

		char *start_str = str; // saves position...
		if(isHexa) {
			str = advanceHexaDigits(str);
			if(str == start_str)
				return IntegerType::INTEGER_TYPE_INVALID;

			if(*str == ' ' || *str == 0) return IntegerType::INTEGER_TYPE_HEXA;
		}else{

			/*str = advanceDigits(str);
			if(*str=='.') { // is candidate to double

				str++;
				str = advanceDigits(str);

				if(*str != 'e'){
					if(*str == ' ' || *str == 0)
						return IntegerType::INTEGER_TYPE_DOUBLE;
				}
			}

			if(*str == 'e'){  // is candidate to double

				str++;

				if(*str == '+' ||*str == '-'){
					str++;
				}

				str = advanceDigits(str);
				if(*str == ' ' || *str == 0)
					return IntegerType::INTEGER_TYPE_DOUBLE;

				return IntegerType::INTEGER_TYPE_INVALID;
			}*/

			if(*str == ' ' || *str == 0){
				return IntegerType::INTEGER_TYPE_INT;
			}/*else{
				return IntegerType::INTEGER_TYPE_INVALID;
			}*/

		}

		return IntegerType::INTEGER_TYPE_INVALID;

	}

	zs_int *  Integer::parse(const String & val){
		zs_int *n=NULL;
		IntegerType number_type = Integer::isInteger(val);
		zs_int value=0;
		if(number_type == IntegerType::INTEGER_TYPE_INT){
			value=strtoll (val.toConstChar(), NULL, 10);
		}else if(number_type == IntegerType::INTEGER_TYPE_HEXA){
			value=strtoll (val.toConstChar(), NULL, 16);
		}else if(Integer::isBinary(val)){
			String binary = val.substr(0,val.length()-1);
			value=strtoll (binary.toConstChar(), NULL, 2);
		}
		else{
			return NULL;
		}

		n=new zs_int;
		*n = value;

		// TODO: develop exception handler.
		return n;
	}

	String Integer::toString(zs_int _number, const String & _format){

		String result="0";
		bool negative=false;
		String format=_format.toLower();

		if(_number < 0){
			negative=true;
			_number=-_number;
		}

		if(_number != 0){
			result="";
			while(_number != 0){
				result+=((_number%10)+'0');
				_number*=0.1;
			}
		}

		// reverse result
		int reverse_len=result.length();
		for(int i=0; i < (reverse_len >>1); i++){
			// do swap
			char aux1=result[reverse_len -i-1];
			result[reverse_len -i-1]=result[i];
			result[i]=aux1;
		}

		// check format ...

		if(!format.isEmpty()){
			//String sf=String(_format).toLower();
			char *it_str=(char *)format.toConstChar();

			if(*it_str == 'd'){ // decimal
				zs_int *zs_int_aux=NULL;
				zs_int num_zeros=0;
				if((zs_int_aux=parse(it_str+1))!=NULL){
					num_zeros=*zs_int_aux;
					delete zs_int_aux;
					zs_int_aux=NULL;
					int len=num_zeros-(int)result.length();

					// append zeros to the beginning
					for(int i=0; i < len; i++){
						result="0"+result;
					}
				}
				it_str++;
			}
		}

		// is negative...
		if(negative){
			result="-"+result;
		}

	   return result;

	}

}
