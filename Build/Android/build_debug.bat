@echo off
ndk-build NDK_PROJECT_PATH=. APP_BUILD_SCRIPT=jni/Android_debug.mk NDK_DEBUG=1 APP_OPTIM=debug NDK_APP_OUT=./obj/debug NDK_APP_LIBS_OUT=../../Bin/Android/D
