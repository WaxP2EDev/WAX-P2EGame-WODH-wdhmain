#pragma once
#include <eosio/eosio.hpp>

using namespace eosio;

//Remove after deleting rows

//Scoped by wheel type
struct [[eosio::contract("drhemsworlds"), eosio::table]] prize
{
    uint64_t template_id;
    asset odd;

    uint64_t primary_key() const { return template_id; };
    uint64_t odd_key() const { return (uint64_t)odd.amount; };
};

using by_odd_key = indexed_by<name("byodd"), const_mem_fun<prize, uint64_t, &prize::odd_key>>;
using prizes = multi_index<name("prizes"), prize, by_odd_key>;