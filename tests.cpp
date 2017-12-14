#include <eoslib/eos.hpp>
#include <eoslib/string.hpp>
#include "vector.hpp"

using namespace eosio;

void test_vector() {
//  vector<string> v;
//  assert(v.get_size() == 0, "Default constructed vector not empty");

//  v.reserve(10);
//  assert(v.get_capacity() == 10, "Vector reserve did not result in specified capacity");

//  string str = "test str";
//  v.push_back(str);
//  assert(v.get_size() == 1, "Size did not increase after push_back");
//  assert(v[0] == str, "operator[] did not return pushed back string");

  //string str[2];
  //varray v(2);
  string* str = new string("aaa");
}

extern "C" {

  void init()  {
  }

  void apply( unsigned long long code, unsigned long long action ) {
    eosio::print("==> CONTRACT: ", code, " ", action, "\n");

    test_vector();

    eosio::print("test_vector exited");

  }

}
