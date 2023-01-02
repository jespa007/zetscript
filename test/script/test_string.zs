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

// Test insertAt. In this case it inserts 'a' character at position 1 changing its current value "1b3" by "1ab3"
s1.insertAt(1,'a');
System::assert(s1=="1ab3","s1!=\"1ab3\"");

// Test eraseAt. In this case it erases position s1[1] changing its current value "1b3" by "13"
s1.eraseAt(1);
System::assert(s1=="1b3","s1!=\"1b3\"");

// Test indexOf. Get index first ocurrence of character 'b'
System::assert("abc".indexOf('b')==1,"\"abc\".indexOf('b')!=1");

// Test indexOf. Get index first ocurrence of string "world"
System::assert("hello world".indexOf("world")==6,"\"hello world\".indexOf(\"world\")!=6");

// Test clear
s1.clear();
System::assert(s1=="","s1!=\"\"");

// Test replace string "world" by "people"
System::assert("hello world world".replace("world","people")=="hello people people","\"hello world\".replace(\"world\",\"people\")!=\"hello people people\"")



