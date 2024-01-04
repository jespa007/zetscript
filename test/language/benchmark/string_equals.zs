var start = System::clock()

var count = 0
for (var i=0; i < 1000000; i++) {
  if ("abc" == "abc") {count = count + 1}
  if ("a slightly longer string" ==
      "a slightly longer string") {count = count + 1}
  if ("a significantly longer string but still not overwhelmingly long string" ==
      "a significantly longer string but still not overwhelmingly long string") {count = count + 1}

  if ("" == "abc") {count = count + 1}
  if ("abc" == "abcd") {count = count + 1}
  if ("changed one character" == "changed !ne character") {count = count + 1}
  if ("123" == "123") {count = count + 1} // <-- if ("123" == 123)  it causes an string vs integer comparison and throws internal error but it doesn't shows
  if ("a slightly longer string" ==
      "a slightly longer string!") {count = count + 1}
  if ("a slightly longer string" ==
      "a slightly longer strinh") {count = count + 1}
  if ("a significantly longer string but still not overwhelmingly long string" ==
      "another") {count = count + 1}
}

Console::outln(count)
Console::outln("elapsed: {0}",System::clock() - start)
