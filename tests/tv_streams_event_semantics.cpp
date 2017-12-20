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

#include "timed_stream_fixture.h"

#include "print_processor.h"

#include "tvs/tracing.h"

#include "gtest/gtest.h"

class StreamEventSemantics : public timed_stream_fixture_b
{
  using base_type = timed_stream_fixture_b;

  using value_type = std::set<int>;
  using traits_type = tracing::timed_event_traits<value_type>;

  using stream_type = tracing::timed_stream<value_type, traits_type>;
  using printer_type = test_event_printer<int>;

  // special event writer to simplify event handling with std::set<T>
  using writer_type = tracing::timed_event_writer<int>;

protected:
  StreamEventSemantics()
    : base_type()
    , writer("writer", tracing::STREAM_CREATE)
    , printer("printer")
  {
    printer.in(writer.name());
  }

  void expect_processor_output(std::string const& str)
  {
    std::stringstream actual;
    printer.print(actual);
    EXPECT_EQ(str, actual.str());
    printer.clear();
  }

  writer_type writer;
  printer_type printer;
};

TEST_F(StreamEventSemantics, PushRelEvent)
{
  // Push two events at the relative offset dur
  writer.push(0, dur);
  writer.push(10, dur);
  writer.commit();

  expect_processor_output("@1 s: { 0, 10 }\n");
}

TEST_F(StreamEventSemantics, PushAbsEvent)
{
  // Push two events to an absolute time point
  tracing::time_type abs{ dur };
  writer.push(0, abs);
  writer.push(10, abs);
  writer.commit();
  expect_processor_output("@1 s: { 0, 10 }\n");
}

TEST_F(StreamEventSemantics, PushSemantics)
{
  // Push two events at the relative offset dur
  writer.push(0, dur);
  writer.push(10, 2 * dur);
  writer.commit();
  expect_processor_output("@1 s: { 0 }\n"
                          "@2 s: { 10 }\n");
}

TEST_F(StreamEventSemantics, PartialCommit)
{
  // Push two events to an absolute time point which need to be 'merged'
  tracing::time_type abs{ dur };
  writer.push(0, abs);
  writer.push(10, abs / 2);

  // note that we use the absolute time commit interface here
  writer.commit(abs / 2);
  writer.commit(abs);

  // a split/merge results in decaying of the old event value to the 'right'
  // side while the duration and an empty value is created on the 'left' side of
  // a split
  expect_processor_output("@500 ms: { 10 }\n"
                          "@1 s: { 0 }\n");
}

TEST_F(StreamEventSemantics, SplitMergeSemantics)
{
  // Push two events to an absolute time point which need to be 'merged'
  tracing::time_type abs{ dur };
  writer.push(0, abs);
  writer.push(10, abs / 2);
  writer.commit();

  // a split/merge results in decaying of the old event value to the 'right'
  // side while the duration and an empty value is created on the 'left' side of
  // a split
  expect_processor_output("@500 ms: { 10 }\n"
                          "@1 s: { 0 }\n");
}

TEST_F(StreamEventSemantics, SplitMergeSemanticsTwo)
{
  // Push two events to an absolute time point which need to be 'merged'
  tracing::time_type abs{ dur };
  writer.push(0, abs);
  writer.push(5, abs);
  writer.push(10, abs / 2);
  writer.push(10, abs / 4);
  writer.commit();

  // we expect to have the events ordered correctly
  expect_processor_output("@250 ms: { 10 }\n"
                          "@500 ms: { 10 }\n"
                          "@1 s: { 0, 5 }\n");
}

TEST_F(StreamEventSemantics, SplitMergeSemanticsThree)
{
  tracing::time_type abs{ dur };
  writer.push(0, abs);
  writer.push(5, abs);
  writer.push(10, abs / 2);
  writer.push(10, abs / 4);

  // now, push again over the whole 'duration'
  writer.push(10, abs);
  writer.commit();

  expect_processor_output("@250 ms: { 10 }\n"
                          "@500 ms: { 10 }\n"
                          "@1 s: { 0, 5, 10 }\n");
}

TEST_F(StreamEventSemantics, SplitMergeCommitSemantics)
{
  // Push two events to an absolute time point which need to be 'merged'
  tracing::time_type abs{ dur };
  writer.push(0, abs);
  writer.push(5, abs);
  writer.push(10, abs / 2);
  writer.push(10, abs / 4);

  // now further split the stream by performing partial commits
  writer.commit(dur / 8);
  writer.commit(dur / 8);

  writer.commit(dur / 4);
  writer.commit(dur / 4);

  // the last commit should update up to the last event of the pushed events
  writer.commit();

  // we expect to have the events ordered correctly and have the splits at the
  // commit boundaries
  expect_processor_output("@125 ms: { - }\n"
                          "@250 ms: { 10 }\n"
                          "@500 ms: { 10 }\n"
                          "@750 ms: { - }\n"
                          "@1 s: { 0, 5 }\n");
}