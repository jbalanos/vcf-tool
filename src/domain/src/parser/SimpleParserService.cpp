#include "SimpleParserService.h"

#include "../entity/RawLine.h"
#include "../entity/ParsedRecord.h"


namespace vcf_tool::domain::parser {

using entity::ParsedRecord;
using entity::RawLine;

void SimpleParserService::operator()() {
    RawLine raw;

    for (;;) {
        input_queue.wait_dequeue(raw);

        if (raw.is_end) {
            // Propagate sentinel downstream
            ParsedRecord sentinel{};
            sentinel.is_end = true;
            output_queue.enqueue(std::move(sentinel));
            break;
        }

        ParsedRecord rec = parser(raw);
        output_queue.enqueue(std::move(rec));
    }
}

} // namespace vcf_tool::domain::parser
