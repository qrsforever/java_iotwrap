---

title: LeIOT For TV
date: 2018-03-26 18:43:30
tags: [ IOT ]
categories: [ Local ]

---

<!-- vim-markdown-toc GFM -->

* [代码调试](#代码调试)
    * [下载及编译](#下载及编译)
    * [目录结构介绍](#目录结构介绍)
    * [调试](#调试)

<!-- vim-markdown-toc -->


-----------------------------------------------------------

代码调试
========

下载及编译
----------
地址:`http://athena.devops.letv.com/#/admin/projects/smarthome/LeIOT`
编译: Android环境下`mm`
位置:`vendor/letv/frameworks/LeIOT`

目录结构介绍
-----------
```
    .
    |-- Android.mk            ------ 编译framework/leiot.jar脚本
    |-- jni
    |   |-- Android.mk        ------ 编译iot_server, client_jni.so脚本
    |   |-- binder            ------ server/client 通讯的binder接口
    |   |-- client            ------ 对client jni封装
    |   |-- IOT_JNI.cpp       ------ cpp <--> java 交互桥梁
    |   |-- sdk               ------ 从 Ares release出的sdk包
    |   |   |-- certs         ------ 测试时用的证书文件
    |   |   |-- include       ------ c-sdk头文件
    |   |   `-- lib           ------ c-sdk库文件
    |   `-- server            ------ System/bin/iot_server源码
    |-- prebuilt.mk           ------ Product预装设置
    |-- README.md
    `-- src                   ------ system/framework/leiot.jar 源码
        `-- com
                `-- android
                            `-- leiot
                                `-- DeviceShadow.java
```

调试
----

```bash
export TOP=/data/source/mainline/mstar938
adb connect 10.58.83.7
adb root
adb connect 10.58.83.7
adb remount
adb push $TOP/out/target/product/almond/system/framework/leiot.jar /system/framework/
adb push $TOP/out/target/product/almond/system/lib/libiotclient_jni.so /system/lib 
adb push $TOP/out/target/product/almond/system/lib64/libiotclient_jni.so /system/lib64
adb push $TOP/out/target/product/mangosteen/symbols/system/bin/iot_server /system/bin/

```
dex转化为jar
`0jdecode.sh $TOP/out/target/common/obj/JAVA_LIBRARIES/leiot_intermediates/classes.dex`
