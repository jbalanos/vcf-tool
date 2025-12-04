#pragma once

#include <vcf_tool/utils/Json.h>
#include <string>
#include <optional>
#include <stdexcept>

namespace vcf_tool {
namespace core {

/**
 * Config - Application configuration management
 *
 * Provides a typed interface for loading and accessing configuration
 * from JSON files or environment variables.
 */
class Config {
public:
    Config() = default;

    /**
     * Load configuration from JSON file
     */
    void load_from_file(const std::string& file_path) {
        config_ = utils::Json::load_from_file(file_path);
    }

    /**
     * Load configuration from JSON string
     */
    void load_from_string(const std::string& json_string) {
        config_ = utils::Json::parse(json_string);
    }

    /**
     * Get a required configuration value
     * Throws if the key doesn't exist
     */
    template<typename T>
    T get(const std::string& key) const {
        if (!config_.contains(key)) {
            throw std::runtime_error("Configuration key not found: " + key);
        }
        try {
            return config_[key].get<T>();
        } catch (const nlohmann::json::type_error& e) {
            throw std::runtime_error(
                std::string("Type mismatch for config key '") + key + "': " + e.what()
            );
        }
    }

    /**
     * Get an optional configuration value
     * Returns std::nullopt if the key doesn't exist
     */
    template<typename T>
    std::optional<T> get_optional(const std::string& key) const {
        if (!config_.contains(key)) {
            return std::nullopt;
        }
        try {
            return config_[key].get<T>();
        } catch (const nlohmann::json::type_error&) {
            return std::nullopt;
        }
    }

    /**
     * Get a configuration value with a default fallback
     */
    template<typename T>
    T get_or(const std::string& key, const T& default_value) const {
        return get_optional<T>(key).value_or(default_value);
    }

    /**
     * Set a configuration value
     */
    template<typename T>
    void set(const std::string& key, const T& value) {
        config_[key] = value;
    }

    /**
     * Check if a configuration key exists
     */
    bool has(const std::string& key) const {
        return config_.contains(key);
    }

    /**
     * Get the underlying JSON object for advanced operations
     */
    const utils::Json::JsonValue& raw() const {
        return config_;
    }

    /**
     * Save configuration to file
     */
    void save_to_file(const std::string& file_path) const {
        utils::Json::save_to_file(config_, file_path);
    }

private:
    utils::Json::JsonValue config_ = utils::Json::object();
};

} // namespace core
} // namespace vcf_tool