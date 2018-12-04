# rplidar-simple-detection
Simple obstacle detection algorithm for rplidar.

This project gives you a detection option for the rplidar. It checks the bounded area and produces a feedback according to the detection method that is given as a parameter. 

## Installation
run ```make``` in root directory. The build results in making an ```output``` directory.

## How To Use
After the installation, locate to ```output``` directory. It would have a directory for your existing platform, Linux, Darwin... Inside the directory contains the built files. Execute the following such as ```./Darwin/Release/detection```.

## Documetation

### `setDetectionMethod(method, options)`

Sets the detection method and options where:
- `method` - the method of detection:
  - `rectangle` - RECTANGULAR 0x01
  - `circular` - CIRCULAR 0x02
  - `circular-sector` - CIRCULAR_SECTOR 0x04

### `runDetecion(nodes)`

Runs the detection in the detection method as scripted with ```setDetectionMethod```. ```nodes``` are the values in which the rplidar sdk produces. The method returns the detection results.
``` c
struct DetectionResult {
  int state = 4;
  //right 1, left 2
  int data = 1;
}
```

### License

``` md
Copyright 2018 keenrangerAndHisFriends

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
```
  
