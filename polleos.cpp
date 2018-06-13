#include "polleos.hpp"
#include <limits>

void polleos::poll::set(polleos::poll_id_t id, const std::string& question,
                        const option_names_t& options, bool is_token_poll,
                        token_info_t token) {

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

void polleos::store_poll(const std::string& question, account_name poll_owner,
                         const option_names_t& options,
                         bool is_token_poll, token_info_t token) {

   poll_id_t  id;

   eosio_assert(options.size() < std::numeric_limits<option_id_t>::max(),
                "Too many options");

   _polls.emplace(poll_owner, [&](poll& p) {
      id = _polls.available_primary_key();
      p.set(id, question, options, is_token_poll, token);
   });

   eosio::print("Poll stored with id: ", id);
}

void polleos::store_vote(const polleos::poll& p, polleos::vote_table& votes,
                         option_id_t option_id, double weight) {

   eosio_assert(weight > 0, "Vote weight cannot be less than 0. Contract logic issue");

   // Voter (votes.get_scope()) pays
   votes.emplace(votes.get_scope(), [&](poll_vote& v) {
      v.poll_id    = p.id;
      v.option_id  = option_id;
   });

   _polls.modify(p, votes.get_scope(), [&](poll& p) {
      p.results[option_id].votes += weight;
   });
}

void polleos::store_token_vote(const polleos::poll& p, polleos::vote_table& votes,
                               option_id_t option_id) {

   account_name voter = votes.get_scope();

   eosio::token token(p.token.contract);
   // Should fail if voter is not a stakeholder
   eosio::asset balance = token.get_balance(voter, p.token.name());
   // Some additional checks
   eosio_assert(balance.is_valid(), "Balance of voter account is invalid. Something"
                                    " wrong with token contract.");
   eosio_assert(balance.amount > 0,
                "Voter has to have more than 0 of tokens to participate in a poll!");

   store_vote(p, votes, option_id, to_weight(balance));
}

void polleos::newpoll(const std::string& question, account_name payer,
                      const option_names_t& options) {

   store_poll(question, payer, options, false, token_info_t());
}

void polleos::newtokenpoll(const std::string& question, account_name owner,
                           const option_names_t& options, token_info_t token_inf) {

   eosio::token token(token_inf.contract);
   eosio_assert(token.exists(token_inf.name()), "This token does not exist");
   store_poll(question, owner, options, true, token_inf);
}

void polleos::vote(polleos::poll_id_t id, account_name voter, option_id_t option_id) {

   eosio::require_auth(voter);

   const poll & p = _polls.get(id, "Poll with this id does not exist");

   eosio_assert(option_id < p.results.size(),
                "Option with this id does not exist");

   vote_table votes(get_self(), voter);
   eosio_assert(votes.find(p.id) == votes.end(), "This account has already "
                                                 "voted in this poll");

   if (p.is_token_poll)
      store_token_vote(p, votes, option_id);
   else
      store_vote(p, votes, option_id, 1);

   eosio::print("Vote stored!");
}

EOSIO_ABI(polleos, (newpoll)(newtokenpoll)(vote))

