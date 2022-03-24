const CASE_3=3

function test_switch(a){
	var v=[];
	switch(a){
	default:
		Console::outln("default ("+a+")")
		v.push(a);
		break
	case - 1:
		v.push(-1);
		Console::outln("case -1")
	case 0:
		Console::outln("case 0")
		v.push(0);
	case 1:
		v.push(1);
		Console::outln("case 1")
	case 2:
		{
		var b
		v.push(2);
		Console::outln("case 2")
		}
	case - CASE_3:{
		var d,e=0
		v.push(-CASE_3);
		Console::outln("case -3")
		break
	}
	case 4:
		v.push(4);
		Console::outln("case 4")
		break
	case 5:
		{
			v.push(5);
			Console::outln("case 5")
			break
		}
	}
	return v
}
Console::outln("--")
var v=test_switch(-2)
System::assert(Vector::equal(v,[-2]),"v!=[-2]")
Console::outln(v)
Console::outln("--")
v=test_switch(-1)
Console::outln(v)
System::assert(Vector::equal(v,[-1,0,1,2,-3]),"v!=[-1,0,1,2,-3]")
Console::outln("--")
v=test_switch(0)
Console::outln(v)
System::assert(Vector::equal(v,[0,1,2,-3]),"v!=[0,1,2,-3]")
Console::outln("--")
v=test_switch(3)
Console::outln(v)
System::assert(Vector::equal(v,[3]),"v!=[3]")
Console::outln("--")
v=test_switch(4)
Console::outln(v)
System::assert(Vector::equal(v,[4]),"v!=[4]")


var s="hello"
var r="";
switch(s){
default:
	Console::outln("default-string")
	r=s
	break
case "hello":
	Console::outln("hello")
	r=s
	break
case "bye":
	Console::outln("bye1")
	r=s
	break
	
}

System::assert(r=="hello","r!=\"hello\"")


var obj={
    a:0
    ,b:3
    ,c:3
}

var g=-CASE_3
var h;

//---> should fail (switch must eval CONSTANT values to avoid ambiguty
switch(g){
case 0:
	Console::outln("0")
	h=g
case CASE_3:
    Console::outln("CASE_3")
    h=g
    break;
case obj.a : 
    Console::outln("a {0}",g)
    h=g
    break;
case obj.b : 
    Console::outln("b")
    h=g
    break;
default:
    Console::outln("default: {0}",g)
    h=g
    break;
}

System::assert(h==-CASE_3,"h!=-CASE_3")

