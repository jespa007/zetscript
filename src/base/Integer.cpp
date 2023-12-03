/*
 *  This file is distributed under the MIT License.
 *  See LICENSE file for details.
 */
#include "@base.h"

namespace zetscript{

	//



	zs_int *  Integer::parse(const String & val){
		zs_int *n=NULL;
		String::NumberType number_type = String::isNumber(val);
		zs_int value=0;
		if(number_type == String::NumberType::NUMBER_TYPE_INT){
			value=strtoll (val.toConstChar(), NULL, 10);
		}else if(number_type == String::NumberType::NUMBER_TYPE_HEXA){
			value=strtoll (val.toConstChar(), NULL, 16);
		}else if(number_type == String::NumberType::NUMBER_TYPE_BINARY){
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
