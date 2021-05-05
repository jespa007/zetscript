var s="this is an string iterator"		
Console::outln("size:"+s.size());
var start=System::clock()
for(var it=s.iter();!it.end();it++){
	var k,v;
	k,v=it.get();
	Console::outln("k:{0} v:{1} ",k,v);
}

Console::outln("ellapsed it1:{0}ms",System::clock()-start);

start=System::clock()
for(var i=0;i < s.size();i++){
	Console::outln("k:{0} v:{1} ",i,s[i]);
}

Console::outln("ellapsed it2:{0}ms",System::clock()-start);

/*start=System::clock()
for(var k,v in s){
	Console::outln("k:{0} v:{1} ",k,v);
}
Console::outln("ellapsed it3:{0}ms",System::clock()-start);*/