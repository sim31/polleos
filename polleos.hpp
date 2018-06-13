#pragma once

#include <eosiolib/eosio.hpp>
#include <cmath>
#include "eosio.token.hpp"

class polleos : public eosio::contract {
   public:
      typedef uint64_t                 poll_id_t;
      typedef std::vector<std::string> option_names_t;
      typedef eosio::extended_symbol   token_info_t;
      typedef uint8_t                  option_id_t;

      polleos(account_name contract_name) : eosio::contract(contract_name),
                                            _polls(contract_name, contract_name) {}

      struct option {
         std::string name;

         option(std::string name) : name(name) {}

         option() {}

         EOSLIB_SERIALIZE(option, (name))
      };

      struct option_result : option {
         double votes = 0;

         option_result(const std::string& name, uint64_t votes) : option(name),
                                                                  votes(votes) {}

         option_result(const std::string& name) : option_result(name, 0) {}

         option_result() {}

         EOSLIB_SERIALIZE(option_result, (name)(votes))
      };

      typedef std::vector<option_result> option_results;

      //@abi table
      struct poll {
         poll_id_t      id;
         std::string    question;
         option_results results;
         bool           is_token_poll = false;
         token_info_t   token;

         uint64_t primary_key() const {

            return id;
         }

         uint64_t get_reverse_key() const {

            return ~id;
         }

         void set(poll_id_t id, const std::string& question,
                  const option_names_t& options, bool is_token_poll,
                  token_info_t token);

         EOSLIB_SERIALIZE(poll, (id)(question)(results)(is_token_poll)(token))
      };

      //@abi table votes
      struct poll_vote {
         poll_id_t   poll_id;
         option_id_t option_id;

         uint64_t primary_key() const {

            return poll_id;
         }

         EOSLIB_SERIALIZE(poll_vote, (poll_id)(option_id))
      };

      typedef eosio::multi_index<N(poll), poll, eosio::indexed_by<
         N(reverse), eosio::const_mem_fun<poll, uint64_t, &poll::get_reverse_key>
      > > poll_table;

      typedef eosio::multi_index<N(votes), poll_vote> vote_table;

      //@abi action
      void newpoll(const std::string& question, account_name creator,
                   const std::vector<std::string>& options);

      //@abi action
      void newtokenpoll(const std::string& question, account_name payer,
                        const std::vector<std::string>& options,
                        token_info_t token);

      //@abi action
      void vote(poll_id_t id, account_name voter, option_id_t option_id);


   private:
      void store_poll(const std::string& question, account_name owner,
                      const option_names_t& options,
                      bool is_token_poll, token_info_t token);

      void store_vote(const poll& p, vote_table& votes, option_id_t option_id,
                      double weight);

      void store_token_vote(const poll& p, vote_table& votes, option_id_t option_id);

      double to_weight(const eosio::asset& stake) {

         return stake.amount / std::pow(10, stake.symbol.precision());
      }

      poll_table _polls;
};
