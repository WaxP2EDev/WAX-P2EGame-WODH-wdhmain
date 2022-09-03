#pragma once
#include <eosio/eosio.hpp>

using namespace eosio;
using namespace std;

struct game_factor
{
    asset game_factor; //MULTI_SYMBOL
    asset water_factor; //WATER_SYMBOL
    asset time_factor; //MULTI_SYMBOL
    name type;
    uint64_t max_usage;
    int32_t penalty;
    int32_t pass1;
    int32_t pass2;
    int32_t pass3;
    string tool_group;
    int32_t land_factor;
};
