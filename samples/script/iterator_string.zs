var s="this is an string iterator"
for(var it=s.iter();!it.end();it++){
	var k,v;
	k,v=it.get();
	Console::outln("k:{0} v:{1} ",k,v);
}

