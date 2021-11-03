class A{
	var b=10
	A(){
		Console::outln("5. test this member call")
		this.member_function()
		
		Console::outln("6. test this anonymous call")
		this.anonymous_function=function(){
			Console::outln("anonymous member function  this.b:{0} ok!",this.b)
		}
		this.anonymous_function()
		
		Console::outln("7. test indirect member call throught var")
		var member_function=this.member_function
		member_function()
		
	}
	
	member_function(){
		Console::outln("member function this.b:{0} ok!",this.b)
	}
}

 
Console::outln("1. test direct call")
function global_function(){
	Console::outln("global_function:ok")
}

global_function()

Console::outln("2. test indirect call through var ")
var var_global_function=global_function
var_global_function()

Console::outln("test anonymous call through var")
var anonymous_function=function(){
	Console::outln("anonymous_function: ok")
}

anonymous_function()

Console::outln("3. test anonymous call itself")
function(){
	Console::outln("anonymous itself: ok")
}()

{
	function local_function(){
		Console::outln("local function:ok")
	}
	
	local_function()
	
	var var_local_function=local_function
	
	var_local_function()
}

var a=new A()

Console::outln("4. Call function member")
a.member_function()

var var_function_member=a.function_member //--> somehow increment n shared pointers due, 

var_function_member();

delete a //--> after delete object var_function_member should be still alive

var_function_member();



