# i-guess-that-counts
Unique words counter

```
sudo apt install pipx
pipx ensurepath
(restart shell)
pipx install conan
conan create conan/libaio --version=0.3.113 # broken mirror
conan install . --output-folder=build --build=missing -pr:h=conan/profiles/gcc15 -pr:b=conan/profiles/gcc15
cmake --preset conan-release
cmake --build build
time ./test.sh
```
