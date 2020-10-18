function test_switch(a){
	

	switch(a){
	default:
		print("default ("+a+")")
		break
	case - 1:
		print("-1")
	case 0:
	case 1:
		print("case 0,1")
	case 2:
		print("case 2")
	case 3:
		print("case 3")
		break
	case 4:
		print("case 4")
		break
	case 5:
		print("case 5")
		break
	}
	
}
print("--")
test_switch(-2)
print("--")
test_switch(-1)
print("--")
test_switch(0)
print("--")
test_switch(3)
print("--")
test_switch(4)
print("--")

var s="hello"

switch(s){
default:
	print("default-string")
	break
case "hello":
	print("hello1")
	break
case "bye":
	print("bye1")
	break
	
}



