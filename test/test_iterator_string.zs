var s="this is an string iterator"
Console::outln("size:"+s.size());
for(var it2=s.iter();!it2.end();it2++){
   var k,v;
   
   k,v=it2
   Console::outln("k:{0} v:{1}",k,v);
   Console::outln(it2); // will print the content
}