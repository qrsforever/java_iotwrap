---

title: LeIOT For TV
date: 2018-03-26 18:43:30
tags: [ IOT ]
categories: [ Local ]

---

<!-- vim-markdown-toc GFM -->

* [代码调试](#代码调试)
    * [代码下载](#代码下载)
    * [目录结构介绍](#目录结构介绍)
    * [编译](#编译)
    * [调试](#调试)
    * [IOT服务](#iot服务)

<!-- vim-markdown-toc -->


-----------------------------------------------------------

代码调试
========

代码下载
--------
地址:`http://athena.devops.letv.com/#/admin/projects/smarthome/LeIOT`

TV项目
分支: `x450_debug_iot_20180323`
位置: `vendor/letv/frameworks/LeIOT`


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

编译
----
*Java版本:java-1.7.0-openjdk-amd64*
```bash
source build/envsetup.sh
lunch (cibn, aosp_mangosteen_xxx-userdebug)
cd vendor/letv/frameworks/LeIOT
mm
```

调试
----

```bash
export TOP=/data/source/mainline/mstar938
adb connect 10.58.83.7
adb root
adb connect 10.58.83.7
adb remount
adb push $TOP/out/target/product/mangosteen/system/framework/leiot.jar /system/framework/
adb push $TOP/out/target/product/mangosteen/system/lib/libiotclient_jni.so /system/lib
adb push $TOP/out/target/product/mangosteen/system/lib64/libiotclient_jni.so /system/lib64
adb push $TOP/out/target/product/mangosteen/system/lib/libiot_binder.so /system/lib
adb push $TOP/out/target/product/mangosteen/system/lib64/libiot_binder.so /system/lib64
adb push $TOP/out/target/product/mangosteen/symbols/system/bin/iot_server /system/bin/

adb shell logcat -c; adb shell logcat | grep -i "iot"
```
dex转化为jar, 可供App使用jar包
```
0jdecode.sh $TOP/out/target/common/obj/JAVA_LIBRARIES/leiot_intermediates/classes.dex
如果$TOP/out/target/common/obj/JAVA_LIBRARIES/leiot_intermediates/classes.jar存在, 可以不用执行
```

IOT服务
-------

1. 由init.rc启动, 服务死掉后会自动重启`restart`, 目前没有给该服务root权限

```
    service iot_server /system/bin/iot_server
        class main
        restart
```

2. 可以使用ctl.stop停止该服务, 这种情况下服务不会自动重启, 启动需要执行ctl.start

```bash
    $ setprop ctl.stop  iot_server
    $ setprop ctl.start iot_server
```

3. Sepolicy并没有设置, userdebug模式下并没有影响, 后续会加上.
