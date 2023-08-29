function(add_sanitizers this_target)
  target_compile_options(${this_target} INTERFACE -fsanitize=address)
  target_link_options(${this_target} INTERFACE -fsanitize=address)
  target_compile_options(${this_target} INTERFACE -fsanitize=undefined)
  target_link_options(${this_target} INTERFACE -fsanitize=undefined)
endfunction()

function(setup_project this_target)

# This warning has a false positive with clang. See
# <https://stackoverflow.com/questions/52416362>.
if (CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    target_compile_options(${this_target} INTERFACE -Wno-error=unused-lambda-capture)
endif()

# If we're building with clang, then use the libc++ version of the standard
# library instead of libstdc++. Better coverage of build configurations.
#
# But there's one exception: libfuzzer is built with libstdc++ on Ubuntu,
# and so won't link to libc++. So, if any of the FUZZ_* variables are set,
# keep to libstdc++ (the default on most systems).
if ((CMAKE_CXX_COMPILER_ID STREQUAL "Clang") AND (NOT ${FUZZ_W3C_PROPAGATION}))
    target_compile_options(${this_target} INTERFACE -stdlib=libc++)
    target_link_options(${this_target} INTERFACE -stdlib=libc++)
endif()

if (SANITIZE)
  add_sanitizers(${this_target})
endif()

if (MSVC)
    target_compile_options(${this_target} INTERFACE /W4 /WX)
else()
    target_compile_options(${this_target} INTERFACE -Wall -Wextra -pedantic -Werror)
    if (BUILD_COVERAGE)
      target_compile_options(${this_target} INTERFACE -g -O0 -fprofile-arcs -ftest-coverage)
    endif()
endif()


endfunction()