#include "polleos.hpp"

void polleos::newpoll(std::string question, std::vector<std::string> options) {
   eosio::print("Helllo!");
}

void polleos::newtokenpoll(std::string question,
                           std::vector<std::string> options,
                           token_name token) {

}

void polleos::vote(polleos::poll_id id, account_name voter,
                   uint32_t option_number) {

}

EOSIO_ABI( polleos, (newpoll)(newtokenpoll)(vote) )

