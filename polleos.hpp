#pragma once

#include <eoslib/eos.hpp>
#include <eoslib/db.hpp>
#include <eoslib/string.hpp>

#define CONTRACT_NAME polleos
#define CONTRACT_NAME_UINT64 N(polleos)

namespace CONTRACT_NAME {

  struct PACKED (option) {
    eosio::string name;
  };

  struct PACKED (option_result) : option {
    uint64_t votes;
  };

  //@abi action newmultiopt
  struct PACKED (multi_opt_poll_msg) {
    eosio::string question;
    option options[4];
  };

  //@abi table
  struct PACKED (multi_opt_poll) {
    eosio::string question;
    option_result results[4];
  };

}

