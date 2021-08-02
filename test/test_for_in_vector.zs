var vec=[
    0
    ,10
    ,"k"
    ,true
    ,{
		k1:0
		,k2:2
	}];

for(var v in vec){
	 Console::outln("v:{0}",v);
}

for(var k,v in vec){
	 Console::outln("vec[{0}]:{1}",k,v);
}
