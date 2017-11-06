# Copyright 2014-2015 Willi Mann
#
# This file is part of set_sim_join.
#
# set_sim_join is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# Foobar is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with set_sim_join.  If not, see <http://www.gnu.org/licenses/>.



set(STATVERSIONS 1)
set(EXT_STATISTICS 1)
set(NOSTAT 1)


cmake_minimum_required(VERSION 2.8)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

if(PROFILE_GEN)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pg -fprofile-generate")
endif()

if(PROFILE_USE)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fprofile-use")
endif()

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake-modules/")
include(AddTargetDefinitions)
include(GetGitRevisionDescription)
get_git_head_revision(GIT_REFSPEC GIT_SHA1)

find_package( Boost 1.40 COMPONENTS program_options REQUIRED )
include_directories( ${Boost_INCLUDE_DIR} )
include_directories( ${CMAKE_CURRENT_BINARY_DIR} )

include(CheckIncludeFileCXX)

Check_INCLUDE_FILE_CXX(city.h USECITYHASH)

CONFIGURE_FILE(${CMAKE_CURRENT_SOURCE_DIR}/config.h.in ${CMAKE_CURRENT_BINARY_DIR}/config.h)

if(FORCE_REVISION)
	set(GIT_SHA1 ${FORCE_REVISION})
endif()

configure_file("${CMAKE_CURRENT_SOURCE_DIR}/GitSHA1.cxx.in" "${CMAKE_CURRENT_BINARY_DIR}/GitSHA1.cpp" @ONLY)

set(setsimjoinfiles main.cxx input.cxx statistics.cxx timing.cxx
		cmdline_adaptjoin.cxx
		cmdline_allpairs.cxx
		cmdline_groupjoin.cxx
		cmdline_groupjoin_jaccard.cxx
		cmdline_groupjoin_cosine.cxx
		cmdline_groupjoin_dice.cxx
		cmdline_groupjoin_hamming.cxx
		cmdline_mpjoin.cxx
		cmdline_mpjoin_jaccard.cxx
		cmdline_mpjoin_cosine.cxx
		cmdline_mpjoin_dice.cxx
		cmdline_mpjoin_hamming.cxx
		lsh.cxx
		${CMAKE_CURRENT_BINARY_DIR}/GitSHA1.cpp)

add_executable(set_sim_join ${setsimjoinfiles})
target_link_libraries( set_sim_join ${Boost_LIBRARIES}  )

add_executable(manverify manverify.cxx)
if(EXT_STATISTICS)
	add_target_definitions(manverify EXT_STATISTICS=1)
endif()
# Don't use this unless you want to waste CPU time
# (or you'd like to see the difference between efficient verification and the
# verification routine used in the ppjoin source code)
if(LONG_VERIFICATION)
	# Attention: This wastes CPU time
	add_definitions(-DLONG_VERIFICATION=1)
endif()

if(PRINT_VERIFICATION_CALLS)
	# Attention: This wastes CPU time
	add_definitions(-DPRINT_VERIFICATION_CALLS=1)
endif()

if(MPJ_LINKEDLIST)
	add_definitions(-DMPJ_LINKEDLIST)
endif()

if(STATVERSIONS)
	add_executable(set_sim_join_candonly ${setsimjoinfiles})
	add_target_definitions(set_sim_join_candonly CAND_ONLY=1 NO_STAT_COUNTERS=1)
	target_link_libraries( set_sim_join_candonly ${Boost_LIBRARIES}  )
	
	add_executable(set_sim_join_cycles ${setsimjoinfiles})
	add_target_definitions(set_sim_join_cycles CYCLE_COUNT_SF=1 CYCLE_COUNT=1 VERIFICATION=1)
	target_link_libraries( set_sim_join_cycles ${Boost_LIBRARIES}  )

	if(USECITYHASH)
		target_link_libraries( set_sim_join_candonly  cityhash )
		target_link_libraries( set_sim_join_cycles  cityhash )
	endif()
endif()

if(NOSTAT)
	add_executable(set_sim_join_nostat ${setsimjoinfiles})
	add_target_definitions(set_sim_join_nostat NO_STAT_COUNTERS=1)
	target_link_libraries( set_sim_join_nostat ${Boost_LIBRARIES}  )
	
	if(USECITYHASH)
		target_link_libraries( set_sim_join_nostat  cityhash )
	endif()
endif()

if(EXT_STATISTICS)
	add_executable(set_sim_join_ext_stat ${setsimjoinfiles})
	add_target_definitions(set_sim_join_ext_stat EXT_STATISTICS=1)
	target_link_libraries( set_sim_join_ext_stat ${Boost_LIBRARIES}  )

	if(USECITYHASH)
		target_link_libraries( set_sim_join_ext_stat  cityhash )
	endif()
endif()

if(USECITYHASH)
	target_link_libraries( set_sim_join  cityhash )
endif()
