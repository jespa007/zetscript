const obj = {
  _get prop() {
    return this.__prop__;
  },
  _set prop(value) {
    this.__prop__ = value * 2;
  },
};

