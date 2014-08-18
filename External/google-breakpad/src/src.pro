TEMPLATE = lib
CONFIG += staticlib warn_on thread

include(../google-breakpad.pri)

INCLUDEPATH += $$PWD/client
win32:INCLUDEPATH += $$WIN_DIA_SDK_DIR/include

# Common Unix sources
unix:SOURCES += \
	client/minidump_file_writer.cc \
	common/convert_UTF.c \
	common/md5.cc \
	common/string_conversion.cc \
	processor/basic_code_modules.cc \
	processor/basic_source_line_resolver.cc \
	processor/call_stack.cc \
	processor/logging.cc \
	processor/minidump.cc \
	processor/minidump_processor.cc \
	processor/pathname_stripper.cc \
	processor/process_state.cc \
	processor/simple_symbol_supplier.cc \
	processor/stackwalker.cc \
	processor/stackwalker_amd64.cc \
	processor/stackwalker_ppc.cc \
	processor/stackwalker_sparc.cc \
	processor/stackwalker_x86.cc

           # Tests
           # client/minidump_file_writer_unittest.cc \
           # processor/address_map_unittest.cc \
           # processor/basic_source_line_resolver_unittest.cc
           # processor/contained_range_map_unittest.cc \
           # processor/minidump_dump.cc \
           # processor/minidump_processor_unittest.cc \
           # processor/minidump_stackwalk.cc \
           # processor/pathname_stripper_unittest.cc \
           # processor/postfix_evaluator_unittest.cc \
           # processor/range_map_unittest.cc \
           # processor/stackwalker_selftest.cc


# Mac sources
macx:SOURCES += \
           client/mac/handler/breakpad_nlist_64.cc \
           client/mac/handler/dynamic_images.cc \
           client/mac/handler/exception_handler.cc \
           client/mac/handler/minidump_generator.cc \
           client/mac/handler/protected_memory_allocator.cc \
           common/mac/dwarf/bytereader.cc \
           common/mac/dwarf/dwarf2reader.cc \
           common/mac/dwarf/functioninfo.cc \
           common/mac/file_id.cc \
           common/mac/macho_id.cc \
           common/mac/macho_utilities.cc \
           common/mac/macho_walker.cc \
           common/mac/string_utilities.cc

           # Tests
           # client/mac/handler/exception_handler_test.cc \
           # client/mac/handler/minidump_generator_test.cc \
           # client/mac/handler/testcases/DynamicImagesTests.cc \
           # client/mac/handler/testcases/breakpad_nlist_test.cc

# Linux sources
linux-*:SOURCES = \
	client/linux/crash_generation/crash_generation_client.cc \
	client/linux/crash_generation/crash_generation_server.cc \
	client/linux/handler/exception_handler.cc \
	client/linux/handler/minidump_descriptor.cc \
	client/linux/log/log.cc \
	client/linux/minidump_writer/linux_dumper.cc \
	client/linux/minidump_writer/linux_ptrace_dumper.cc \
	client/linux/minidump_writer/minidump_writer.cc \
	client/minidump_file_writer.cc \
	client/linux/handler/lightcore_generator.cc \
	common/convert_UTF.c \
	common/string_conversion.cc \
	common/linux/elfutils.cc \
	common/linux/file_id.cc \
	common/linux/guid_creator.cc \
	common/linux/linux_libc_support.cc \
	common/linux/memory_mapped_file.cc \
	common/linux/safe_readlink.cc

          # HTTP Upload (curl), required by tools
          # common/linux/http_upload.cc

          # Tools
          # tools/linux/dump_syms/dump_syms.cc \
          # tools/linux/symupload/minidump_upload.cc \
          # tools/linux/symupload/sym_upload.cc

          # Tests
          # client/linux/handler/exception_handler_test.cc \
          # client/linux/handler/linux_thread_test.cc \
          # client/linux/handler/minidump_test.cc


# Windows sources
win32:SOURCES += \
          client/windows/crash_generation/client_info.cc \
          client/windows/crash_generation/crash_generation_client.cc \
          client/windows/crash_generation/crash_generation_server.cc \
          client/windows/crash_generation/minidump_generator.cc \
          client/windows/handler/exception_handler.cc \
          client/windows/sender/crash_report_sender.cc \
          common/windows/guid_string.cc \
          common/windows/http_upload.cc \
          common/windows/pdb_source_line_writer.cc \
          common/windows/string_utils.cc

          # Tools
          # tools/windows/converter/ms_symbol_server_converter.cc \
          # tools/windows/dump_syms/dump_syms.cc \
          # tools/windows/symupload/symupload.cc

          # Tests
          # client/windows/tests/crash_generation_app/abstract_class.cc \
          # client/windows/tests/crash_generation_app/crash_generation_app.cc\
          # client/windows/tests/crash_generation_app/precompile.cc \
          # tools/windows/dump_syms/testdata/dump_syms_regtest.cc

QMAKE_CFLAGS -= -Werror
QMAKE_CXXFLAGS -= -Werror
