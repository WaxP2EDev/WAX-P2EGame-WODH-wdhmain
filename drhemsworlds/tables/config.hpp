#pragma once
#include <eosio/eosio.hpp>
#include <eosio/singleton.hpp>
#include <eosio/asset.hpp>

using namespace eosio;

struct [[eosio::contract("drhemsworlds"), eosio::table]] config
{
    asset global_point_factor;
    asset global_max_time_factor;
    asset global_time_factor;
    asset water_price;
    name mode;
    name current_wheel;
    asset wheel_cost;
};

using config_table = singleton<name("config"), config>;
