#pragma once

#include <eoslib/eos.hpp>
#include <eoslib/db.hpp>

namespace polleos {

  struct PACKED(account) {
    const uint64_t key = N(account);
    uint64_t balance;

    account( uint64_t balance = 0) : balance(balance) {}
  };
   /**
   Assert statement to verify structure packing for account
   **/
   static_assert( sizeof(account) == sizeof(uint64_t)*2, "unexpected packing" );

   inline account get_account(token_name token, account_name owner) {
     account owned_account;
     load_i64(owner, token, N(account), &owner, sizeof(account));
     return owned_account;
   }
}
