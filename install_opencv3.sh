sudo apt-get install build-essential cmake git libgtk2.0-dev pkg-config libavcodec-dev libavformat-dev libswscale-dev python-dev python-numpy libtbb2 libtbb-dev libjpeg-dev libpng-dev libtiff-dev libjasper-dev libdc1394-22-dev
mkdir ~/opencv3
cd ~/opencv3
wget -qO- https://github.com/Itseez/opencv/archive/3.1.0.tar.gz  | tar xvz
cd ~/opencv3/opencv-3.1.0/
mkdir ~/opencv3/opencv-3.1.0/release
cd ~/opencv3/opencv-3.1.0/release
cmake -D CMAKE_BUILD_TYPE=RELEASE -D CMAKE_INSTALL_PREFIX=/usr/local ..
make -j8 
sudo make install

