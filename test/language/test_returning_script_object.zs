// This test prove return script object that are not contanier. 

class A{
	var __value__="string"
	
	returnValue(){
		return this.__value__;
	}
	
	returnProperty(){
		return this.value;
	}
		
	
	value{
		_get(){
			return this.__value__;
		}
	}
}

// return from function 0
function returnValueFromFunction(){
	return a.__value__;
}

// return from function 1
function returnValueFromFunctionFunction(){
	return returnValueFromFunction();
}

// return from function 2
function returnPropertyFromFunction(){
	return a.value;
}

// return from function 3
function returnPropertyFromFunctionFunction(){
	return returnPropertyFromFunction();
}

// return from param function 5
function returnFromFunctionParam(_param){
	return _param;
}

// return from param function 6
function returnFromFunctionParamAndAssign(_param,_value){
	_param = _value;
	return returnFromFunctionParam(_param);
}





var a=new A();

System::assert(a.__value__ == "string") // return from __value__
System::assert(a.returnValue() == "string") // return from member function  
System::assert(a.returnProperty() == "string") // return from member property
System::assert(returnValueFromFunction() == "string") // return from function that return a.__value__ 
System::assert(returnValueFromFunctionFunction() == "string") // return from function from a function that return a.__value__
System::assert(returnPropertyFromFunction() == "string") // return from function that return property a.value
System::assert(returnPropertyFromFunctionFunction() == "string") // return from function in other function that return property a.value

System::assert(returnFromFunctionParam(a.__value__) == "string") //  return from function param a.__value__
System::assert(returnFromFunctionParam(a.returnValue()) == "string") // return from function param from function that return function from parameter
System::assert(returnFromFunctionParam(a.returnProperty()) == "string") // return from function param that return function from parameter
System::assert(returnFromFunctionParam(returnValueFromFunction()) == "string") // return from function param that return from function that return a.__value__
System::assert(returnFromFunctionParam(returnValueFromFunctionFunction()) == "string") // return from function param that return from function from a function that return a.__value__
System::assert(returnFromFunctionParam(returnPropertyFromFunction()) == "string") // return from function param that return from function that return property a.value
System::assert(returnFromFunctionParam(returnPropertyFromFunctionFunction()) == "string") //eturn from function param that return from function in other function that return property a.value

System::assert(returnFromFunctionParamAndAssign(a.__value__,0) == 0) //  return from function param a.__value__
System::assert(returnFromFunctionParamAndAssign(a.returnValue(),0) == 0) // return from function param from function that return function from parameter
System::assert(returnFromFunctionParamAndAssign(a.returnProperty(),0) == 0) // return from function param that return function from parameter
System::assert(returnFromFunctionParamAndAssign(returnValueFromFunction(),0) == 0) // return from function param that return from function that return a.__value__
System::assert(returnFromFunctionParamAndAssign(returnValueFromFunctionFunction(),0) == 0) // return from function param that return from function from a function that return a.__value__
System::assert(returnFromFunctionParamAndAssign(returnPropertyFromFunction(),0) == 0) // return from function param that return from function that return property a.value
System::assert(returnFromFunctionParamAndAssign(returnPropertyFromFunctionFunction(),0) == 0) //eturn from function param that return from function in other function that return property a.value

System::assert(returnFromFunctionParamAndAssign(a.__value__,{}) == 0) //  return from function param a.__value__
System::assert(returnFromFunctionParamAndAssign(a.returnValue(),{}) == 0) // return from function param from function that return function from parameter
System::assert(returnFromFunctionParamAndAssign(a.returnProperty(),{}) == 0) // return from function param that return function from parameter
System::assert(returnFromFunctionParamAndAssign(returnValueFromFunction(),{}) == 0) // return from function param that return from function that return a.__value__
System::assert(returnFromFunctionParamAndAssign(returnValueFromFunctionFunction(),{}) == 0) // return from function param that return from function from a function that return a.__value__
System::assert(returnFromFunctionParamAndAssign(returnPropertyFromFunction(),{}) == 0) // return from function param that return from function that return property a.value
System::assert(returnFromFunctionParamAndAssign(returnPropertyFromFunctionFunction(),{}) == 0) //eturn from function param that return from function in other function that return property a.value
