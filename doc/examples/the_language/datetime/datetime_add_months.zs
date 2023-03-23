var now = DateTime::now();
Console::outln("now => {0}", now);
now.addMonths(10);
Console::outln("after +10 months from now => {0}", now);