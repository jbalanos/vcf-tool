#pragma once

#include <string>
#include <vector>
#include <optional>
#include <nlohmann/json.hpp>

#include "../entity/RawLine.h"
#include "../entity/ParsedRecord.h"

namespace vcf_tool::domain {

class VcfLineParser {
public:
    VcfLineParser() = default;

    // Main parsing interface (matches NaiveLineParser)
    entity::ParsedRecord operator()(const entity::RawLine& raw) const;

private:
    // Helper methods
    std::vector<std::string> split_tabs(const std::string& line) const;
    nlohmann::json parse_info_field(const std::string& info_str) const;
    nlohmann::json parse_format_field(
        const std::string& format_str,
        const std::string& sample_str
    ) const;
    std::optional<double> try_parse_double(const std::string& str) const;
};

}  // namespace vcf_tool::domain
