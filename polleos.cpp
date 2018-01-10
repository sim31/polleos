#include <polleos.hpp>
#include <polleos_raw.hpp>
#include <currency.hpp>
#include <eoslib/memory.hpp>

namespace polleos {

  bool get_poll(opt_poll& poll) {
    //FIXME: what happens if this size is not enough?
    //       would be great to get size of the whole row beforehand.
    const uint32_t bufflen = 2048;
    char buff[bufflen];
    int32_t r = load_poll(poll.id, N(optpoll), buff, bufflen);

    if ( r <= 0)
      return false;

    //eosio::raw::unpack((char*)b.data, r, poll.results);
    eosio::datastream<const char*> ds(buff, r);
    eosio::raw::unpack(ds, poll);
    return true;
  }

  void store_poll(const opt_poll& poll) {
    //eosio::dump(poll);
    //FIXME:
    const uint32_t bufflen = 2048;
    char buff[bufflen];
    eosio::datastream<char*> ds(buff, bufflen);

    eosio::raw::pack(ds, poll);

    store_i64(CONTRACT_NAME_UINT64, N(optpoll), buff, ds.tellp());
  }

  // Log that vote.voter account voted, so that we can prevent it from voting twice
  void store_voter(const opt_vote& vote) {
    store_i64(vote.voter, N(optvotes), &vote.id, sizeof(poll_id));
  }

  bool is_stakeholder(account_name acc_name, token_name token) {
    account acc = get_account(token, acc_name);
    eosio::print("\nAccount balance: ", acc.balance, "\n");
    eosio::print("token: ", eosio::name(token));
    eosio::print("\naccount: ", eosio::name(acc_name), "\n");

    account acc1 = get_account(N(currency), N(inita));
    eosio::print("\nacc1 balance: ", acc1.balance, "\n");
    return acc.balance > 0;
  }

  void store_vote(const opt_vote& vote) {
    opt_poll poll(vote.id);
    assert( get_poll(poll), "Poll with this id does not exist");
    if ( poll.is_token_poll ) {
      assert( is_stakeholder(vote.voter, poll.token ),
              "Voter has to be a stakeholder of a token to participate in a poll");
    }
    assert ( poll.add_vote(vote.option), "This option number does not exist for this poll");
    store_poll(poll);
    store_voter(vote);
  }

  void validate_poll_msg(const opt_poll_msg& msg) {
    assert( msg.is_valid(), "Poll is invalid. Check if question and option fields are not empty");
  }

  
  void set_poll_id(opt_poll& poll) {
    poll_id buff[2];
    int32_t r = back_i64(CONTRACT_NAME_UINT64, CONTRACT_NAME_UINT64, N(optpoll), buff, sizeof(buff));
    poll.id = ( r > -1) ? buff[0] + 1 : 0;
  }

  void add_poll(opt_poll& poll) {
    set_poll_id(poll);
    store_poll(poll);
  }

  void add_vote(const opt_vote& vote) {
    eosio::require_auth(vote.voter);
    assert( vote.is_valid(), "Vote is invalid");
    assert( !has_voted(vote), "This account has already voted in this poll");
    store_vote(vote);
  }

}

using namespace polleos;
/**
 *  The init() and apply() methods must have C calling convention so that the blockchain can lookup and
 *  call these methods.
 */
extern "C" {
    /**
     *  This method is called once when the contract is published or updated.
     */
    void init()  {
    }

    /// The apply method implements the dispatch of events to this contract
    void apply( uint64_t code, uint64_t action ) {
      if (code == CONTRACT_NAME_UINT64) {
        if (action == N(newoptpoll)) {
          auto msg = eosio::current_message<opt_poll_msg>();
          validate_poll_msg(msg);
          opt_poll poll = create_poll(msg);
          add_poll(poll);
        } else if (action == N(newtokenpoll)) {
          auto msg = eosio::current_message<opt_token_poll_msg>();
          validate_poll_msg(msg);
          opt_poll poll = create_poll(msg);
          add_poll(poll);
        } else if (action == N(vote)) {
          auto msg = eosio::current_message<opt_vote>();
          add_vote(msg);

        }
      }
    }

} // extern "C"
