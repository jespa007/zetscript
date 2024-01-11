
class Entity{
    // Member property 'name' (read and write)
    name{
        // Implement metamethod _set (aka '=')
        _set(_name){
            this.__name__=_name;
        }

        // Implement metamethod getter
        _get(){
            return this.__name__;
        }
    }
}

class Player extends Entity{
    constructor(){
        // Calls constructor base (i.e Entity::constructor)
        this.name="Player";
    }
}

var player=new Player();


