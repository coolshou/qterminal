# qtvt
qt base virtual terminal emulator for serial communications

# requirement
    debhelper (>=9)
    qt5-qmake (>=5.5.1)
    qtbase5-dev (>=5.5.1)
    libqt5serialport5-dev (>=5.5.1)
    qtchooser (/usr/lib/x86_64-linux-gnu/qtchooser/default.conf)
    gdebi (for update use, not finish)
  
# build
    check qt5 as defaule
```    
  > cat /usr/lib/x86_64-linux-gnu/qtchooser/default.conf
    /usr/lib/x86_64-linux-gnu/qt5/bin
    /usr/lib/x86_64-linux-gnu
```    
```
  > git clone https://github.com/coolshou/qtvt
  > cd qtvt
  > qmake
  > make
```
# build deb
```
  > dpkg-buildpackage -b
```  
