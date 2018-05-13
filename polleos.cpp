#include "polleos.hpp"

void polleos::newpoll(std::string question,
                      std::vector<std::string> option_names) {
   //TODO: check if message is valid

   polls polls_table(get_self(), get_self());
   poll_id id;

   //FIXME: Make sender of action payer.
   polls_table.emplace(get_self(), [&](poll& p) {
      id = p.id = polls_table.available_primary_key();
      p.question = question;
      p.is_token_poll = false;

      p.results = std::vector<option_result>();
      p.results.resize(option_names.size());
      std::transform(option_names.begin(), option_names.end(), p.results.begin(),
         [&](std::string str) {
            return option_result(str);
         });
   });

   eosio::print("Poll stored with id: ", id);
}

void polleos::newtokenpoll(std::string question,
                           std::vector<std::string> options,
                           token_name token) {

}

void polleos::vote(polleos::poll_id id, account_name voter,
                   uint32_t option_number) {

}

EOSIO_ABI( polleos, (newpoll)(newtokenpoll)(vote) )

