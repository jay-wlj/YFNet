APP_PROJECT_PATH := $(call my-dir)/../

#APP_MODULES := libyfnet
APP_ABI := armeabi armeabi-v7a arm64-v8a
#armeabi-v7a x86
APP_STL := stlport_static
#用下面的编译工具可以混淆代码，防止反汇编
NDK_TOOLCHAIN_VERSION := clang3.5
