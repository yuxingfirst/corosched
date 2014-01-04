PROJECT (COROSCHED)
cmake_minimum_required(VERSION 2.6)
ADD_DEFINITIONS(
-O0 
-g 
-W 
-DCORO_ASM -DCS_ASSERT_PANIC
)
SET(LIBRARY_OUTPUT_PATH lib)
AUX_SOURCE_DIRECTORY(. DIR_SRCS)
ADD_LIBRARY(corosched_s STATIC ${DIR_SRCS})
ADD_LIBRARY(corosched SHARED ${DIR_SRCS})
SET_TARGET_PROPERTIES(corosched PROPERTIES VERSION 1.2 SOVERSION 1)
INSTALL(TARGETS corosched corosched_s LIBRARY DESTINATION lib ARCHIVE DESTINATION lib)
INSTALL(FILES cs_common.h cs_coro.h cs_coroutine.h cs_log.h cs_queue.h cs_scheduler.c cs_scheduler.h cs_string.h cs_util.h DESTINATION include)

