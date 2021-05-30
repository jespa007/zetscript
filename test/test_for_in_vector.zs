var vec=[0,10,"k",true];

for(var v in vec){
	 Console::outln("v:{0}",v);
}

for(var k,v in vec){
	 Console::outln("vec[{0}]:{1}",k,v);
}
