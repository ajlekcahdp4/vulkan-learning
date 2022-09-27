# vulkan-hpp learning
Repository with my attempts to learn vulkan C++ API

## How to clone
```
git clone git@github.com:ajlekcahdp4/vulkan-learning.git
git submodule init
git submodule update
```
## How to build
```
cmake -S . -B build
make -C build -j12 install
```

After that bin files will be installed in steps/XXX/bin/ and you wiil be able to run it using

```
./steps/XXX/bin/XXX
```