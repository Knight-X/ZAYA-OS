################################################################################
#
# @file module.mk
#
# @author Murat Cakmak
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
CPU_SRC_FILES := $(CPU_PATH)/internal/startup_ARMCM3.s $(shell /usr/bin/find $(CPU_PATH) -mindepth 0 -maxdepth 3 -name "*.c" ! -path "*UnitTest*")

MODULE_INC_PATHS += \
	-I$(CPU_PATH)/internal
