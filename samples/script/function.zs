var test=0

class A{
	var b=10
	A(){
		
		test=this.member_function(test)
		System::assert(test==7,"test!=7")		
		
		this.anonymous_function=function(test){
			return test+1;
		}
		
		test=this.anonymous_function(test)
		System::assert(test==8,"test!=8")
		
		var member_function=this.member_function
		test=member_function(test)
		System::assert(test==9,"test!=9")
		
	}
	
	member_function(test){
		return test+1
	}
}



function global_function(test){
	return test+1
}

test=global_function(test)
System::assert(test==1,"test!=1")

var var_global_function=global_function
test=var_global_function(test)
System::assert(test==2,"test!=2")

var anonymous_function=function(test){
	return test+1
}

test=anonymous_function(test)
System::assert(test==3,"test!=3")

test=function(test){
	return test+1
}(test)
System::assert(test==4,"test!=4")

{
	function local_function(test){
		return test+1
	}
	System::assert(test==5,"test!=5")
	
	test=ocal_function(test)
	
	var var_local_function=local_function
		
	test=var_local_function(test)
	System::assert(test==5,"test!=6")
}


var a=new A()
test=a.member_function(test)
System::assert(test==10,"test!=10")

var var_member_function=a.member_function  
test=var_member_function(test);
System::assert(test==11,"test!=11")




