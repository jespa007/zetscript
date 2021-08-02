class A{
	_in(k){
		if(k<10 || k > 20){
			return false;
		}
		return true;
	}
	
}

var b1=false
var b2=true
var s1="abc"

var i=10
var test_in=[
	"0 in [3,2,1,1,0]"
	,"20 in [3,2,1,1,0]"
	,"s1 in [3,2,1,1,\"abcd\",0]"
	,"s1 in [3,2,1,1,\"abc\",0]"
	,"b1 in [3,2,1,1,\"abc\",0,true]"
	,"b2 in [3,2,1,1,\"abc\",0,false]"
	,"b1 in [3,2,1,1,\"abc\",0,false]"
	,"b2 in [3,2,1,1,\"abc\",0,true]"
	,"\"a\" in {k:'a',a:0}"
	,"\"b\" in {k:'a',a:0}"
	//0 in 10 // <-- invalid (2n is not iterator)
	,"\"abc\" in \"aaaabcdef\""
	,"\"abce\" in \"aaaabcdef\""
	,"'a' in \"jdkjakdkk\""
	,"'b' in \"jdkjakdkk\""
	,"i in new A()"
	,"0 in new A()"
];


for(var v in test_in){
	Console::outln("{0}:{1}",v,System::eval(v))
}


