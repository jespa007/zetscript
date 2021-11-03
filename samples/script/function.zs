var test=0

class A{
	var b=10
	/*A(){
		
		test=this.member_function(test)
		System::assert(test==5,"test!=5")		
		
		this.anonymous_function=function(test){
			return test+1;
		}
		
		test=this.anonymous_function(test)
		System::assert(test==6,"test!=6")
		
		var member_function=this.member_function
		test=member_function(test)
		System::assert(test==7,"test!=7")
		
	}*/
	
	member_function(test){
		return test+1
	}
}

/*

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
*/
//---> crash
/*{
	function local_function(){
		//Console::outln("local function:ok")
	}
	
	local_function()
	
	var var_local_function=local_function
		
	var_local_function()
}*/
//---> crash


var a=new A()
/*test=a.member_function(test)
System::assert(test==8,"test!=8")
*/
var var_function_member=a.function_member //--> somehow increment n shared pointers due, 
test=var_function_member(test);
/*System::assert(test==9,"test!=9")

delete a //--> after delete object var_function_member should be still alive

test=var_function_member(test);
System::assert(test==10,"test!=10")*/



