#pragma once
#include <eoslib/types.hpp>
#include <eoslib/message.hpp>
#include <eoslib/datastream.hpp>
#include <eoslib/raw_fwd.hpp>
#include <polleos.hpp>

using namespace CONTRACT_NAME;

namespace eosio { namespace raw {
    template<typename Stream>
    inline void pack( Stream& s, const option& value ) {
      raw::pack(s, value.name);
    }

    template<typename Stream>
    inline void unpack( Stream& s, option& value ) {
      raw::unpack(s, value.name);
    }

    template<typename Stream>
    inline void pack( Stream& s, const option_result& value ) {
      raw::pack(s, static_cast<const option&>(value));
      raw::pack(s, value.votes);
    }

    template<typename Stream>
    inline void unpack( Stream& s, option_result& value ) {
      raw::unpack(s, value.votes);
      raw::unpack(s, static_cast<option&>(value));
    }

    template<typename Stream, typename T>
    inline void pack( Stream& s, const T* arr, uint32_t size) {
      raw::pack(s, size);
      for (uint32_t i = 0; i < size; i++) {
        raw::pack(s, arr[i]);
      }
    }

    template<typename Stream, typename T>
    inline void unpack( Stream& s, T* arr, uint32_t size) {
      uint32_t real_size;
      raw::unpack(s, real_size);
      assert(real_size <= size, "Trying to unpack bigger array than buffer allows");
      for (uint32_t i = 0; i < real_size; i++) {
        raw::unpack(s, arr[i]);
      }
    }

    template<typename Stream>
    inline void pack( Stream& s, const multi_opt_poll_msg& value ) {
      raw::pack(s, value.question);
      raw::pack(s, value.options, 4);
    }

    template<typename Stream>
    inline void unpack( Stream& s, multi_opt_poll_msg& value ) {
      raw::unpack(s, value.question);
      raw::unpack(s, value.options, 4);
    }

    template<typename Stream>
    inline void pack( Stream& s, const multi_opt_poll& value ) {
      raw::pack(s, value.question);
      raw::pack<Stream, option_result>(s, value.results, 4);
    }

    template<typename Stream>
    inline void unpack( Stream& s, multi_opt_poll& value ) {
      raw::unpack(s, value.question);
      raw::unpack(s, value.results, 4);
    }
  }
}

#include <eoslib/raw.hpp>
namespace eosio {
  template<typename Type>
  Type current_message_ex() {
    uint32_t size = message_size();
    char* data = (char *)eosio::malloc(size);
    assert(data && read_message(data, size) == size, "error reading message");
    Type value;
    eosio::raw::unpack(data, size, value);
    eosio::free(data);
    return value;
  }

//  template<>
//  option current_message<option>() {
//    return current_message_ex<option>();
//  }

  template<>
  multi_opt_poll_msg current_message<multi_opt_poll_msg>() {
    return current_message_ex<multi_opt_poll_msg>();
  }

//  template<>
//  option_result current_message<option_result>() {
//    return current_message_ex<option_result>();
//  }

  template<>
  multi_opt_poll current_message<multi_opt_poll>() {
    return current_message_ex<multi_opt_poll>();
  }

  void print_ident(int n){while(n-->0){print("  ");}};

  void dump(const option_result& value, int tab=0) {
    print_ident(tab);print("name:[");prints_l(value.name.get_data(), value.name.get_size());print("]\n");
    print_ident(tab);print("votes:[");printi(value.votes);print("]\n");
  }

  void dump(const option& value, int tab=0) {
    print_ident(tab);print("name:[");prints_l(value.name.get_data(), value.name.get_size());print("]\n");
  }

  void dump(const multi_opt_poll_msg& value, int tab=0) {
    print_ident(tab);print("key:[");prints_l(value.question.get_data(), value.question.get_size());print("]\n");
    print_ident(tab);print("value:[");print("\n");
    for (int i = 0; i < 4; i++) {
      eosio::dump(value.options[i], tab + 1);
    }
    print_ident(tab);print("]\n");
  }
  void dump(const multi_opt_poll& value, int tab=0) {
    print_ident(tab);print("key:[");prints_l(value.question.get_data(), value.question.get_size());print("]\n");
    print_ident(tab);print("value:[");print("\n");
    for (int i = 0; i < 4; i++) {
      eosio::dump(value.results[i], tab + 1);
    }
    print_ident(tab);print("]\n");
  }


//  void dump(const multi_opt_poll& value, int tab=0) {
//    print_ident(tab);print("key:[");prints_l(value.key.get_data(), value.key.get_size());print("]\n");
//    print_ident(tab);print("value:[");print("\n"); eosio::dump(value.value, tab+1);print_ident(tab);print("]\n");
//  }

} //eosio

