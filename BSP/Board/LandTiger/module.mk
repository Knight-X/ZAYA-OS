################################################################################
#
# @file module.mk
#
# @author Murat Cakmak (MC)
#
# @brief Module make file
#
# @see https://github.com/ZA-YA/ZAYA-OS/wiki
#
#*****************************************************************************
#
# GNU GPLv2
#
# Copyright (c) 2016 ZAYA
#
#  See GNU GPLv2 License Details in the Root Directory.
#
#*****************************************************************************/

#
# Get all source files (.c files) using 'find' command except UnitTest folder
#
BOARD_SRC_FILES := $(shell /usr/bin/find $(BOARD_PATH) -mindepth 0 -maxdepth 6 -name "*.c" ! -path "*UnitTest*")
