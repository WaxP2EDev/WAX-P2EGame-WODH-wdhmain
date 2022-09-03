#pragma once
#include <eosio/eosio.hpp>

using namespace eosio;

//Remove after deleting rows

struct [[eosio::contract("drhemsworlds"), eosio::table]] lfactor
{
    name player;
    asset factor;

    uint64_t primary_key() const { return player.value; };
};

using lfactors = multi_index<name("lfactors"), lfactor>;
