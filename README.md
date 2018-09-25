# Description

This is essentially the libjpeg example re-massaged into two executables
to read in a jpeg and print out width, height, and RGB values (one number per line); and to read in width, height, and RGB values (one number per line) and save them to a jpeg. 


# Dependencies

  - [libjpeg](http://libjpeg.sourceforge.net/)
    - Install on Linux via: apt install libjpeg-dev

# Usage

```
./jpeg2rgb <path to a jpeg file> <maximum desired width>
```

If the maximum desired width is greater than the jpeg's actual width, then the jpeg's actual width
is used for the output. If the maximum desired width is lower than the
jpeg's actual width, then the image is scaled down via pixel RGB values
averaging (i.e., information is lost). Output is printed to stdout, one (positive) integer per line. 
The first two lines contain the width and the height of the image. If the output image has n pixels,
then 3n lines are printed, each with R, G, and B values, for pixels in row-major order.

```
./rgb2jpeg <path to a jpeg file>
```

The input is read from stdin, and the expected format is the same as that of the output of jpeg2rgb.

---

