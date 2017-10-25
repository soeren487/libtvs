/*
 * Copyright (c) 2017 OFFIS Institute for Information Technology
 *                          Oldenburg, Germany
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

/**
 * \author Philipp Ittershagen <philipp.ittershagen@offis.de>
 * \brief  generic timed-value stream print processor class implementation
 */

#ifndef TVS_TIMED_STREAM_PRINT_PROCESSOR_H
#define TVS_TIMED_STREAM_PRINT_PROCESSOR_H

#include "tvs/tracing/processors/timed_stream_processor_base.h"

#include "tvs/tracing/timed_duration.h"

#include <ostream>

namespace tracing {

struct timed_value_base;

/**
 * \brief Simple stream sink for printing values to an output stream.
 */
struct timed_stream_print_processor : timed_stream_processor_base
{
  using this_type = timed_stream_print_processor;
  using base_type = timed_stream_processor_base;

  timed_stream_print_processor(char const* name, std::ostream& out);

  virtual ~timed_stream_print_processor();

protected:
  virtual void print_tuple(std::ostream& out, timed_value_base const&) = 0;

  /// Appends all available tokens for the given duration to the buffer.
  duration_type process(duration_type dur) override;

  std::ostream& output_;
};

} // namespace tracing

#endif /* TVS_TIMED_STREAM_PRINT_PROCESSOR_H */
