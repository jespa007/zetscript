var s1="123";
var s2="456";
var s3="456---678";

var v=s3.split("---")


for(var i=0; i < s1.size(); i++){
	Console::outln(s1[i]);
}


s1[1]='b'
for(var i=0; i < s1.size(); i++){
	Console::outln(s1[i]);
}


// test contains char
if(s1.contains('b')){
	Console::outln("b exist");
}

// test contains string
if(s1.contains("12")){
	Console::outln("12 exist");
}else{
	Console::outln("12 NOT exist");
}

s1.eraseAt(1);

if(s1.contains('b') == false){
	Console::outln("b NOT exist");
}
else{
	Console::outln("b exist");
}

s1.clear();

for(var i=0; i < s1.size(); i++){
	Console::outln(s1[i]);
}


for(var i=0; i < v.size(); i++){
	Console::outln(v[i]);
}


