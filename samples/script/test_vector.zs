var v1=[1,2]
var v2=[4,5]
var v4=["s1","s2","s3","s4"]

v1.push(3);

for(var i=0; i < v1.size(); i++){
	Console::outln(v1[i]);
}

var item=v1.pop();

for(var i=0; i < v1.size(); i++){
	Console::outln(v1[i]);
}

Console::outln("---")
Console::outln(item)
Console::outln("---")

//Console::outln("---")

v2.insertAt(1,"string")

for(var i=0; i < v2.size(); i++){
	Console::outln(v2[i]);
}

Console::outln("---")

v2.eraseAt(1)

for(var i=0; i < v2.size(); i++){
	Console::outln(v2[i]);
}

Console::outln("---")

var v3=v1+v2

for(var i=0; i < v3.size(); i++){
	Console::outln(v3[i]);
}

Console::outln("---")

v3.clear()

for(var i=0; i < v3.size(); i++){
	Console::outln(v3[i]);
}

Console::outln("---")

Console::outln(v4.join('_'))
