function test_switch(a){
	

	switch(a){
	default:
		System::println("default ("+a+")")
		break
	case - 1:
		System::println("-1")
	case 0:
	case 1:
		System::println("case 0,1")
	case 2:
		{
		var b
		System::println("case 2")
		}
	case 3:{
		var c,d,e=0
		System::println("case 3 {0} {1}",a,b,c)
		break
	}
	case 4:
		System::println("case 4")
		break
	case 5:
		{
			System::println("case 5")
			break
		}
	}
	
}
System::println("--")
test_switch(-2)
System::println("--")
test_switch(-1)
System::println("--")
test_switch(0)
System::println("--")
test_switch(3)
System::println("--")
test_switch(4)
System::println("--")

var s="hello"

switch(s){
default:
	System::println("default-string")
	break
case "hello":
	System::println("hello1")
	break
case "bye":
	System::println("bye1")
	break
	
}




