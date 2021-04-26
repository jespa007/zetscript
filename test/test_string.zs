var s1="123";
var s2="456";
var s3="456---678";

var v=s3.split("---")

Console::println("----")
for(var i=0; i < s1.size(); i++){
	//Console::println(s1[i]);
}

Console::println("----")
s1[1]='b'
for(var i=0; i < s1.size(); i++){
	Console::println(s1[i]);
}


Console::println("----")

// test contains char
if(s1.contains('b')){
	Console::println("b exist");
}

// test contains string
if(s1.contains("12")){
	Console::println("12 exist");
}else{
	Console::println("12 NOT exist");
}


Console::println("----")
s1.eraseAt(1);

if(s1.contains('b') == false){
	Console::println("b NOT exist");
}
else{
	Console::println("b exist");
}

Console::println("----")

s1.clear();

for(var i=0; i < s1.size(); i++){
	Console::println(s1[i]);
}


//var v=s3.split("---")

for(var i=0; i < v.size(); i++){
	Console::println(v[i]);
}

