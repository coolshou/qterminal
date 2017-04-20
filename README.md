# qtvt
qt base virtual terminal emulator for serial communications

# requirement
    debhelper (>=9)
    qtbase5-dev (>=5.5.1)
    libqt5serialport5-dev (>=5.5.1)
  
# build
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
