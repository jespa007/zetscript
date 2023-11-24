var now = DateTime::now();
Console::outln("now => {0}", now);
now.addDays(36);
Console::outln("after +36 days from now => {0}", now);