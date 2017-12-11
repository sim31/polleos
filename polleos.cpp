#include <polleos.hpp>
#include <polleos_raw.hpp>

namespace CONTRACT_NAME {

  bool get_poll(const eosio::string& question, multi_opt_poll& poll) {
    const uint32_t bufflen = 1024;
    bytes b = bytes { bufflen, (uint8_t*)eosio::malloc(bufflen) };
    int32_t r = load_poll(question, N(multioptpoll), (char*)b.data, b.len);

    if ( r < 0)
      return false;
    else if ( r == 0)
      return true;

    poll.question = question;
    //eosio::raw::unpack((char*)b.data, r, poll.results);
    eosio::datastream<const char*> ds((const char*)b.data, r);
    eosio::raw::unpack(ds, poll.results, 4);
    eosio::free(b.data);
    return true;
  }

  void store_poll(const multi_opt_poll& poll) {
    eosio::dump(poll);
    char* key = (char*)poll.question.get_data();
    uint32_t keylen = poll.question.get_size();
    bytes value = eosio::raw::pack<option_result>(poll.results, 4);
    ::store_str(CONTRACT_NAME_UINT64, N(multioptpoll), key, keylen,
                (char*)value.data, value.len);
  }

  void store_vote(const multi_opt_vote& vote) {
    //TODO: store vote in voter's scope
    multi_opt_poll poll;
    assert( get_poll(vote.question, poll), "Poll with this question does not exist");
    assert ( poll.add_vote(vote.option), "This option number does not exist for this poll");
    store_poll(poll);
  }

  void create_poll(const multi_opt_poll_msg& msg) {
    assert( !poll_exists(msg), "Poll with this question already exists");
    multi_opt_poll poll = multi_opt_poll(msg);
    store_poll(poll);
  }

  void add_vote(const multi_opt_vote& vote) {
    eosio::require_auth(vote.voter);
    assert( !has_voted(vote), "This account has already voted in this poll");
    store_vote(vote);
  }
}

using namespace CONTRACT_NAME;
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
        if (action == N(newmultiopt)) {
          auto msg = eosio::current_message<multi_opt_poll_msg>();
          create_poll(msg);
        } else if (action == N(vote)) {
          auto msg = eosio::current_message<multi_opt_vote>();
          add_vote(msg);
        }
      }
    }

} // extern "C"
