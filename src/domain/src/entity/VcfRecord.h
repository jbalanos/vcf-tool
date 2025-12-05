#pragma once

#include <string>
#include <cstdint>
#include <nlohmann/json.hpp>

namespace vcf_tool::domain {

struct VcfRecord {
    std::string chromosome;
    std::uint64_t position{};
    std::string ref;
    std::string alt;
    nlohmann::json data;  // JSON: {FILTER, QUAL, INFO, FORMAT}
};

}  // namespace vcf_tool::domain
