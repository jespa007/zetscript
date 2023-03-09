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


Console::outln("result : "+ Json::serialize(object))