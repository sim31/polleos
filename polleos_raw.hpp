#pragma once
#include <eoslib/types.hpp>
#include <eoslib/message.hpp>
#include <eoslib/datastream.hpp>
#include <eoslib/raw_fwd.hpp>
#include <polleos.hpp>

using namespace polleos;

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
      raw::unpack(s, static_cast<option&>(value));
      raw::unpack(s, value.votes);
    }

    template<typename Stream, typename T>
    inline void pack( Stream& s, const T* arr, uint32_t size) {
      unsigned_int size1(size);
      raw::pack(s, size1);
      for (uint32_t i = 0; i < size; i++) {
        raw::pack(s, arr[i]);
      }
    }

    template<typename Stream, typename T>
    inline uint32_t unpack( Stream& s, T* arr, uint32_t size) {
      unsigned_int real_size;
      raw::unpack(s, real_size);
      //eosio::print("real_size: ", (uint32_t)real_size, "\n");
      assert((uint32_t)real_size <= size, "Trying to unpack bigger array than buffer allows");
      for (uint32_t i = 0; i < real_size; i++) {
        raw::unpack(s, arr[i]);
      }
      return real_size;
    }

    template<typename T>
    inline bytes pack(const T* arr, uint32_t size) {
      eosio::datastream<size_t> s;
      eosio::raw::pack<eosio::datastream<size_t>, T>(s, arr, size);
      bytes b;
      b.len = s.tellp();
      b.data = (uint8_t*)eosio::malloc(b.len);

      if( b.len ) {
         eosio::datastream<char*>  ds( (char*)b.data, b.len );
         eosio::raw::pack<eosio::datastream<char*>, T>(ds, arr, size);
      }
      return b;
    }

    template<typename Stream>
    inline void pack( Stream& s, const opt_poll_msg& value ) {
      raw::pack(s, value.question);
      raw::pack<Stream, option>(s, value.options, value.options_len);
    }

    template<typename Stream>
    inline void unpack( Stream& s, opt_poll_msg& value ) {
      raw::unpack(s, value.question);
      value.options_len = raw::unpack(s, value.options, max_options);
    }

    template<typename Stream>
    inline void pack( Stream& s, const opt_token_poll_msg& value) {
      raw::pack(s, static_cast<const opt_poll_msg&>(value));
      raw::pack(s, value.token);
      raw::pack(s, value.stake_weighted);
    }

    template<typename Stream>
    inline void unpack( Stream& s, opt_token_poll_msg& value) {
      raw::unpack(s, static_cast<opt_poll_msg&>(value));
      raw::unpack(s, value.token);
      raw::unpack(s, value.stake_weighted);
    }

    template<typename Stream>
    inline void pack( Stream& s, const opt_poll& value ) {
      raw::pack(s, value.id);
      raw::pack(s, value.question);
      raw::pack<Stream, option_result>(s, value.results, value.results_len);
      raw::pack(s, value.is_token_poll);
      raw::pack(s, value.token);
      raw::pack(s, value.stake_weighted);
    }

    template<typename Stream>
    inline void unpack( Stream& s, opt_poll& value ) {
      raw::unpack(s, value.id);
      raw::unpack(s, value.question);
      value.results_len = raw::unpack(s, value.results, max_options);
      raw::unpack(s, value.is_token_poll);
      raw::unpack(s, value.token);
      raw::unpack(s, value.stake_weighted);
    }

    template<typename Stream>
    inline void pack( Stream& s, const opt_vote& value ) {
      raw::pack(s, value.id);
      raw::pack(s, value.voter);
      raw::pack(s, value.option);
    }

    template<typename Stream>
    inline void unpack( Stream& s, opt_vote& value ) {
      raw::unpack(s, value.id);
      raw::unpack(s, value.voter);
      raw::unpack(s, value.option);
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
  opt_poll_msg current_message<opt_poll_msg>() {
    return current_message_ex<opt_poll_msg>();
  }

  template<>
  opt_token_poll_msg current_message<opt_token_poll_msg>() {
    return current_message_ex<opt_token_poll_msg>();
  }

//  template<>
//  option_result current_message<option_result>() {
//    return current_message_ex<option_result>();
//  }

  template<>
  opt_poll current_message<opt_poll>() {
    return current_message_ex<opt_poll>();
  }

  template<>
  opt_vote current_message<opt_vote>() {
    return current_message_ex<opt_vote>();
  }


  void print_ident(int n){while(n-->0){print("  ");}};

  void dump(const option_result& value, int tab=0) {
    print_ident(tab);print("name:[");prints_l(value.name.get_data(), value.name.get_size());print("]\n");
    print_ident(tab);print("votes:[");printi(value.votes);print("]\n");
  }

  void dump(const option& value, int tab=0) {
    print_ident(tab);print("name:[");prints_l(value.name.get_data(), value.name.get_size());print("]\n");
  }

  void dump(const opt_poll_msg& value, int tab=0) {
    print_ident(tab);print("key:[");prints_l(value.question.get_data(), value.question.get_size());print("]\n");
    print_ident(tab);print("value:[");print("\n");
    for (int i = 0; i < 4; i++) {
      eosio::dump(value.options[i], tab + 1);
    }
    print_ident(tab);print("]\n");
  }
  void dump(const opt_poll& value, int tab=0) {
    print_ident(tab);print("key:[");prints_l(value.question.get_data(), value.question.get_size());print("]\n");
    print_ident(tab);print("value:[");print("\n");
    for (int i = 0; i < 4; i++) {
      eosio::dump(value.results[i], tab + 1);
    }
    print_ident(tab);print("]\n");
  }


//  void dump(const opt_poll& value, int tab=0) {
//    print_ident(tab);print("key:[");prints_l(value.key.get_data(), value.key.get_size());print("]\n");
//    print_ident(tab);print("value:[");print("\n"); eosio::dump(value.value, tab+1);print_ident(tab);print("]\n");
//  }

} //eosio

