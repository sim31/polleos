#define BOOST_TEST_DYN_LINK

#include <boost/test/unit_test.hpp>
#include <eosio/testing/tester.hpp>
#include <eosio/chain/abi_serializer.hpp>
#include <eosio/chain/symbol.hpp>

#include <polleos.abi.hpp>
#include <polleos.wast.hpp>

#include <Runtime/Runtime.h>

#include <fc/variant_object.hpp>

#include <iostream>

using namespace eosio;
using namespace eosio::chain;
using namespace eosio::testing;
using namespace fc;
using namespace std;

using mvo = fc::mutable_variant_object;

class polleos_tester : public tester {
   public:
      typedef uint64_t                 poll_id;
      typedef std::vector<std::string> option_names;
      typedef extended_symbol          token_info;
      typedef vector<variant>          option_results;

      polleos_tester() {

         produce_blocks(2);

         create_accounts({N(alice), N(bob), N(carol), contract_name});
         produce_blocks(2);

         set_code(contract_name, polleos_wast);
         set_abi(contract_name, polleos_abi);

         produce_blocks();

         const auto& accnt = control->db().get<account_object, by_name>(N(polleos));
         abi_def abi;
         BOOST_REQUIRE_EQUAL(abi_serializer::to_abi(accnt.abi, abi), true);
         abi_ser.set_abi(abi);
      }

      action_result push_action( const account_name& signer, const action_name &name, const variant_object &data ) {
         string action_type_name = abi_ser.get_action_type(name);

         action act;
         act.account = contract_name;
         act.name    = name;
         act.data    = abi_ser.variant_to_binary( action_type_name, data );

         return base_tester::push_action( std::move(act), uint64_t(signer));
      }

      action_result newpoll(string question, account_name owner, option_names options) {

         return push_action( owner, N(newpoll), mvo()
            ( "question", question)
            ( "creator", owner)
            ( "options", options )
         );
      }

      fc::variant get_poll( poll_id id ) {

         auto data = get_row_by_account( contract_name, contract_name, N(poll), id);
         return data.empty() ? fc::variant() : abi_ser.binary_to_variant( "poll", data );
      }


      const account_name contract_name = N(polleos);
   private:

      abi_serializer abi_ser;
};

BOOST_AUTO_TEST_SUITE( polleos_tests )

BOOST_FIXTURE_TEST_CASE( create_poll, polleos_tester ) try {

   string question = "Q1";
   option_names options {"opt1", "opt2", "opt3"};
   option_results results { mvo("name", "opt1")("votes", 0.0),
                            mvo("name", "opt2")("votes", 0.0),
                            mvo("name", "opt3")("votes", 0.0) };


   newpoll(question, N(alice), options);
   produce_blocks(1);

   variant created_poll = get_poll(0);

   variant required_poll = mvo()
      ("question", question)
      ("results", results)
      ("is_token_poll", false);

   cout << "Created poll: " << created_poll << endl;
   cout << "Expected poll: " << required_poll << endl;

   REQUIRE_MATCHING_OBJECT(required_poll, created_poll);
} FC_LOG_AND_RETHROW()

BOOST_AUTO_TEST_SUITE_END()

