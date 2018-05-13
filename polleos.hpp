#pragma once

#include <eosiolib/eosio.hpp>

class polleos : public eosio::contract {
   public:
      typedef uint64_t                 poll_id;
      typedef std::vector<std::string> option_names;

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

      typedef std::vector<option_result> option_results;

      //@abi table
      struct poll {
         poll_id        id;
         std::string    question;
         option_results results;
         bool           is_token_poll = false;
         token_name     token;

         uint64_t primary_key() const {
            return id;
         }

         uint64_t get_reverse_key() const {
            return ~id;
         }

         void set(poll_id id, const std::string& question,
                  const option_names& options, bool is_token_poll,
                  token_name token);

         EOSLIB_SERIALIZE(poll, (id)(question)(results)(is_token_poll)(token))
      };

      //@abi table
      struct poll_vote {
         poll_id id;

         uint64_t primary_key() {
            return id;
         }

         EOSLIB_SERIALIZE( poll_vote,(id))
      };

      typedef eosio::multi_index<N(poll), poll, eosio::indexed_by<
         N(reverse), eosio::const_mem_fun<poll, uint64_t, &poll::get_reverse_key>
       > > poll_table;
      typedef eosio::multi_index<N(votes), poll_vote> vote_table;

      //@abi action
      void newpoll(const std::string& question,
                   const std::vector<std::string>& options);
      //@abi action
      void newtokenpoll(const std::string& question,
                        const std::vector<std::string>& options,
                        token_name token);
      //@abi action
      void vote(poll_id id, account_name voter, uint32_t option_id);

   private:
      void store_poll(const std::string& question,
                      const option_names& options,
                      bool is_token_poll, token_name token);

};
