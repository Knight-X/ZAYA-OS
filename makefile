################################################################################
#
# @file makefile
#
# @author Murat Cakmak (MC)
#
# @brief Main makefile of build system
#
#	Build system can be managed through this file. 
#	
#		- Build a Project
#			[USAGE] : 
#				make PROJECT=<PROJECT_NAME>
#		
#			Builds a project. Uses project.mk file under project 
#			directory to get project configurations. 
#
#		- Run a Unit test
#			[USAGE] : 
#				make unittest TEST_MODULE=<UNIT_NAME>
#		
#			Builds and runs a Unit test. Uses unittest.mk file under unittest 
#			directory to get test configurations. 
#
#			 	> Runs and prints Unit Test Resuts (PASS/FAIL)
#			 	> Runs and prints Code Coverage Results (% of coverage)
#
#		- Check All System Stability
#			[USAGE] : 
#				make check_all
#
#			Tests and checks all system.
#				> Builds all projects under Projects directory. 
#				> Runs all unittests, code coverage, static analysis
#				> Runs all Integration Tests
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

################################################################################
#                    		  DEFINITIONS                                      #
################################################################################

#
# Root Path 
#
ROOT_PATH = .

# Path which includes all makefiles
MAKE_FILES_PATH = Environment/BuildSystem

#
# Detailed Compiling output
#
ifndef V
SILENCE = -s
else
SILENCE =
endif

################################################################################
#                    		     RULES                                   	   #
################################################################################

#
# Default Rule. Builds a Projects
#
default:
	make -f $(MAKE_FILES_PATH)/build_project.mk PROJECT=$(PROJECT) $(SILENCE)
#
# Builds and Runs a Unit Test
#
unittest:
	make -f $(MAKE_FILES_PATH)/execute_unittest.mk TEST_MODULE=$(TEST_MODULE) $(SILENCE)

#
# Builds and Runs all system validation objects.
#
check_all: clean
	make -f $(MAKE_FILES_PATH)/execute_systemcheck.mk $(SILENCE)

#
# Cleans build output 
#
clean:
	@echo "Cleaned..."
	rm -rf out
