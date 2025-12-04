#pragma once

#include "../entity/RawLine.h"
#include "../entity/ParsedRecord.h"


namespace vcf_tool::domain::parser {

using entity::ParsedRecord;
using entity::RawLine;

/**
 * @brief Simple TAB-delimited line parser for VCF format
 *
 * Splits input lines on TAB characters and extracts fields.
 * Suitable for basic VCF parsing without validation.
 */
struct NaiveLineParser {
    /**
     * @brief Parse a raw line into structured fields
     * @param raw The raw input line to parse
     * @return ParsedRecord containing extracted fields
     */
    ParsedRecord operator()(const RawLine& raw) const;
};

} // namespace vcf_tool::domain::parser
