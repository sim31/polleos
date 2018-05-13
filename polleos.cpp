#include "polleos.hpp"

void polleos::poll::set(polleos::poll_id id, const std::string& question,
                        const option_names& options, bool is_token_poll,
                        token_info token) {
   eosio_assert( !question.empty(), "Question can't be empty" );

   this->id = id;
   this->question = question;
   this->is_token_poll = is_token_poll;

   results.resize(options.size());
   std::transform(options.begin(), options.end(), results.begin(),
                  [&](std::string str) {
                     eosio_assert( !str.empty(), "Option names can't be empty");
                     return option_result(str);
                  });
}

void polleos::store_poll(const std::string& question, const option_names& options,
                         bool is_token_poll, token_info token) {

   poll_table polls(get_self(), get_self());
   poll_id id;

   //FIXME: Make sender of action payer.
   polls.emplace(get_self(), [&](poll& p) {
      id = polls.available_primary_key();
      p.set(id, question, options, is_token_poll, token);
   });

   eosio::print("Poll stored with id: ", id);
}

void polleos::newpoll(const std::string& question, const option_names& options) {
   store_poll(question, options, false, token_info());
}

void polleos::newtokenpoll(const std::string& question, const option_names& options,
                           token_info token) {

   eosio_assert( token_exists(token), "This token does not exist" );
   store_poll(question, options, true, token);
}

void polleos::vote(polleos::poll_id id, account_name voter,
                   uint32_t option_id) {
   //TODO: Implement voting in token polls
   eosio::require_auth(voter);

   poll_table polls(get_self(), get_self());
   const poll& p = polls.get(id, "Poll with this id does not exist");

   eosio_assert( option_id < p.results.size(),
           "Option with this id does not exist" );

   vote_table votes(get_self(), voter);
   eosio_assert( votes.find(id) == votes.end(),  "This account has already voted "
                                                 "in this poll");
   //TODO: Make poll creator pay?
   votes.emplace(get_self(), [&](poll_vote& v) {
      v.id = id;
   });

   polls.modify(p, get_self(), [&](poll& p) {
      //FIXME: Check for overflows
      p.results[option_id].votes++;
   });

   eosio::print("Voted stored");
}

EOSIO_ABI( polleos, (newpoll)(newtokenpoll)(vote) )

