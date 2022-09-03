#include <eosio/eosio.hpp>

using namespace eosio;

constexpr name ATOMIC_ASSETS("atomicassets");
constexpr name COLLECTION_NAME("drhemsworlds");
constexpr name ORNG_CONTRACT("orng.wax");
constexpr name INGAME_CONTRACT("drhemsingame");
constexpr name LANDS_CONTRACT("drhemslandsx");

constexpr symbol POINTS_SYMBOL("POINTS", 0);
constexpr symbol WATER_SYMBOL("WATER", 0);
constexpr symbol PERCENT_SYMBOL("PERCENT", 4);
constexpr symbol MULTI_SYMBOL("MULTI", 2);
// constexpr symbol HEMS_SYMBOL("HEMZ", 4);
constexpr symbol HEMS_SYMBOL("HEMS", 4);

const asset MAX_WATER_LEVEL(100, WATER_SYMBOL);

constexpr name READY("ready");
constexpr name PROD("prod");
constexpr name LOCKED("locked");
constexpr name BANNED("banned");
const std::set<name> STATES{READY, PROD, LOCKED, BANNED};

constexpr name AVATAR("avatar");
constexpr name TOOL("tool");
constexpr name SEED("seed");
constexpr name LAND("land");
const std::set<name> TYPES{AVATAR, TOOL, SEED, LAND};

constexpr name TOOL1_PENALTY_WHEEL("penalty1");
constexpr name TOOL2_PENALTY_WHEEL("penalty2");
constexpr name NFT_CHANCE_WHEEL("nftchance");

// constexpr name NORMAL("normal");
// constexpr name MAINTENANCE("maintenance");
// const std::set<name> MODES{NORMAL, MAINTENANCE};

const uint32_t EXCEPTION_PRIZE_ID = 10;

const uint32_t PASS1_TEMPLATE_ID = 437278; // 340295
const uint32_t PASS2_TEMPLATE_ID = 437279; // 340297
const uint32_t PASS3_TEMPLATE_ID = 437280; // 340299

const std::string NFT_CHANCE("6");
const std::string NEW_PROD_MEMO("newprod");
const std::string EXST_PROD_PREFIX("exstprod");
const std::string PART_EXST_PROD_PREFIX("partexstprod");
const std::string TRANSFER_MEMO("transfer");
std::string ENC_KEY("ken48fhsDFWSF44456");
