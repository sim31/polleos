#include "polleos.hpp"

void polleos::poll::set(polleos::poll_id id, const std::string &question,
                        const option_names &options, bool is_token_poll,
                        token_info token) {

   eosio_assert(!question.empty(), "Question can't be empty");

   this->id            = id;
   this->question      = question;
   this->is_token_poll = is_token_poll;
   this->token         = token;

   results.resize(options.size());
   std::transform(options.begin(), options.end(), results.begin(),
                  [&](std::string str) {
                     eosio_assert(!str.empty(), "Option names can't be empty");
                     return option_result(str);
                  });
}

void polleos::store_poll(const std::string &question, const option_names &options,
                         bool is_token_poll, token_info token) {

   poll_table polls(get_self(), get_self());
   poll_id    id;

   //FIXME: Make sender of action payer.
   polls.emplace(get_self(), [&](poll &p) {
      id = polls.available_primary_key();
      p.set(id, question, options, is_token_poll, token);
   });

   eosio::print("Poll stored with id: ", id);
}

void polleos::store_vote(const polleos::poll &p, polleos::vote_table &votes,
                         uint32_t option_id, uint64_t weight) {

   eosio_assert(weight > 0, "Vote weight cannot be less than 1. Contract logic issue");

   //TODO: Make poll creator pay?
   votes.emplace(get_self(), [&](poll_vote &v) {
      v.id = p.id;
   });

   _polls.modify(p, get_self(), [&](poll &p) {
      //FIXME: Check for overflows
      p.results[option_id].votes += weight;
   });
}

void polleos::store_token_vote(const polleos::poll &p, polleos::vote_table &votes,
                               uint32_t option_id) {
   eosio::currency::accounts accounts(p.token.contract, votes.get_scope());
   eosio::currency::account  acc = accounts.get(p.token.name(),
                                                "Voter has to be a stakeholder of a "
                                                "token to participate in a token poll");
   eosio_assert(acc.balance.amount > 0,
                "Voter has to have more than 0 of tokens to participate in a poll");

   store_vote(p, votes, option_id, acc.balance.amount);
}

void polleos::newpoll(const std::string &question, const option_names &options) {

   store_poll(question, options, false, token_info());
}

void polleos::newtokenpoll(const std::string &question, const option_names &options,
                           token_info token) {

   eosio_assert(token_exists(token), "This token does not exist");
   store_poll(question, options, true, token);
}

void polleos::vote(polleos::poll_id id, account_name voter, uint32_t option_id) {

   eosio::require_auth(voter);

   const poll &p = _polls.get(id, "Poll with this id does not exist");

   eosio_assert(option_id < p.results.size(),
                "Option with this id does not exist");

   vote_table votes(get_self(), voter);
   eosio_assert(votes.find(id) == votes.end(), "This account has already voted "
                                               "in this poll");

   if (p.is_token_poll)
      store_token_vote(p, votes, option_id);
   else
      store_vote(p, votes, option_id, 1);

   eosio::print("Vote stored");
}

EOSIO_ABI(polleos, (newpoll)(newtokenpoll)(vote))

