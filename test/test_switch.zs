function test_switch(a){
	

	switch(a){
	default:
		Console::println("default ("+a+")")
		break
	case - 1:
		Console::println("-1")
	case 0:
	case 1:
		Console::println("case 0,1")
	case 2:
		{
		var b
		Console::println("case 2")
		}
	case 3:{
		var c,d,e=0
		Console::println("case 3 {0} {1}",a,b,c)
		break
	}
	case 4:
		Console::println("case 4")
		break
	case 5:
		{
			Console::println("case 5")
			break
		}
	}
	
}
Console::println("--")
test_switch(-2)
Console::println("--")
test_switch(-1)
Console::println("--")
test_switch(0)
Console::println("--")
test_switch(3)
Console::println("--")
test_switch(4)
Console::println("--")

var s="hello"

switch(s){
default:
	Console::println("default-string")
	break
case "hello":
	Console::println("hello1")
	break
case "bye":
	Console::println("bye1")
	break
	
}




