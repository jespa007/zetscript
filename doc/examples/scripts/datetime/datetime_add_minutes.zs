var now = DateTime::now();
Console::outln("now => {0}", now);
now.addMinutes(10);
Console::outln("after +10 minutes from now => {0}", now);