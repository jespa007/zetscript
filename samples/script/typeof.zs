class A{}

var a=new A()

var test_typeof=[
	"typeof 0"
	,"typeof 1.0"
	,"typeof true"
	,"typeof [0,1,2]"
	,"typeof {a:0,b:1}"
	,"typeof a"
];


for(var i in test_typeof){
	Console::outln("{0}:{1}",i,System::eval(i))
}


