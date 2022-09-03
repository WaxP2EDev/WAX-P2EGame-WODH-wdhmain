#pragma once
#include <eosio/eosio.hpp>

using namespace eosio;

//Scoped by get_self().value
struct [[eosio::contract("drhemsworlds"), eosio::table]] spin
{
    uint64_t id;
    name wheel;
    name owner;
    int32_t pentemp;
    uint64_t penalty_factor;

    uint64_t primary_key() const { return id; };
    uint64_t owner_key() const { return  owner.value; };
};

using by_owner_key = indexed_by<name("byowner"), const_mem_fun<spin, uint64_t, &spin::owner_key>>;
using spins = multi_index<name("spins"), spin, by_owner_key>;
