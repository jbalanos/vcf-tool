// ParsedRecord.h
#pragma once

#include <cstdint>
#include <string>
#include <vector>


namespace vcf_tool::domain::entity {

struct ParsedRecord {
    std::uint64_t            line_number{};
    std::string              raw_text;
    std::vector<std::string> fields;
    bool                     is_end{false};  // sentinel flag for downstream
};

} // namespace vcf_tool::domain::entity