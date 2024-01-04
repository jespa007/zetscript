const CASE_3=3

function test_switch(a){
	var v=[];
	switch(a){
	default:
		v.push(a);
		break
	case - 1:
		v.push(-1);
	case 0:
		v.push(0);
	case 1:
		v.push(1);
	case 2:
		{
		var b
		v.push(2);
		}
	case - CASE_3:{
		var d,e=0
		v.push(-CASE_3);
		break
	}
	case 4:
		v.push(4);
		break
	case 5:
		{
			v.push(5);
			break
		}
	}
	return v
}
var v=test_switch(-2)
System::assert(v==[-2],"v!=[-2]")

v=test_switch(-1)
System::assert(v==[-1,0,1,2,-3],"v!=[-1,0,1,2,-3]")
v=test_switch(0)

System::assert(v==[0,1,2,-3],"v!=[0,1,2,-3]")
v=test_switch(3)

System::assert(v==[3],"v!=[3]")
v=test_switch(4)

System::assert(v==[4],"v!=[4]")

var s="hello"
var r="";
switch(s){
default:
	r=s
	break
case "hello":
	r=s
	break
case "bye":
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
	h=g
case CASE_3:
    h=g
    break;
case obj.a : 
    h=g
    break;
case obj.b : 
    h=g
    break;
default:
    h=g
    break;
}

System::assert(h==-CASE_3,"h!=-CASE_3")

