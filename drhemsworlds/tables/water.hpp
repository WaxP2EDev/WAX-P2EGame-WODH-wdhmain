#pragma once
#include <eosio/eosio.hpp>

using namespace eosio;

struct [[eosio::contract("drhemsworlds"), eosio::table]] water
{
    name user;
    time_point_sec last_update;

    uint64_t primary_key() const { return user.value; };
};

using water_table = multi_index<name("water"), water>;
using freewater_table = multi_index<name("freewater"), water>;