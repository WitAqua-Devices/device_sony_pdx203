#
# Copyright (C) 2018 The LineageOS Project
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#      http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
#

# Inherit from sony sm8250-common
-include device/sony/sm8250-common/BoardConfigCommon.mk

DEVICE_PATH := device/sony/pdx203

# Display
TARGET_SCREEN_DENSITY := 420

BOARD_KERNEL_CMDLINE += buildproduct=pdx203

TARGET_KERNEL_SOURCE := kernel/sony/sm8250
TARGET_KERNEL_CONFIG += vendor/pdx203.config

BOARD_MKBOOTIMG_ARGS += --base $(BOARD_KERNEL_BASE)
BOARD_MKBOOTIMG_ARGS += --pagesize $(BOARD_KERNEL_PAGESIZE)
BOARD_MKBOOTIMG_ARGS += --ramdisk_offset $(BOARD_RAMDISK_OFFSET)
BOARD_MKBOOTIMG_ARGS += --tags_offset $(BOARD_KERNEL_TAGS_OFFSET)
BOARD_MKBOOTIMG_ARGS += --kernel_offset $(BOARD_KERNEL_OFFSET)
BOARD_MKBOOTIMG_ARGS += --second_offset $(BOARD_KERNEL_SECOND_OFFSET)
BOARD_MKBOOTIMG_ARGS += --dtb_offset $(BOARD_DTB_OFFSET)
BOARD_MKBOOTIMG_ARGS += --header_version $(BOARD_BOOT_HEADER_VERSION)

# OTA
# ro.product.device is overridden to XQ-AT52 for the stock fingerprint, so an
# A/B package ends up with pre-device=XQ-AT52 and recovery turns it down on
# anything reporting a different name - most notably a device still on its
# stock japanese firmware, which calls itself SO-51A or SOG01. All of these are
# the same pdx203 board.
TARGET_OTA_ASSERT_DEVICE := pdx203,XQ-AT42,XQ-AT51,XQ-AT52,SO-51A,SOG01

# Props
TARGET_VENDOR_PROP += $(DEVICE_PATH)/vendor.prop

# inherit from the proprietary version
-include vendor/sony/pdx203/BoardConfigVendor.mk
