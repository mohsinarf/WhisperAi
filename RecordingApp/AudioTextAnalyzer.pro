QT += quick core sql network svg serialport multimedia

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
        application.cpp \
        main.cpp

RESOURCES += qml.qrc

# Additional import path used to resolve QML modules in Qt Creator's code model
QML_IMPORT_PATH =

# Additional import path used to resolve QML modules just for Qt Quick Designer
QML_DESIGNER_IMPORT_PATH =

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target


DISTFILES += \
    Android/AndroidManifest.xml \
    Android/build.gradle \
    Android/gradle/wrapper/gradle-wrapper.jar \
    Android/gradle/wrapper/gradle-wrapper.properties \
    Android/gradlew \
    Android/gradlew.bat \
    Android/res/drawable-hdpi/icon.png \
    Android/res/drawable-ldpi/icon.png \
    Android/res/drawable-mdpi/icon.png \
    Android/res/drawable-xhdpi/icon.png \
    Android/res/drawable-xxhdpi/icon.png \
    Android/res/drawable-xxxhdpi/icon.png \
    Android/res/drawable/ic_launcher_background.xml \
    Android/res/drawable/test_sign.png \
    Android/res/values/libs.xml \
    Android/src/kr/co/smartcast/kds/ExternalTunnelActivity.java \
     \
    android/aidl/service/vcat/smartro/com/vcat/SmartroVCatCallback.aidl \
    android/aidl/service/vcat/smartro/com/vcat/SmartroVCatInterface.aidl \
    android/res/values/colors.xml \
    android/res/values/themes.xml \
  straightLine.png

android{
contains(ANDROID_TARGET_ARCH, arm64-v8a) {
    message("Building for ARM64-v8a architecture on Android")
        ANDROID_PACKAGE_SOURCE_DIR = \
            $$PWD/android
    LIBS += -L$$PWD/Prebuilt/arm64-shared/lib/ -lcrypto -lssl

    ANDROID_EXTRA_LIBS += \
        $$PWD/Prebuilt/arm64-shared/lib/libssl.so \
        $$PWD/Prebuilt/arm64-shared/lib/libcrypto.so

    INCLUDEPATH += $$PWD/Prebuilt/arm64-shared/include

    QT += androidextras

    # Add ARM64-v8a specific configuration here
}

contains(ANDROID_TARGET_ARCH, armeabi-v7a) {
    message("Building for ARMv7a architecture on Android")
    ANDROID_PACKAGE_SOURCE_DIR = \
        $$PWD/android
    # Add ARMv7a specific configuration here
    LIBS += -L$$PWD/Prebuilt/armv7-shared/lib/ -lcrypto -lssl

    ANDROID_EXTRA_LIBS += \
        $$PWD/Prebuilt/armv7-shared/lib/libssl.so \
        $$PWD/Prebuilt/armv7-shared/lib/libcrypto.so

    INCLUDEPATH += $$PWD/Prebuilt/armv7-shared/include

    QT += androidextras
}
} else {
    win32-g++{
        INCLUDEPATH += $$PWD/Prebuilt/win32/include
        LIBS += -L$$PWD/Prebuilt/win32/bin -llibcrypto-3 -llibssl-3
        }

    !win32-g++ {
        message("Target architecture is Raspberry Pi")
        INCLUDEPATH += $$PWD/Prebuilt/win32/include
#                        $$PWD/Prebuilt/win64/sub/include
#        LIBS += -L$$PWD/Prebuilt/win64/bin -llibcrypto3 -llibssl-3
        }
}
android: include($$PWD/android_openssl-master/openssl.pri)

HEADERS += \
  application.h
