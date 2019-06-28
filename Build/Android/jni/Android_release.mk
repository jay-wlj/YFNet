
LOCAL_PATH:= $(call my-dir)
MY_PATH:= $(LOCAL_PATH)/../../../Src

include $(CLEAR_VARS)
# �������ѱ���õ�libcryptoģ��
LOCAL_MODULE	:= libcrypto
# ��дԴ�ļ�����ʱ��Ҫ�Ѿ�̬���̬����ļ�����д������
# $(TARGET_ARCH_ABI)/ ��ʾ����ͬ�ܹ��µĿ��ļ���ŵ���Ӧ�ܹ�Ŀ¼��
LOCAL_SRC_FILES	:= ./../../../Lib/Android/Libs/$(TARGET_ARCH_ABI)/libcrypto.a
# ����Ԥ������̬�⣨����ɱ����ӣ�
include $(PREBUILT_STATIC_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE	:= libssl
LOCAL_SRC_FILES	:= ./../../../Lib/Android/Libs/$(TARGET_ARCH_ABI)/libssl.a
include $(PREBUILT_STATIC_LIBRARY)



include $(CLEAR_VARS)

MY_PATH:= $(LOCAL_PATH)/../../../Src

LOCAL_MODULE    := libYfNetPlush
LOCAL_C_INCLUDES += $(NDK)/sources/cxx-stl/stlport/stlport/
LOCAL_C_INCLUDES += $(MY_PATH)/include/
LOCAL_C_INCLUDES += $(MY_PATH)/include/json
LOCAL_C_INCLUDES += $(MY_PATH)/Common/
LOCAL_C_INCLUDES += $(MY_PATH)/httpagent/
LOCAL_C_INCLUDES += $(MY_PATH)/httpupload/
LOCAL_C_INCLUDES += $(MY_PATH)/httpserver/
LOCAL_C_INCLUDES += $(MY_PATH)/msgpool/
LOCAL_C_INCLUDES += $(MY_PATH)/qvodbase/
LOCAL_C_INCLUDES += $(MY_PATH)/taskmgr/
LOCAL_C_INCLUDES += $(MY_PATH)/YfNetPlush/
LOCAL_C_INCLUDES += $(MY_PATH)/zlib-1.2.8/
LOCAL_C_INCLUDES += $(MY_PATH)/AES


LOCAL_CPPFLAGS += -D_STLP_USE_SIMPLE_NODE_ALLOC -D_FILE_OFFSET_BITS=64 -DANDROID -DYUNFAN_LIVE -DAIPAI -DSUPPORT_HTTPS -Wno-error=format-security
LOCAL_CPPFLAGS += -fexceptions

MY_CPP_LIST := $(wildcard $(LOCAL_PATH)/*.cpp $(LOCAL_PATH)/*.c)
MY_CPP_LIST += $(wildcard $(MY_PATH)/Common/*.cpp $(MY_PATH)/Common/*.c)
MY_CPP_LIST += $(wildcard $(MY_PATH)/include/json/*.cpp $(MY_PATH)/Common/*.c)
MY_CPP_LIST += $(wildcard $(MY_PATH)/httpagent/*.cpp $(MY_PATH)/httpagent/*.c)
MY_CPP_LIST += $(wildcard $(MY_PATH)/httpupload/*.cpp $(MY_PATH)/httpupload/*.c)
MY_CPP_LIST += $(wildcard $(MY_PATH)/httpserver/*.cpp $(MY_PATH)/httpserver/*.c)
MY_CPP_LIST += $(wildcard $(MY_PATH)/msgpool/*.cpp $(MY_PATH)/msgpool/*.c)
MY_CPP_LIST += $(wildcard $(MY_PATH)/qvodbase/*.cpp $(MY_PATH)/qvodbase/*.c)
MY_CPP_LIST += $(wildcard $(MY_PATH)/taskmgr/*.cpp $(MY_PATH)/taskmgr/*.c)
MY_CPP_LIST += $(wildcard $(MY_PATH)/YfNetPlush/*.cpp $(MY_PATH)/YfNetPlush/*.c)
MY_CPP_LIST += $(wildcard $(MY_PATH)/zlib-1.2.8/*.cpp $(MY_PATH)/zlib-1.2.8/*.c)
MY_CPP_LIST += $(wildcard $(MY_PATH)/AES/*.cpp $(MY_PATH)/AES/*.c)

LOCAL_SRC_FILES := $(MY_CPP_LIST:$(LOCAL_PATH)/%=%)


# ����ǰ�������õľ�̬��
LOCAL_STATIC_LIBRARIES := libssl libcrypto 
LOCAL_CFLAGS := -DMONOLITH
include $(LOCAL_PATH)/android-config.mk
LOCAL_CFLAGS += -DOPENSSL_NO_DTLS1
LOCAL_CFLAGS += -DANDROID

LOCAL_LDLIBS    := -L$(SYSROOT)/usr/lib -llog -L./jni 
LOCAL_LDLIBS += -lz

include $(BUILD_SHARED_LIBRARY)
