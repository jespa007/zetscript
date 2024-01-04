var v1=[1,2]
var v2=[4,5]
var v4=["s1","s2","s3","s4"]

System::assert(v4.length==4,"v4.length!=4")

//check push
v1.push("s");
var k_result=0
var v_result=""
for(var k,v in v1){
	k_result+=k
	v_result+=v;
}

System::assert(k_result==(0+1+2),   "v1 (1) k_result != "+(0+1+2));
System::assert(v_result=="12s",		"v1 (1) v_result != \"12s\"");

//check pop
v1.pop();
k_result=0
v_result=""
for(var k,v in v1){
	k_result+=k
	v_result+=v;
}


System::assert(k_result==1,   "v1 (2) k_result != 1");
/*System::assert(v_result=="12","v1 (2)  v_result != \"12\"");



//check insertAt
v2.insertAt(1,"string")
k_result=0
v_result=""
for(var k,v in v2){
	k_result+=k
	v_result+=v;
}

System::assert(k_result==(0+1+2),   "v2 (1) k_result != "+(0+1+2));
System::assert(v_result=="4string5","v2 (1) v_result != \"4string5\"");

//check eraseAt
v2.eraseAt(1)
k_result=0
v_result=""
for(var k,v in v2){
	k_result+=k
	v_result+=v;
}

System::assert(k_result==1,   "v2 (2) k_result != 1");
System::assert(v_result=="45","v2 (2) v_result != \"45\"");


// check concatenation
var v3=v1+v2
k_result=0
v_result=""
for(var k,v in v3){
	k_result+=k
	v_result+=v;
}

System::assert(k_result==(0+1+2+3), "v3 (1) k_result != "+(0+1+2+3));
System::assert(v_result=="1245",	"v3 (1) v_result != \"1245\"");


// check clear
v3.clear()
k_result=0
v_result=""
for(var k,v in v3){
	k_result+=k
	v_result+=v;
}

System::assert(k_result==0, "v3 (2) k_result != 0");
System::assert(v_result=="","v3 (2) v_result != \"\"");


// check join
k_result=0
v_result=0
for(var k,v in v4.join('_')){
	k_result+=k
	v_result+=v;
}

System::assert(k_result==(0+1+2+3+4+5+6+7+8+9+10), 		   			   "v4 (1) k_result != "+(0+1+2+3+4+5+6+7+8+9+10));
System::assert(v_result==('s'+'1'+'_'+'s'+'2'+'_'+'s'+'3'+'_'+'s'+'4'),"v4 (1) v_result != "+('s'+'1'+'_'+'s'+'2'+'_'+'s'+'3'+'_'+'s'+'4'));
*/
