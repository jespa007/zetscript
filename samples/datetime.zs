var now=DateTime::now(); // now
var utc=DateTime::utc(); // utc

Console::outln("utc:{0}",utc)
Console::outln("now:{0}",now)

now.addDays(1)
Console::outln("Added one day:{0}",now)

now.addMonths(1)
Console::outln("Added one month:{0}",now)

now.addYears(1)
Console::outln("Added one year:{0}",now)