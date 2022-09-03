#pragma once
#include <eosio/eosio.hpp>

using namespace eosio;

struct [[eosio::contract("drhemsworlds"), eosio::table]] player
{
    name user;
    asset game_balance;
    asset water_level;
    std::string production_info;
    name production_state;
    time_point_sec production_end;
    asset points_to_add;
    uint32_t prize_won;

    uint64_t primary_key() const { return user.value; };
};

using players = multi_index<name("players"), player>;