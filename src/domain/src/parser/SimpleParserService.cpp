#include "SimpleParserService.h"

#include "../entity/RawLine.h"
#include "../entity/ParsedRecord.h"
#include "NaiveLineParser.h"
#include "VcfLineParser.h"


namespace vcf_tool::domain::parser {

using entity::ParsedRecord;
using entity::RawLine;

template<typename Parser>
void SimpleParserService<Parser>::operator()() {
    RawLine raw;

    for (;;) {
        this->input_queue.wait_dequeue(raw);

        if (raw.is_end) {
            // Propagate sentinel downstream
            ParsedRecord sentinel{};
            sentinel.is_end = true;
            this->output_queue.enqueue(std::move(sentinel));
            break;
        }

        ParsedRecord rec = this->parser(raw);
        this->output_queue.enqueue(std::move(rec));
    }
}

// Explicit template instantiations for the parsers we use
template struct SimpleParserService<NaiveLineParser>;
template struct SimpleParserService<::vcf_tool::domain::VcfLineParser>;

} // namespace vcf_tool::domain::parser
