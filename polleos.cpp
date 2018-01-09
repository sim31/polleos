#include <polleos.hpp>
#include <polleos_raw.hpp>
#include <currency.hpp>

namespace polleos {

  bool get_poll(const eosio::string& question, opt_poll& poll) {
    const uint32_t bufflen = 1024;
    bytes b = bytes { bufflen, (uint8_t*)eosio::malloc(bufflen) };
    int32_t r = load_poll(question, N(optpoll), (char*)b.data, b.len);

    if ( r <= 0)
      return false;

    poll.question = question;
    //eosio::raw::unpack((char*)b.data, r, poll.results);
    eosio::datastream<const char*> ds((const char*)b.data, r);
    poll.results_len = eosio::raw::unpack(ds, poll.results, max_options);
    eosio::free(b.data);
    return true;
  }

  void store_poll(const opt_poll& poll) {
    //eosio::dump(poll);
    char* key = (char*)poll.question.get_data();
    uint32_t keylen = poll.question.get_size();
    bytes value = eosio::raw::pack<option_result>(poll.results, poll.results_len);
    store_str(CONTRACT_NAME_UINT64, N(optpoll), key, keylen,
                (char*)value.data, value.len);
  }

  // Log that vote.voter account voted, so that we can prevent it from voting twice
  void store_voter(const opt_vote& vote) {
    store_str(vote.voter, N(optvotes), (char*)vote.question.get_data(), vote.question.get_size(),
              nullptr, 0);
  }

  void store_vote(const opt_vote& vote) {
    opt_poll poll;
    assert( get_poll(vote.question, poll), "Poll with this question does not exist");
    assert ( poll.add_vote(vote.option), "This option number does not exist for this poll");
    store_poll(poll);
    store_voter(vote);
  }

  void validate_poll_msg(const opt_poll_msg& msg) {
    assert( msg.is_valid(), "Poll is invalid. Check if question and option fields are not empty");
    assert( !poll_exists(msg), "Poll with this question already exists");
  }

  void create_poll(const opt_poll_msg& msg) {
    validate_poll_msg(msg);
    opt_poll poll = opt_poll(msg);
    store_poll(poll);
  }

  void create_poll(const opt_token_poll_msg& msg) {
    validate_poll_msg(msg);
    opt_poll poll = opt_poll(msg);
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
          create_poll(msg);
        } else if (action == N(newtokenpoll)) {
          auto msg = eosio::current_message<opt_token_poll_msg>();
          create_poll(msg);
        } else if (action == N(vote)) {
          auto msg = eosio::current_message<opt_vote>();
          add_vote(msg);

          account acc = get_account(N(currency), N(inita));
          eosio::print("inita currency balance: ", acc.balance);
        }
      }
    }

} // extern "C"
