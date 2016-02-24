# Image_Filters




如何在android 上使用ndk 呢？

1.首先 android studio 內需要灌ndk 

2.在build.gradle 內加入

ndk {
            moduleName "JniTest"
            ldLibs "m", "log", "jnigraphics"
        }
        
        
3.在app資料夾下創建一個jni 這是專門放c/c++ code的地方


4.我們需要創建一個Android.mk 的一個檔案內容如下


LOCAL_PATH := $(call my-dir)
include $(CLEAR_VARS)

LOCAL_MODULE := JniTest
LOCAL_SRC_FILES := JniTest.c
LOCAL_LDLIBS    := -llog -ljnigraphics

  include $(BUILD_SHARED_LIBRARY)

5.在jni內在創建一個Application.mk 

APP_ABI := all
APP_PLATFORM :=android-8

6.最後就是放入我們需要寫c/c++的code 的class

7.在Activity 內加入導入c的class名字和class 內的fucntion

    static {
        System.loadLibrary("JniTest");
    }

 public native void convertToGray(Bitmap bitmapIn,Bitmap bitmapOut);
 



