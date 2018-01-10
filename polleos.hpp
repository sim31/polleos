#pragma once

#include <eoslib/eos.hpp>
#include <eoslib/db.hpp>
#include <eoslib/string.hpp>
#include <currency.hpp>

#define CONTRACT_NAME polleos1
#define CONTRACT_NAME_UINT64 N(polleos1)

namespace polleos {

  typedef uint64_t poll_id;

  const size_t max_options = 32;

  struct option {
    eosio::string name;

    option(eosio::string name) : name(name) {}
    option() {}
  };

  struct option_result : option {
    uint64_t votes = 0;

    option_result(const eosio::string& name, uint64_t votes) : option(name), votes(votes ) {}
    option_result(const eosio::string& name) : option(name), votes(0) {}
    option_result(const option& opt) : option(opt), votes(0) {}
    option_result() {}
  };

  typedef option_result* results_array;


  //@abi table
  struct opt_poll {
    poll_id id;
    eosio::string question;
    uint8_t results_len = 0;
    option_result results[max_options];
    bool is_token_poll = false;
    token_name token;
    //bool stake_weighted;

    opt_poll() {}
    opt_poll(poll_id id) : id(id) {}

    opt_poll(const eosio::string& question, const option* options, uint8_t options_len) : question(question) {
      set_options(options, options_len);
    }

    opt_poll(const eosio::string& question, const option* options, uint8_t options_len,
             token_name token) : question(question),
      is_token_poll(true), token(token) {

      set_options(options, options_len);
    }

    bool has_option(uint32_t option_num) const {
      return (option_num >= 1 && option_num <= results_len);
    }

    // Returns true if vote is successfully added
    bool add_vote(uint32_t option_num, uint64_t weight) {
      if ( has_option(option_num) ) {
        results[option_num - 1].votes += weight;
        return true;
      } else return false;
    }

    void set_options(const option* options, uint8_t options_len) {
      for (int i = 0; i < options_len; i++) {
        results[i] = option(options[i]);
      }
      results_len = options_len;
    }
  };

  //@abi action newoptpoll
  struct opt_poll_msg {
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

  //@abi action newtokenpoll
  struct opt_token_poll_msg : opt_poll_msg {
    token_name token;
    //bool stake_weighted;
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

  opt_poll create_poll(const opt_poll_msg& msg) {
    return opt_poll(msg.question, msg.options, msg.options_len);
  }

  opt_poll create_poll(const opt_token_poll_msg& msg) {
    return opt_poll(msg.question, msg.options, msg.options_len, msg.token);
  }


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

