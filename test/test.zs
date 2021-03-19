var s1="123";
var s2="456";
var s3="456---678";

var v=s3.split("---")

System::println("----")
for(var i=0; i < s1.size(); i++){
	//System::println(s1[i]);
}

System::println("----")
s1[1]='b'
for(var i=0; i < s1.size(); i++){
	System::println(s1[i]);
}


System::println("----")

// test contains char
if(s1.contains('b')){
	System::println("b exist");
}

// test contains string
if(s1.contains("12")){
	System::println("12 exist");
}else{
	System::println("12 NOT exist");
}


System::println("----")
s1.eraseAt(1);

if(s1.contains('b') == false){
	System::println("b NOT exist");
}
else{
	System::println("b exist");
}

System::println("----")

s1.clear();

for(var i=0; i < s1.size(); i++){
	System::println(s1[i]);
}


//var v=s3.split("---")

for(var i=0; i < v.size(); i++){
	System::println(v[i]);
}

