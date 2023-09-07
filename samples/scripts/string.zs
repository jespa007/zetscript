var s1="123";
var s2="456";

Console::outln("----")
Console::outln("Test concatenation: s1+s2 ==> \"{0}\"",s1+s2)

Console::outln("----")
Console::outln("Iteration string 's1' as normal 'for':")
for(var i=0; i < s1.length; i++){
	Console::outln("s1[{0}]:{1}",i,s1[i]);
}
Console::outln("----")
Console::outln("Iteration string 's1' as 'for-in' value:")
for(var v in  s1){
	Console::outln("v:{0}",v);
}
Console::outln("----")
Console::outln("Iteration string 's1' as 'for-in' pair key-pair:")
for(var k,v in  s1){
	Console::outln("s1[{0}]:{1}",k,v);
}
Console::outln("----")
Console::outln("Test split: \"456---678\".split(\"---\") ==> {0}","456---678".split("---"))
Console::outln("----")
Console::outln("Replace s1[1] by char 'b':")
s1[1]='b'
Console::outln("After modifed s1 ==> \"{0}\"",s1)
Console::outln("----")
Console::outln("Test 's1' contains char 'b': s1.contains('b') ==> {0}",s1.contains('b'))
Console::outln("----")
Console::outln("Test 's1' contains string \"1b\": s1.contains(\"1b\") ==> {0}",s1.contains("1b"))
Console::outln("----")
Console::outln("Erase element s1[1]:")
s1.eraseAt(1);
Console::outln("----")
Console::outln("Test 's1' NOT contains char 'b': s1.contains('b') ==> {0}",s1.contains('b'))

