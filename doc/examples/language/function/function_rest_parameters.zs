function number(str,...args) {
  var number = 0;
  for (var arg in args) {
  		number += arg;
  }
  return str+number;
}

var res=number("Total number is = ",1, 10, 100, 1000, 10000);

Console::outln(res)