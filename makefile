# -*- coding: utf8 -*-

FILE_PATH := $(abspath $(lastword $(MAKEFILE_LIST)))
PROJECT_DIR := $(shell dirname $(FILE_PATH))
PROJECT_NAME := $(notdir $(patsubst %/,%,$(dir $(FILE_PATH))))
BUILD_DIR ?= "$(PROJECT_DIR)/staging"
BUILD_MODE ?= Release # Either Debug or Release
BUILD_SHARED_LIBS ?= ON

default: all

debug:
	@echo "FILE_PATH: $(FILE_PATH)"
	@echo "PROJECT_DIR: $(PROJECT_DIR)"
	@echo "PROJECT_NAME: $(PROJECT_NAME)"

clean:
	@rm -rf "$(BUILD_DIR)"

prepare:
	@mkdir -p "$(BUILD_DIR)"
	@cmake -B $(BUILD_DIR) -D CMAKE_BUILD_TYPE=${BUILD_MODE} \
		-D BUILD_SHARED_LIBS=${BUILD_SHARED_LIBS} -S $(PROJECT_DIR)

stage: prepare
	@(cd $(BUILD_DIR) && cmake ${TEST_FLAGS} -D CMAKE_BUILD_TYPE=${BUILD_MODE} -D BUILD_SHARED_LIBS=ON ..)

build:
	@make -C "$(BUILD_DIR)"

docs: clean-docs icon
	doxygen docs/doxygen.cfg > docs/doxygen.log 2> docs/doxygen.err.log

open-docs:
	open docs/html/index.html

clean-docs:
	rm -rf docs/{html,latex}
	rm -f docs/doxygen*.log

icon: prepare
	convert -background white -fill black \
	  -size 128x64 -gravity center label:".:* qrencode *:." "$(PROJECT_DIR)/docs/icon.png"

all: stage build