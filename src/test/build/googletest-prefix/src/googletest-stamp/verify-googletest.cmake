set(file "/home/yuan/Dropbox/whoAmI/mpm_implement/my_impl_mpm/src/test/build/googletest-prefix/src/release-1.7.0.tar.gz")
message(STATUS "verifying file...
     file='${file}'")
set(expect_value "4ff6353b2560df0afecfbda3b2763847")
set(attempt 0)
set(succeeded 0)
while(${attempt} LESS 3 OR ${attempt} EQUAL 3 AND NOT ${succeeded})
  file(MD5 "${file}" actual_value)
  if("${actual_value}" STREQUAL "${expect_value}")
    set(succeeded 1)
  elseif(${attempt} LESS 3)
    message(STATUS "MD5 hash of ${file}
does not match expected value
  expected: ${expect_value}
    actual: ${actual_value}
Retrying download.
")
    file(REMOVE "${file}")
    execute_process(COMMAND ${CMAKE_COMMAND} -P "/home/yuan/Dropbox/whoAmI/mpm_implement/my_impl_mpm/src/test/build/googletest-prefix/src/googletest-stamp/download-googletest.cmake")
  endif()
  math(EXPR attempt "${attempt} + 1")
endwhile()

if(${succeeded})
  message(STATUS "verifying file... done")
else()
  message(FATAL_ERROR "error: MD5 hash of
  ${file}
does not match expected value
  expected: ${expect_value}
    actual: ${actual_value}
")
endif()
