Install [Teensy Loader] (https://www.pjrc.com/teensy/loader_linux.html) with udev rules.

Then

```bash
sudo apt-get install gcc-avr binutils-avr avr-libc
git clone --recursive https://github.com/elshaio/switch-teensy.git
```


 

 For writing file to teensy:

 ```
 make
# Take .hex from teensy app and write onto teensy
 ```





