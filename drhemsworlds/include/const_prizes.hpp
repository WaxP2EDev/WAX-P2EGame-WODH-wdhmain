#pragma once
#include <eosio/eosio.hpp>

using namespace eosio;

//<<odd>,<template_id>>
const std::map<uint64_t, int32_t> normal_prizes = {
  {1000, 10},
  {2500, 374194},
  {4000, 343750},
  {5850, 367033},
  {7350, 316958},
  {8850, 343805},
  {9250, 343780},
  {9550, 316898},
  {9750, 316903},
  {9900, 317126},
  {9975, 376239},
  {10000, 316915}
};

//<<wheel_name>, <prizes>>
const std::map<name, std::map<uint64_t, int32_t>> wheels = {
    {name("normal"), normal_prizes}
};
