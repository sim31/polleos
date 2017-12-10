#include <polleos.hpp>
#include <polleos_raw.hpp>

namespace CONTRACT_NAME {

  void store_poll(const multi_opt_poll& poll) {
    eosio::dump(poll);
    char* key = (char*)poll.question.get_data();
    uint32_t keylen = poll.question.get_size();
    bytes value = eosio::raw::pack<option_result>(poll.results, 4);
    ::store_str(CONTRACT_NAME_UINT64, N(multioptpoll), key, keylen,
                (char*)value.data, value.len);
  }

  void create_poll(const multi_opt_poll_msg& msg) {
    assert( !poll_exists(msg), "Poll with this question already exists");
    multi_opt_poll poll = multi_opt_poll(msg);
    store_poll(poll);
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
        }
      }
    }

} // extern "C"
