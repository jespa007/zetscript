class Test{

}

// defines member method Test::member1
function Test::member1(){
	Console::outln("call from member1()")
}


var test=new Test();
test.member1(); // member1 is defined so it can be called

