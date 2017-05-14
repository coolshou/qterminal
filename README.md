# qtvt
qt base virtual terminal emulator for serial communications

# environment
    Ubuntu 16.04.2
    user account should be in dialout group to access /dev/ttyS*, /dev/ttyUSB*
    
# requirement
    debhelper (>=9)
    qt5-qmake (>=5.5.1)
    qtbase5-dev (>=5.5.1)
    libqt5serialport5-dev (>=5.5.1)
    qtchooser 
    gdebi (for deb update use)
    devscripts (for debuild)
    openssl (windows)

# build
```    
  # make sure using qt5 as default setting
  > qtchooser -print-env
  #if the result does not include qt5, change following setting
  #on i386
  > sudo ln -sf /usr/share/qtchooser/qt5-i386-linux-gnu.conf /usr/lib/i386-linux-gnu/qt-default/qtchooser/default.conf
  #on x86_64 
  > sudo ln -sf /usr/share/qtchooser/qt5-x86_64-linux-gnu.conf /usr/lib/x86_64-linux-gnu/qt-default/qtchooser/default.conf
  > sudo ln -sf /usr/share/qtchooser/qt5-x86_64-linux-gnu.conf /usr/lib/x86_64-linux-gnu/qtchooser/default.conf

```    
```
  > git clone --recursive https://github.com/coolshou/qtvt
  > cd qtvt
  > qmake
  > make
```
# build deb
```
  > debuild -b -uc -us
```  
