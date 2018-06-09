#pragma once

#include <eosiolib/eosio.hpp>
#include <cmath>
#include "eosio.token.hpp"

class polleos : public eosio::contract {
   public:
      typedef uint64_t                 poll_id;
      typedef std::vector<std::string> option_names;
      typedef eosio::extended_symbol   token_info;

      polleos(account_name contract_name) : eosio::contract(contract_name) {}

      struct option {
         std::string name;

         option(std::string name) : name(name) {}

         option() {}

         EOSLIB_SERIALIZE(option, (name))
      };

      struct option_result : option {
         double votes = 0;

         option_result(const std::string& name, uint64_t votes) : option(name),
                                                                  votes(0) {}

         option_result(const std::string& name) : option_result(name, 0) {}

         option_result() {}

         EOSLIB_SERIALIZE(option_result, (name)(votes))
      };

      typedef std::vector<option_result> option_results;

      //@abi table
      struct poll {
         poll_id        id;
         std::string    question;
         option_results results;
         bool           is_token_poll = false;
         token_info     token;

         uint64_t primary_key() const {

            return id;
         }

         uint64_t get_reverse_key() const {

            return ~id;
         }

         void set(poll_id id, const std::string& question,
                  const option_names& options, bool is_token_poll,
                  token_info token);

         EOSLIB_SERIALIZE(poll, (id)(question)(results)(is_token_poll)(token))
      };

      //@abi table votes
      struct poll_vote {
         uint64_t     vote_id;   // opaque, using available_primary_key()
         poll_id      poll_id;
         account_name poll_owner;
         uint32_t     option_id;

         uint64_t primary_key() const {

            return vote_id;
         }

         uint128_t owner_poll_key() const {
            return get_owner_poll_key(poll_owner, poll_id);
         }

         static uint128_t get_owner_poll_key(account_name owner, polleos::poll_id id) {
            uint128_t key = owner;
            *(reinterpret_cast<uint64_t*>(&key) + 1) = id;
            return key;
         }

         EOSLIB_SERIALIZE(poll_vote, (vote_id)(poll_id)(poll_owner)(option_id))
      };

      typedef eosio::multi_index<N(poll), poll, eosio::indexed_by<
         N(reverse), eosio::const_mem_fun<poll, uint64_t, &poll::get_reverse_key>
      > > poll_table;

      typedef eosio::multi_index<N(votes), poll_vote, eosio::indexed_by<
         N(ownerpoll), eosio::const_mem_fun<poll_vote, uint128_t, &poll_vote::owner_poll_key>
      > > vote_table;

      //@abi action
      void newpoll(const std::string& question, account_name owner,
                   const std::vector<std::string>& options);

      //@abi action
      void newtokenpoll(const std::string& question, account_name owner,
                        const std::vector<std::string>& options,
                        token_info token);

      //@abi action
      void vote(poll_id id, account_name poll_owner, account_name voter,
                uint32_t option_id);


   private:
      void store_poll(const std::string& question, account_name owner,
                      const option_names& options,
                      bool is_token_poll, token_info token);

      void store_vote(const poll& p, poll_table& polls, vote_table& votes,
                      uint32_t option_id, double weight);

      void store_token_vote(const poll& p, poll_table& poll, vote_table& votes,
                            uint32_t option_id);

      double to_weight(const eosio::asset& stake) {

         return stake.amount / std::pow(10, stake.symbol.precision());
      }
};
