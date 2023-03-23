var birth=new DateTime(1979,9,6,12,00,00)
Console::outln("birth => "+birth)
var now=DateTime::now()
var diff=now-birth;
Console::outln("years => "+Integer::parse(diff.days/365))