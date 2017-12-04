#pragma once
#include <eoslib/types.hpp>
#include <eoslib/message.hpp>
#include <eoslib/datastream.hpp>
#include <eoslib/raw_fwd.hpp>

namespace eosio { namespace raw {
   template<typename Stream> inline void pack( Stream& s, const option& value ) {
      raw::pack(s, value.name);
   }
   template<typename Stream> inline void unpack( Stream& s, option& value ) {
      raw::unpack(s, value.name);
   }
   template<typename Stream> inline void pack( Stream& s, const multi_opt_poll_msg& value ) {
      raw::pack(s, value.key);
      raw::pack(s, value.value);
   }
   template<typename Stream> inline void unpack( Stream& s, multi_opt_poll_msg& value ) {
      raw::unpack(s, value.key);
      raw::unpack(s, value.value);
   }
   template<typename Stream> inline void pack( Stream& s, const option_result& value ) {
      raw::pack(s, value.votes);
      raw::pack(s, static_cast<const option&>(value));
   }
   template<typename Stream> inline void unpack( Stream& s, option_result& value ) {
      raw::unpack(s, value.votes);
      raw::unpack(s, static_cast<option&>(value));
   }
   template<typename Stream> inline void pack( Stream& s, const multi_opt_poll& value ) {
      raw::pack(s, value.key);
      raw::pack(s, value.value);
   }
   template<typename Stream> inline void unpack( Stream& s, multi_opt_poll& value ) {
      raw::unpack(s, value.key);
      raw::unpack(s, value.value);
   }
} }

#include <eoslib/raw.hpp>
namespace eosio {
   void print_ident(int n){while(n-->0){print("  ");}};
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
   void dump(const option& value, int tab=0) {
      print_ident(tab);print("name:[");prints_l(value.name.get_data(), value.name.get_size());print("]\n");
   }
   template<>
   option current_message<option>() {
      return current_message_ex<option>();
   }
   void dump(const multi_opt_poll_msg& value, int tab=0) {
      print_ident(tab);print("key:[");prints_l(value.key.get_data(), value.key.get_size());print("]\n");
      print_ident(tab);print("value:[");print("\n"); eosio::dump(value.value, tab+1);print_ident(tab);print("]\n");
   }
   template<>
   multi_opt_poll_msg current_message<multi_opt_poll_msg>() {
      return current_message_ex<multi_opt_poll_msg>();
   }
   void dump(const option_result& value, int tab=0) {
      print_ident(tab);print("votes:[");printi128(&value.votes);print("]\n");
   }
   template<>
   option_result current_message<option_result>() {
      return current_message_ex<option_result>();
   }
   void dump(const multi_opt_poll& value, int tab=0) {
      print_ident(tab);print("key:[");prints_l(value.key.get_data(), value.key.get_size());print("]\n");
      print_ident(tab);print("value:[");print("\n"); eosio::dump(value.value, tab+1);print_ident(tab);print("]\n");
   }
   template<>
   multi_opt_poll current_message<multi_opt_poll>() {
      return current_message_ex<multi_opt_poll>();
   }
} //eosio

