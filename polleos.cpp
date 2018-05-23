#include "polleos.hpp"

void polleos::poll::set(polleos::poll_id id, const std::string& question,
                        const option_names& options, bool is_token_poll,
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

void polleos::store_poll(const std::string& question, account_name poll_owner,
                         const option_names& options,
                         bool is_token_poll, token_info token) {

   poll_table polls(get_self(), poll_owner);
   poll_id    id;

   polls.emplace(poll_owner, [&](poll& p) {
      id = polls.available_primary_key();
      p.set(id, question, options, is_token_poll, token);
   });

   eosio::print("Poll stored with id: ", id);
}

void polleos::store_vote(const polleos::poll& p, polleos::poll_table& polls,
                         polleos::vote_table& votes, uint32_t option_id, double weight) {

   eosio_assert(weight > 0, "Vote weight cannot be less than 0. Contract logic issue");

   // Voter (votes.get_scope()) pays
   votes.emplace(votes.get_scope(), [&](poll_vote& v) {
      v.vote_id = votes.available_primary_key();
      v.poll_id = p.id;
      v.poll_owner = polls.get_scope();
   });

   polls.modify(p, votes.get_scope(), [&](poll& p) {
      p.results[option_id].votes += weight;
   });
}

void polleos::store_token_vote(const polleos::poll& p, polleos::poll_table& polls,
                               polleos::vote_table& votes, uint32_t option_id) {

   eosio::currency::accounts accounts(p.token.contract, votes.get_scope());
   eosio::currency::account  acc = accounts.get(p.token.name(),
                                                "Voter has to be a stakeholder of a "
                                                "token to participate in a token poll");

   eosio_assert(acc.balance.is_valid(), "Balance of voter account is invalid. Something"
                                        " wrong with token contract.");
   eosio_assert(acc.balance.amount > 0,
                "Voter has to have more than 0 of tokens to participate in a poll");

   store_vote(p, polls, votes, option_id, to_weight(acc.balance));
}

void polleos::newpoll(const std::string& question, account_name owner,
                      const option_names& options) {

   store_poll(question, owner, options, false, token_info());
}

void polleos::newtokenpoll(const std::string& question, account_name owner,
                           const option_names& options, token_info token) {

   eosio_assert(token_exists(token), "This token does not exist");
   store_poll(question, owner, options, true, token);
}

void polleos::vote(polleos::poll_id id, account_name poll_owner, account_name voter,
                   uint32_t option_id) {

   eosio::require_auth(voter);

   poll_table polls(get_self(), poll_owner);
   const poll & p = polls.get(id, "Poll with this id does not exist");

   eosio_assert(option_id < p.results.size(),
                "Option with this id does not exist");

   vote_table votes(get_self(), voter);
   auto index = votes.get_index<N(ownerpoll)>();
   uint128_t ownerpoll_key = poll_vote::get_owner_poll_key(poll_owner, id);
   eosio_assert(index.find(ownerpoll_key) == index.end(), "This account has already "
                                                          "voted in this poll");

   if (p.is_token_poll)
      store_token_vote(p, polls, votes, option_id);
   else
      store_vote(p, polls, votes, option_id, 1);

   eosio::print("Vote stored!");
}

EOSIO_ABI(polleos, (newpoll)(newtokenpoll)(vote))

