// ParsedRecord.h
#pragma once

#include <cstdint>
#include <string>
#include <vector>

#include "VcfRecord.h"

namespace vcf_tool::domain::entity {

struct ParsedRecord {
    std::uint64_t            line_number{};
    std::string              raw_text;
    vcf_tool::domain::VcfRecord vcf_data;
    bool                     is_end{false};  // sentinel flag for downstream
};

} // namespace vcf_tool::domain::entity