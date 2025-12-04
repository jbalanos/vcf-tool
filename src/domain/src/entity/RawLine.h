// domain/src/entity/raw_line.hpp
#pragma once

#include <cstdint>
#include <string>

namespace vcf_tool::domain::entity {

struct RawLine {
    std::uint64_t line_number{};
    std::string   text;
    bool          is_end{false};
};

} // namespace vcf_tool::domain::entity
