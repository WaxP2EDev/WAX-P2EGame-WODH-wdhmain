/*#pragma once
#include <eosio/eosio.hpp>

using namespace eosio;

struct [[eosio::contract("drhemsworlds"), eosio::table]] pp1
{
    name user;
    uint32_t nft_won;
    asset penalty;

    uint64_t primary_key() const { return user.value; };
};

using prodplugin1 = multi_index<name("prodplugin1"), pp1>;
**/
