#include <polleos.hpp>
#include <polleos_raw.hpp>

namespace CONTRACT_NAME {

  void store_poll(const multi_opt_poll& poll) {
    eosio::dump(poll);
    char* key = (char*)poll.question.get_data();
    uint32_t keylen = poll.question.get_size();
//    bytes packed = eosio::raw::pack(poll);
//    //FIXME: should substract more than 1 if keylen does not fit into 1 byte
//    uint32_t datalen = packed.len - keylen - 1;
//    char* data_start = (char*)packed.data + (packed.len - datalen);
//    ::store_str(CONTRACT_NAME_UINT64, N(multioptpoll), key, keylen,
//               data_start, datalen);
    //eosio::print("keylen: ", keylen, "datalen: ", datalen, "\n");

    //bytes value = eosio::raw::pack(poll.results);
    eosio::datastream<size_t> stream;
    eosio::raw::pack<eosio::datastream<size_t>, option_result>(stream, poll.results, 4);
    bytes b;
    b.len = stream.tellp();
    b.data = (uint8_t*)eosio::malloc(b.len);

    if( b.len ) {
       eosio::datastream<char*>  ds( (char*)b.data, b.len );
       eosio::raw::pack<eosio::datastream<char*>, option_result>(ds, poll.results, 4);
    }


    ::store_str(CONTRACT_NAME_UINT64, N(multioptpoll), key, keylen,
               (char*)b.data, b.len);

  }

  void store_msg(const multi_opt_poll_msg& poll) {
    eosio::dump(poll);
    char* key = (char*)poll.question.get_data();
    uint32_t keylen = poll.question.get_size();
//    bytes packed = eosio::raw::pack(poll);
//    //FIXME: should substract more than 1 if keylen does not fit into 1 byte
//    uint32_t datalen = packed.len - keylen - 1;
//    char* data_start = (char*)packed.data + (packed.len - datalen);
//    ::store_str(CONTRACT_NAME_UINT64, N(multioptpoll), key, keylen,
//               data_start, datalen);
    //eosio::print("keylen: ", keylen, "datalen: ", datalen, "\n");

    //bytes value = eosio::raw::pack(poll.results);
    eosio::datastream<size_t> stream;
    eosio::raw::pack<eosio::datastream<size_t>, option>(stream, poll.options, 4);
    bytes b;
    b.len = stream.tellp();
    b.data = (uint8_t*)eosio::malloc(b.len);

    if( b.len ) {
       eosio::datastream<char*>  ds( (char*)b.data, b.len );
       eosio::raw::pack<eosio::datastream<char*>, option>(ds, poll.options, 4);
    }


    ::store_str(CONTRACT_NAME_UINT64, N(msgs), key, keylen,
                (char*)b.data, b.len);
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
          eosio::print("message:\n");
          eosio::dump(msg);
          eosio::print("size: ", message_size());
          // Test packing
          multi_opt_poll poll;
          poll.question = msg.question;
          for (int i = 0; i < 4; i++) {
             poll.results[i].name = msg.options[i].name;
          }
          store_poll(poll);
          store_msg(msg);
//          uint32_t size = message_size() + 4 * sizeof (uint64_t);
//          char* data = (char*)eosio::malloc(size);
//          eosio::raw::pack(data, size, poll);
//          bytes packed = eosio::raw::pack(poll);
//          multi_opt_poll unpacked;
//          eosio::raw::unpack((char*)packed.data, packed.len, unpacked);
//          eosio::print("created poll:\n");
//          eosio::dump(unpacked);
//          eosio::print("size: ", packed.len);
          //eosio::print("key: ", msg.key, ", arr_size: ", (uint32_t)msg.arr_size, "\n");
        }
      }
    }

} // extern "C"
