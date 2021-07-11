const CASE_3=3;

function test_switch(a){

	switch(a){
	default:
		Console::outln("default ("+a+")")
		break
	case - 1:
		Console::outln("-1")
	case 0:
	case 1:
		Console::outln("case 0,1")
	case 2:
		{
		var b
		Console::outln("case 2")
		}
	case - CASE_3:{
		var c,d,e=0
		Console::outln("case 3 {0} {1}",a,c)
		break
	}
	case 4:
		Console::outln("case 4")
		break
	case 5:
		{
			Console::outln("case 5")
			break
		}
	}
	
}
Console::outln("--")
test_switch(-2)
Console::outln("--")
test_switch(-1)
Console::outln("--")
test_switch(0)
Console::outln("--")
test_switch(3)
Console::outln("--")
test_switch(4)
Console::outln("--")

var s="hello"

switch(s){
default:
	Console::outln("default-string")
	break
case "hello":
	Console::outln("hello1")
	break
case "bye":
	Console::outln("bye1")
	break
	
}




