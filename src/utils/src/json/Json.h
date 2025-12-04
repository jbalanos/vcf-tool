#pragma once

#include <nlohmann/json.hpp>
#include <string>
#include <fstream>
#include <stdexcept>

namespace vcf_tool {
namespace utils {

/**
 * Json - Generic wrapper around nlohmann::json for JSON operations
 *
 * This provides a cleaner interface and hides the nlohmann::json
 * implementation from the rest of the codebase.
 */
class Json {
public:
    using JsonValue = nlohmann::json;

    /**
     * Parse JSON from string
     */
    static JsonValue parse(const std::string& json_string) {
        try {
            return JsonValue::parse(json_string);
        } catch (const nlohmann::json::parse_error& e) {
            throw std::runtime_error(std::string("JSON parse error: ") + e.what());
        }
    }

    /**
     * Load JSON from file
     */
    static JsonValue load_from_file(const std::string& file_path) {
        std::ifstream file(file_path);
        if (!file.is_open()) {
            throw std::runtime_error("Failed to open file: " + file_path);
        }

        try {
            JsonValue j;
            file >> j;
            return j;
        } catch (const nlohmann::json::parse_error& e) {
            throw std::runtime_error(
                std::string("Failed to parse JSON from file ") + file_path + ": " + e.what()
            );
        }
    }

    /**
     * Save JSON to file
     */
    static void save_to_file(const JsonValue& json, const std::string& file_path, int indent = 2) {
        std::ofstream file(file_path);
        if (!file.is_open()) {
            throw std::runtime_error("Failed to open file for writing: " + file_path);
        }

        file << json.dump(indent);
    }

    /**
     * Convert JSON to string
     */
    static std::string to_string(const JsonValue& json, int indent = -1) {
        return json.dump(indent);
    }

    /**
     * Create empty JSON object
     */
    static JsonValue object() {
        return JsonValue::object();
    }

    /**
     * Create empty JSON array
     */
    static JsonValue array() {
        return JsonValue::array();
    }
};

} // namespace utils
} // namespace vcf_tool
