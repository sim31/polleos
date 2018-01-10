#pragma once

#include <eoslib/eos.hpp>
#include <eoslib/db.hpp>
#include <eoslib/string.hpp>

#define CONTRACT_NAME polleos
#define CONTRACT_NAME_UINT64 N(polleos)

namespace CONTRACT_NAME {

  typedef uint64_t poll_id;

  const size_t max_options = 32;

  struct PACKED (option) {
    eosio::string name;

    option(eosio::string name) : name(name) {}
    option() {}
  };

  struct PACKED (option_result) : option {
    uint64_t votes = 0;

    option_result(const eosio::string& name, uint64_t votes) : option(name), votes(votes ) {}
    option_result(const eosio::string& name) : option(name), votes(0) {}
    option_result(const option& opt) : option(opt), votes(0) {}
    option_result() {}
  };

  typedef option_result* results_array;

  //@abi action newoptpoll
  struct PACKED (opt_poll_msg) {
    eosio::string question;
    uint8_t options_len = 0;
    option options[max_options];

    bool is_valid() const {
      //Check if question and all options have descriptions
      if ( question.get_size() == 0 || options_len < 2)
        return false;
      for (int i = 0; i < options_len; i++) {
        if ( options[i].name.get_size() == 0 )
          return false;
      }
      return true;
    }
  };

  //@abi table
  struct PACKED (opt_poll) {
    poll_id id;
    eosio::string question;
    uint8_t results_len = 0;
    option_result results[max_options];

    opt_poll() {}
    opt_poll(const opt_poll_msg& msg) : question(msg.question) {
      for (int i = 0; i < msg.options_len; i++) {
        results[i] = option(msg.options[i]);
      }
      results_len = msg.options_len;
    }
    opt_poll(poll_id id) : id(id) {}

    bool has_option(uint32_t option_num) const {
      return (option_num >= 1 && option_num <= results_len);
    }

    // Returns true if vote is successfully added
    bool add_vote(uint32_t option_num) {
      if ( has_option(option_num) ) {
        results[option_num - 1].votes++;
        return true;
      } else return false;
    }
  };

  //@abi action vote
  struct opt_vote {
    poll_id id;
    account_name  voter;
    uint32_t      option;

    bool is_valid() const {
      return option > 0 && option < max_options;
    }
  };

  inline int32_t load_poll(poll_id id, table_name table, void* buffer, uint32_t size) {
    memcpy(buffer, &id, sizeof (poll_id));
    return load_i64(CONTRACT_NAME_UINT64, CONTRACT_NAME_UINT64, table, buffer, size);
  }

  // Returns true if finds a poll with id specified in poll parameter
  bool get_poll(opt_poll& poll);

  bool poll_exists(poll_id id, table_name poll_type) {
    const size_t bufflen = sizeof(poll_id) + 1;
    char buff[bufflen];
    int r = load_poll(id, poll_type, buff, bufflen);
    return r > -1;
  }

  inline bool poll_exists(const opt_poll& poll) {
    return poll_exists(poll.id, N(optpoll));
  }

  inline bool has_voted(poll_id id, account_name account, table_name poll_type) {
    int32_t r = load_i64(account, CONTRACT_NAME_UINT64, poll_type, &id, sizeof(poll_id) + 1);
    return r > -1;
  }

  inline bool has_voted(const opt_vote& vote) {
    return has_voted(vote.id, vote.voter, N(optvotes));
  }
}

