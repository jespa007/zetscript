var now = DateTime::now();
Console::outln("now => {0}", now);
now.addYears(3);
Console::outln("after +3 years from now => {0}", now);