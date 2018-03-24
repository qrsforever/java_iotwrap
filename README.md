####
0jdecode.sh /data/source/mainline/mstar648//out/target/common/obj/JAVA_LIBRARIES/leiot_intermediates/classes.dex
cp /data/source/mainline/mstar648/vendor/letv/apps/LeIOT/output/dex2jar/classes.jar /workspace/android/test/simple/libs/leiot.jar 
adb push /data/source/mainline/mstar648//out/target/product/almond/system/framework/leiot.jar /system/framework/
adb push /data/source/mainline/mstar648//out/target/product/almond/system/lib/libiotclient_jni.so /system/lib
adb push /data/source/mainline/mstar648//out/target/product/almond/system/lib64/libiotclient_jni.so /system/lib64
adb push /data/source/mainline/mstar648//out/target/product/almond/system/bin/iot_server /system/bin 
