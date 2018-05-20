#pragma once

#include <eosiolib/eosio.hpp>
#include <eosiolib/currency.hpp>
#include <cmath>

class polleos : public eosio::contract {
   public:
      typedef uint64_t                 poll_id;
      typedef std::vector<std::string> option_names;
      typedef eosio::extended_symbol   token_info;

      polleos(account_name contract_name) : eosio::contract(contract_name),
                                            _polls(get_self(), get_self()) {}

      struct option {
         std::string name;

         option(std::string name) : name(name) {}

         option() {}

         EOSLIB_SERIALIZE(option, (name))
      };

      struct option_result : option {
         double votes = 0;

         option_result(const std::string &name, uint64_t votes) : option(name),
                                                                  votes(0) {}

         option_result(const std::string &name) : option_result(name, 0) {}

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

         void set(poll_id id, const std::string &question,
                  const option_names &options, bool is_token_poll,
                  token_info token);

         EOSLIB_SERIALIZE(poll, (id)(question)(results)(is_token_poll)(token))
      };

      //@abi table votes
      struct poll_vote {
         poll_id id;

         uint64_t primary_key() {

            return id;
         }

         EOSLIB_SERIALIZE(poll_vote, (id))
      };

      typedef eosio::multi_index<N(poll), poll, eosio::indexed_by<
         N(reverse), eosio::const_mem_fun<poll, uint64_t, &poll::get_reverse_key>
      > >                                             poll_table;
      typedef eosio::multi_index<N(votes), poll_vote> vote_table;

      //@abi action
      void newpoll(const std::string &question,
                   const std::vector<std::string> &options);

      //@abi action
      void newtokenpoll(const std::string &question,
                        const std::vector<std::string> &options,
                        token_info token);

      //@abi action
      void vote(poll_id id, account_name voter, uint32_t option_id);

      static bool token_exists(token_info token) {

         auto sym_name = token.name();
         eosio::currency::stats st(token.contract, sym_name);
         return st.find(sym_name) != st.end();
      }

   private:
      void store_poll(const std::string &question,
                      const option_names &options,
                      bool is_token_poll, token_info token);

      void store_vote(const poll &p, vote_table &votes, uint32_t option_id,
                      double weight);

      void store_token_vote(const poll &p, vote_table &votes, uint32_t option_id);

      double to_weight(const eosio::asset& stake) {
         return stake.amount / std::pow(10, stake.symbol.precision());
      }

      poll_table _polls;
};
