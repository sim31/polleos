/**
 *  @file
 *  @copyright defined in eos/LICENSE.txt
 */
#define BOOST_TEST_MODULE polleos-tests
#define BOOST_TEST_DYN_LINK
#include <boost/test/unit_test.hpp>
#include <boost/test/unit_test_monitor.hpp>
#include <cstdlib>
#include <iostream>
#include <fc/log/logger.hpp>
#include <eosio/chain/exceptions.hpp>

//extern uint32_t EOS_TESTING_GENESIS_TIMESTAMP;

void translate_fc_exception(const fc::exception &e) {
   std::cerr << "\033[33m" <<  e.to_detail_string() << "\033[0m" << std::endl;
   BOOST_TEST_FAIL("Caught Unexpected Exception");
}

struct eosio_test_fixture {
   eosio_test_fixture() {
      // Turn off blockchain logging if no --verbose parameter is not added
      // To have verbose enabled, call "tests/chain_test -- --verbose"
      bool is_verbose = false;
      std::string verbose_arg = "--verbose";
      int argc = boost::unit_test::framework::master_test_suite().argc;
      char** argv = boost::unit_test::framework::master_test_suite().argv;
      for (int i = 0; i < argc; i++) {
         if (verbose_arg == argv[i]) {
            is_verbose = true;
            break;
         }
      }
      if(!is_verbose)
         fc::logger::get(DEFAULT_LOGGER).set_log_level(fc::log_level::off);

      // Register fc::exception translator
      boost::unit_test::unit_test_monitor.register_exception_translator<fc::exception>
         (&translate_fc_exception);

      std::srand(time(NULL));
      std::cout << "Random number generator seeded to " << time(NULL) << std::endl;
   }
};

BOOST_TEST_GLOBAL_FIXTURE( eosio_test_fixture );

