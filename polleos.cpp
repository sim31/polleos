#include <polleos.hpp>
#include <polleos_raw.hpp>

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
          eosio::dump(msg);
          //eosio::print("key: ", msg.key, ", arr_size: ", (uint32_t)msg.arr_size, "\n");
        }
      }
    }

} // extern "C"
