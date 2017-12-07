#pragma once

#include <eoslib/eos.hpp>
#include <eoslib/db.hpp>
#include <eoslib/string.hpp>

#define CONTRACT_NAME polleos2
#define CONTRACT_NAME_UINT64 N(polleos2)

namespace CONTRACT_NAME {

  struct PACKED (option) {
    eosio::string name;

    option(eosio::string name) : name(name) {}
    option() {}
  };

  struct PACKED (option_result) : option {
    uint64_t votes = 0;

    option_result(eosio::string name, uint64_t votes) : option(name), votes(votes ) {}
    option_result() {}
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

    multi_opt_poll() {}
  };

}

