var s1="123";
var s2="456";
var s3="456---678";

var v=s3.split("---")
System::assert(v.size()==2,"v.size() != 2");

System::assert(v[0]=="456","v[0] != \"456\"");
System::assert(v[1]=="678","v[1] != \"678\"");

// assert that we have 123
System::assert(s1=="123","s1!=\"123\"");

// Test modify character. In this case it has position s1[1] changing its current value  "123" by "1b3"
s1[1]='b'
System::assert(s1=="1b3","s1!=\"1b3\"");

// Test eraseAt. In this case it erases position s1[1] changing its current value "1b3" by "13"
s1.eraseAt(1);
System::assert(s1=="13","s1!=\"13\"");

// Test clear
s1.clear();
System::assert(s1=="","s1!=\"\"");

// Test replace string "hello world" by "hello people"
System::assert("hello world".replace("world","people")=="hello people","\"hello world\".replace(\"world\",\"people\")!=\"hello people\"")



