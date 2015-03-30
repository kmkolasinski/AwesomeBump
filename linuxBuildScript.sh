
# Add your QT path here
MY_QT_PATH=your_similar_qt_path/Qt/5.4/gcc_64/bin/

export PATH=$MY_QT_PATH:$PATH
cd Sources
qmake
make
cp AwesomeBump ../Bin/
cd ../Bin
./AwesomeBump
