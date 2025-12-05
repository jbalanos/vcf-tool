#include "VcfLineParser.h"

#include <sstream>
#include <algorithm>

#include <vcf_tool/utils/Errors.h>
#include <vcf_tool/utils/Format.h>

using vcf_tool::utils::errors::ParsingError;
using vcf_tool::utils::format;

namespace vcf_tool::domain {

entity::ParsedRecord VcfLineParser::operator()(const entity::RawLine& raw) const {
    entity::ParsedRecord result;
    result.line_number = raw.line_number;
    result.raw_text = raw.text;
    result.is_end = raw.is_end;

    // Handle sentinels and empty lines
    if (raw.is_end || raw.text.empty()) {
        return result;
    }

    // Skip header lines (## and #CHROM)
    if (raw.text[0] == '#') {
        return result;  // Return empty record for headers
    }

    // Split on TAB
    auto fields = split_tabs(raw.text);

    // Validate: need at least 8 fields (CHROM through INFO)
    if (fields.size() < 8) {
        throw ParsingError(format(
            "Line {}: Expected at least 8 fields, got {}",
            raw.line_number, fields.size()
        ));
    }

    // Extract fixed fields
    result.vcf_data.chromosome = fields[0];

    // Parse position
    try {
        result.vcf_data.position = std::stoull(fields[1]);
    } catch (...) {
        throw ParsingError(format(
            "Line {}: Invalid position '{}'",
            raw.line_number, fields[1]
        ));
    }

    result.vcf_data.ref = fields[3];
    result.vcf_data.alt = fields[4];

    // Build DATA JSON
    nlohmann::json data = nlohmann::json::object();

    // FILTER (string)
    data["FILTER"] = fields[6];

    // QUAL (numeric or null)
    if (fields[5] == ".") {
        data["QUAL"] = nullptr;
    } else {
        auto qual_opt = try_parse_double(fields[5]);
        data["QUAL"] = qual_opt.value_or(0.0);
    }

    // INFO (JSON object)
    data["INFO"] = parse_info_field(fields[7]);

    // FORMAT (JSON object) - if present
    if (fields.size() >= 10) {
        data["FORMAT"] = parse_format_field(fields[8], fields[9]);
    } else {
        data["FORMAT"] = nlohmann::json::object();
    }

    result.vcf_data.data = std::move(data);

    return result;
}

std::vector<std::string> VcfLineParser::split_tabs(const std::string& line) const {
    std::vector<std::string> result;
    std::istringstream iss(line);
    std::string field;

    while (std::getline(iss, field, '\t')) {
        result.push_back(field);
    }

    return result;
}

nlohmann::json VcfLineParser::parse_info_field(const std::string& info_str) const {
    nlohmann::json info = nlohmann::json::object();

    if (info_str.empty() || info_str == ".") {
        return info;
    }

    // Split on semicolon: "DP=50;AF=0.25;AC=2"
    std::istringstream iss(info_str);
    std::string pair;

    while (std::getline(iss, pair, ';')) {
        auto eq_pos = pair.find('=');

        if (eq_pos == std::string::npos) {
            // Flag field (no value, e.g., "PASS")
            info[pair] = true;
        } else {
            std::string key = pair.substr(0, eq_pos);
            std::string value = pair.substr(eq_pos + 1);

            // Try to parse as number, otherwise keep as string
            auto num_opt = try_parse_double(value);
            if (num_opt) {
                info[key] = num_opt.value();
            } else {
                info[key] = value;  // Keep as string (handles "10,20,30")
            }
        }
    }

    return info;
}

nlohmann::json VcfLineParser::parse_format_field(
    const std::string& format_str,
    const std::string& sample_str
) const {
    nlohmann::json format = nlohmann::json::object();

    if (format_str.empty() || sample_str.empty()) {
        return format;
    }

    // Split format keys: "GT:AD:DP"
    std::vector<std::string> keys;
    std::istringstream iss_keys(format_str);
    std::string key;
    while (std::getline(iss_keys, key, ':')) {
        keys.push_back(key);
    }

    // Split sample values: "0/1:18,18:36"
    std::vector<std::string> values;
    std::istringstream iss_vals(sample_str);
    std::string value;
    while (std::getline(iss_vals, value, ':')) {
        values.push_back(value);
    }

    // Zip together (stop at minimum length)
    size_t count = std::min(keys.size(), values.size());
    for (size_t i = 0; i < count; ++i) {
        if (values[i] == ".") {
            format[keys[i]] = nullptr;
        } else {
            // Try numeric, otherwise string
            auto num_opt = try_parse_double(values[i]);
            if (num_opt) {
                format[keys[i]] = num_opt.value();
            } else {
                format[keys[i]] = values[i];  // Keeps "0/1", "10,20,30", etc.
            }
        }
    }

    return format;
}

std::optional<double> VcfLineParser::try_parse_double(const std::string& str) const {
    try {
        size_t pos;
        double val = std::stod(str, &pos);
        // Ensure entire string was consumed (not just prefix)
        if (pos == str.size()) {
            return val;
        }
    } catch (...) {
        // Not a valid number
    }
    return std::nullopt;
}

}  // namespace vcf_tool::domain
