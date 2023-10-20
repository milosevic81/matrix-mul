
## Results ##

```
## Test 1 ##
Matrix size 256 mul Duration: 0.0127086
Matrix size 256 rmul Duration: 0.00649467
Matrix size 256 smul Duration: 0.00622396

Matrix size 512 mul Duration: 0.0994609
Matrix size 512 rmul Duration: 0.0518193
Matrix size 512 smul Duration: 0.0429592

Matrix size 1024 mul Duration: 1.1037
Matrix size 1024 rmul Duration: 0.426107
Matrix size 1024 smul Duration: 0.298547

Matrix size 2048 mul Duration: 12.0699
Matrix size 2048 rmul Duration: 3.44349
Matrix size 2048 smul Duration: 2.17547

Matrix size 4096 mul Duration: 299.81
Matrix size 4096 rmul Duration: 27.7858
Matrix size 4096 smul Duration: 16.2194

Matrix size 8192 rmul Duration: 224.617
Matrix size 8192 smul Duration: 119.845

```

https://web.physics.utah.edu/~detar/lessons/c++/matrices/node4.html

Elgen
http://eigen.tuxfamily.org/index.php?title=Main_Page
http://eigen.tuxfamily.org/index.php?title=Benchmark



For compilers to find libomp you may need to set:
  export LDFLAGS="-L/opt/homebrew/opt/libomp/lib"
  export CPPFLAGS="-I/opt/homebrew/opt/libomp/include"