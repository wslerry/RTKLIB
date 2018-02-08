#!/bin/bash
RELEASE_DIR="rtklib-appimage-x86_64-2.4.3"

mkdir $RELEASE_DIR
rm -rf $RELEASE_DIR/*

qmake RTKLib.pro -spec linux-g++
make -f Makefile -j4


single_image() {
    app=$1
    app_qt="${app}_qt"
    appimage_dir=$RELEASE_DIR/"${app^^}_Qt-x86_64"
    icon="${app}_Icon"
    mkdir $appimage_dir
    cp -t $appimage_dir app/$app_qt/$app_qt app/$app_qt/"${icon}.ico"
    convert $appimage_dir/"${icon}.ico" $appimage_dir/"${icon}.png"
    if [ $app == 'rtknavi' ]
    then
        mv $appimage_dir/rtknavi_Icon-0.png $appimage_dir/rtknavi_Icon.png
        rm $appimage_dir/rtknavi_Icon-1.png
    fi
    printf  \
"[Desktop Entry]
Name=${app^^}_Qt
Comment=${app^^} Qt version by Emlid
Exec=${app_qt}
Icon=${icon}
Type=Application
Terminal=false" > $appimage_dir/"${app_qt}.desktop"
    cd $appimage_dir/
    ../../linuxdeployqt-continuous-x86_64.AppImage "${app_qt}.desktop" -appimage
    cd ../..
}

for app in rtkpost rtkconv rtkplot rtknavi; do single_image "$app" done &
done