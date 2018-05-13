#include "polleos.hpp"

void polleos::poll::set(polleos::poll_id id, const std::string& question,
                        const std::vector<std::string>& options, bool is_token_poll,
                        token_name token) {
   this->id = id;
   this->question = question;
   this->is_token_poll = is_token_poll;

   results.resize(options.size());
   std::transform(options.begin(), options.end(), results.begin(),
                  [&](std::string str) {
                     return option_result(str);
                  });
}

void polleos::store_poll(const std::string& question,
                         const std::vector<std::string>& options,
                         bool is_token_poll, token_name token) {
   //TODO: check if message is valid

   poll_table polls(get_self(), get_self());
   poll_id id;

   //FIXME: Make sender of action payer.
   polls.emplace(get_self(), [&](poll& p) {
      id = polls.available_primary_key();
      p.set(id, question, options, is_token_poll, token);
   });

   eosio::print("Poll stored with id: ", id);
}

void polleos::newpoll(const std::string& question,
                      const std::vector<std::string>& option_names) {

   store_poll(question, option_names, false, 0);
}

void polleos::newtokenpoll(const std::string& question,
                           const std::vector<std::string>& option_names,
                           token_name token) {
   //TODO: Check if token exists
   store_poll(question, option_names, true, token);
}

void polleos::vote(polleos::poll_id id, account_name voter,
                   uint32_t option_number) {

}

EOSIO_ABI( polleos, (newpoll)(newtokenpoll)(vote) )

