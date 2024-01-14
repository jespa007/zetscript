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

// return from function 2
function returnPropertyFromFunctionFunction(){
	return returnPropertyFromFunction();
}



var a=new A();

System::assert(a.__value__ == "string") // from direct
System::assert(a.returnValue() == "string") // this direct  
System::assert(a.returnProperty() == "string") // property
System::assert(returnValueFromFunction() == "string") // from function 
System::assert(returnValueFromFunctionFunction() == "string") // from value
System::assert(returnPropertyFromFunction() == "string") // from value
System::assert(returnPropertyFromFunctionFunction() == "string") // from value




