#pragma once

#include <eosiolib/eosio.hpp>

class polleos : public eosio::contract {
   public:
      typedef uint64_t poll_id;

      polleos(account_name contract_name) : eosio::contract(contract_name) {}

      struct option {
         std::string name;

         option(std::string name) : name(name) {}
         option() {}

         EOSLIB_SERIALIZE(option, (name))
      };

      struct option_result : option {
         uint64_t votes = 0;

         //FIXME: will you use these constructors?
         option_result(const std::string& name, uint64_t votes) : option(name),
                                                                  votes(0) {}
         option_result(const std::string& name) : option_result(name, 0) {}
         option_result(const option& opt) : option(opt), votes(0) {}
         option_result() {}

         EOSLIB_SERIALIZE(option_result, (name)(votes))
      };

      //@abi table
      struct poll {
         poll_id id;
         std::string question;
         std::vector<option_result> results;
         bool is_token_poll = false;
         token_name token;

         uint64_t primary_key() const {
            return id;
         }

         uint64_t get_reverse_key() const {
            return ~id;
         }

         EOSLIB_SERIALIZE(poll, (id)(question)(results)(is_token_poll)(token))
      };

      typedef eosio::multi_index<N(poll), poll, eosio::indexed_by<
         N(reverse), eosio::const_mem_fun<poll, uint64_t, &poll::get_reverse_key>
       > > polls;

      //@abi action
      void newpoll(std::string question, std::vector<std::string> option_names);
      //@abi action
      void newtokenpoll(std::string question, std::vector<std::string> option_names,
                        token_name token);
      //@abi action
      void vote(poll_id id, account_name voter, uint32_t option_number);

};
