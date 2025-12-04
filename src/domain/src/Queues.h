#pragma once

#include <moodycamel/blockingconcurrentqueue.h>
#include "entity/RawLine.h"
#include "entity/ParsedRecord.h"


namespace vcf_tool::domain {

using entity::RawLine;
using entity::ParsedRecord;

// Thread-safe blocking queues for pipeline communication
using LineQueue   = moodycamel::BlockingConcurrentQueue<RawLine>;
using RecordQueue = moodycamel::BlockingConcurrentQueue<ParsedRecord>;

} // namespace vcf_tool::domain
