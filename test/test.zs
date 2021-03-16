var s1="123";
var s2="456";


System::println("----")
for(var i=0; i < s1.size(); i++){
	System::println(s1[i]);
}

System::println("----")
s1[1]='b'
for(var i=0; i < s1.size(); i++){
	System::println(s1[i]);
}


System::println("----")

// test contains char
if(s1.contains('2')){
	System::println("2 exist");
}

// test contains string
if(s1.contains("12")){
	System::println("12 exist");
}


System::println("----")
s1.eraseAt(1);

if(s1.contains('2') == false){
	System::println("2 not exist");
}

System::println("----")

s1.clear();

for(var i=0; i < s1.size(); i++){
	System::println(s1[i]);
}

