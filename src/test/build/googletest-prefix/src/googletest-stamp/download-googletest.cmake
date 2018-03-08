if(EXISTS "/home/yuan/Dropbox/whoAmI/mpm_implement/my_impl_mpm/src/test/build/googletest-prefix/src/release-1.7.0.tar.gz")
  file("MD5" "/home/yuan/Dropbox/whoAmI/mpm_implement/my_impl_mpm/src/test/build/googletest-prefix/src/release-1.7.0.tar.gz" hash_value)
  if("x${hash_value}" STREQUAL "x4ff6353b2560df0afecfbda3b2763847")
    return()
  endif()
endif()
message(STATUS "downloading...
     src='https://github.com/google/googletest/archive/release-1.7.0.tar.gz'
     dst='/home/yuan/Dropbox/whoAmI/mpm_implement/my_impl_mpm/src/test/build/googletest-prefix/src/release-1.7.0.tar.gz'
     timeout='none'")




file(DOWNLOAD
  "https://github.com/google/googletest/archive/release-1.7.0.tar.gz"
  "/home/yuan/Dropbox/whoAmI/mpm_implement/my_impl_mpm/src/test/build/googletest-prefix/src/release-1.7.0.tar.gz"
  SHOW_PROGRESS
  # no TIMEOUT
  STATUS status
  LOG log)

list(GET status 0 status_code)
list(GET status 1 status_string)

if(NOT status_code EQUAL 0)
  message(FATAL_ERROR "error: downloading 'https://github.com/google/googletest/archive/release-1.7.0.tar.gz' failed
  status_code: ${status_code}
  status_string: ${status_string}
  log: ${log}
")
endif()

message(STATUS "downloading... done")
