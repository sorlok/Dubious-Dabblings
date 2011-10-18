//Helper classes
class SomeClass {
public:
	float value;
};

//Our functions
extern "C" {
  SomeClass* make_new_class() {
	  SomeClass* res = new SomeClass();
	  res->value = 10.0;
	  return res;
  }

  float get_nonclass_value() {
	  return 5.0;
  }

  float get_class_value(SomeClass* item) {
	  return item->value;
  }

}

