
#include "producer.h"

#include "gtest/gtest.h"

#include "../print_processor.h"

int
#ifdef SYSX_NO_SYSTEMC
main(int argc, char* argv[])
#else
sc_main(int argc, char* argv[])
#endif
{

  using printer_type = ::test_event_printer<producer::events>;

  printer_type printer(printer_type::OUTPUT_COUT);

  producer prod;

  printer.in(prod.writer_.name());

  prod.run();

  return 0;
}
