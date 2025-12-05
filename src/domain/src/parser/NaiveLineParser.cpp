#include "NaiveLineParser.h"

#include <sstream>
#include <iostream>


namespace vcf_tool::domain::parser {

ParsedRecord NaiveLineParser::operator()(const RawLine& raw) const {
    ParsedRecord result;
    result.line_number = raw.line_number;
    result.raw_text    = raw.text;
    result.is_end      = raw.is_end;

    // Split on TAB delimiter (VCF format)
   // std::istringstream iss(raw.text);
   // std::string token;
   // while (std::getline(iss, token, '\t')) {
   //     result.fields.push_back(std::move(token));
    //}

    // Temporary stdout logging for testing
    if (!raw.is_end) {
        std::cout << "[NaiveParser] -- " << raw.line_number << " -- " << raw.text << "\n";
    }

    return result;
}

} // namespace vcf_tool::domain::parser
