var object={
    encoding : "UTF-8"
    ,number: 3.34E-5
    ,plug_ins : [
        "python",
        "c++",
        "ruby"
      ]
    ,indent : { "length" : 3, "use_space": true }
}

Console::outln("serialize :")
Console::outln()
Console::outln(Json::serialize(object))
Console::outln()
Console::outln("serialize with format : ")
Console::outln()
Console::outln(Json::serialize(object,true))