var s="this is an string iterator"
Console::outln("size:"+s.size());
for(var it2=s.iter();!it2.end();it2++){
   //var v=it2
   //Console::outln(v);
   Console::outln(it2); // will print the content
}