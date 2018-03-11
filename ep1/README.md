# history command
    apt-get update
    apt-get -y install git fakeroot build-essential ncurses-dev xz-utils libssl-dev bc
    tar xvf linux-4.15.7.tar.xz
    cd linux-4.15.7
    cp /boot/config-3.19.0-75-generic ./.config
    make menuconfig
    make -j4 
    apt-get install libelf-dev
    make -j4 
    make modules_install -j4 
    make install -j4 
    update-initramfs -c -k 4.15.7 
    update-grub

