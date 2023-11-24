
function test_conditional(_n){
	if(_n<10){
		return "_n<10";
	}else if(_n<20){
		return "_n<20";
	}else{
		return "_n>=20";
	}
}

System::assert(test_conditional(5)=="_n<10","!= _n<10");
System::assert(test_conditional(15)=="_n<20","!= _n<20");
System::assert(test_conditional(25)=="_n>=20","!= _n>=20");

