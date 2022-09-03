#pragma once
#include <eosio/eosio.hpp>

using namespace eosio;

//Scoped by get_self().value
struct [[eosio::contract("drhemsworlds"), eosio::table]] penaltyTable
{
    uint64_t id;
    name wheel;
    name owner;
    int32_t pentemp;
    uint64_t penalty_factor;

    uint64_t primary_key() const { return id; };
    uint64_t owner_key() const { return  owner.value; };
};

using by_owner_key2 = indexed_by<name("byowner2"), const_mem_fun<penaltyTable, uint64_t, &penaltyTable::owner_key>>;
using penalty = multi_index<name("penalty"), penaltyTable, by_owner_key2>;
