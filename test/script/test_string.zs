var s1="123";
var s2="456";
var s3="456---678";

var v=s3.split("---")
System::assert(v.size()==2,"v.size() != 2");

System::assert(v[0]=="456","v[0] != \"456\"");
System::assert(v[1]=="678","v[1] != \"678\"");


// assert that we have 123
System::assert(s1=="123","s1!=\"123\"");
s1[1]='b'

// we have modified s1="123" by s1="1b3"
System::assert(s1=="1b3","s1!=\"1b3\"");

//we have erased position s1[1] so s1="1b3" it becomes s1="13"
s1.eraseAt(1);
System::assert(s1=="13","s1!=\"13\"");

s1.clear();
System::assert(s1=="","s1!=\"\"");



