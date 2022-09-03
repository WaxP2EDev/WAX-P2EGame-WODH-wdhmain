#pragma once
#include <eosio/eosio.hpp>

using namespace eosio;

//<<odd>,<template_id>>
const std::map<uint64_t, int32_t> normal_prizes = {
  {1200, 10},
  {2000, 308083},
  {2800, 308084},
  {3600, 308086},
  {4400, 308087},
  {5200, 308088},
  {6000, 308089},
  {6800, 308090},
  {7600, 308091},
  {8400, 308092},
  {9200, 308093},
  {10000, 308094}
};

//<<wheel_name>, <prizes>>
const std::map<name, std::map<uint64_t, int32_t>> wheels = {
    {name("normal"), normal_prizes}
};
