# The Need for Speed

In this second phase, we convert our image manipulation algorithms to C.

You will learn
* that programs in different programming languages can look almost the same
* how to allocate dynamic arrays on the heap and on the stack
* how multi-dimensional arrays are represented in the memory
* that C is a lot faster than Python

[[_TOC_]]


## Important Dates

| Date | Description |
|:---  |:--- |
| Monday, April 10 | Handout |
| Monday, May 1 | Submission deadline |

### Hand-out

Fork this repository into your namespace and make sure its visibility is set to private. Read the instructions here carefully, then clone your copy of the lab to your computer and get to work.

**Important:** Make sure to push your work to your private repository in your namespace and not the official handout. *Pushing code or merge requests to the handout will result in a 50-100% point reduction*. If you do not know how to use Git, look it up. Most Git commands support the `--dry-run` option that shows you what actions a command would perform without actually executing them.

### Submission

Commit and push your work frequently to avoid data loss. When you are ready to submit your code for grading, create a tag called "Submission". The timestamp of the "Submission" tag is considered your submission time.

To create a tag, visit the repository on GitLab and navigate to Repository -> Tags. Enter "Submission" as the Tag name then hit "Create tag". You can leave the other fields empty.

If you later discover an error and want to update your submission, you can delete the "Submission" tag, fix your solution, and create a new "Submission" tag. If that happens _after_ the submission deadline, you also need to email the TAs so that they are aware of the update to your submission.

### Coding Rules

In this lab, your work goes into the files `blur_float.c`, `blend_float.c`, `blur_int.c`, and `blend_int.c`. Also in this phase, you are not allowed to use any additional libraries and must not change the number of types of parameters passed to, or returned from the functions.

You can modify the other files of the handout to your liking, however, our grading system will evaluate only the four files mentioned above in a standard environment.


## Image Manipulation in C

Python has many advantages when used as a rapid prototyping language but is less suited for large projects or high-performance computing. While Python is the de-facto standard for machine learning and neural network frameworks, the libraries such as TensorFlow, PyTorch, Numpy, Pandas, or OpenCV are all written in C or C++. 

C (and C++), on the other hand, is the de-facto standard to implement system software. As an imperative, close-to-the hardware, and strongly-typed language the programmer has maximum control over the way the hardware is accessed. However, with great power comes great responsibility. With C, it is easy to shoot yourself in the foot.

### Data Representation

Since functions can only return one value in C, we cannot use the "Python way" and return the image data, height, width, and number of channels with a single return statement. 
```Python
    return blurred, bheight, bwidth, bchannels
```

Instead, the provided C image library `imlib.c/h` defines a struct that contains the relevant fields that define an image:
```C
typedef unsigned char uint8;

struct Image {
    uint8 *data;
    int height;
    int width;
    int channels;
};
```

The struct contains fields to store the height, width, and the number of channels of an image.
The image data, conceptually a 3-dimensional array of bytes with dimensions [height][width][channels] is stored as a flat (1-dimensional) array of `uint8` (=`unsigned char`) values. 

The flattening occurs along the axes height, width, and channels, in this order. The following illustration demonstrates the concepts:

![](resources/Array.Flatten.png)

The flattened array stores, from inner to outer dimensions, the data of the pixels (channel direction), the columns (width direction), and finally the rows (height direction). This format is called "row-major" layout and is the standard for almost all programming languages with the notable exception of Fortran.

### Indexed Array Accesses

Similar to the one-dimensional case, the location of an element in an n-dimensional array can be easily calculated. In our particular case, the offset of `image[h][w][c]` in the flattened array is given by
```math
\begin{align}
offset_{flat} &= c + w*CHANNELS + h * WIDTH*CHANNELS \\
              &= c + CHANNELS*(w + WIDTH*h)
\end{align}
```

The handout contains two macros (defined in `imlib.h`) that will help you with calculating the offset of an element in the flattened array and with reading/writing an element:
1. `INDEX(img, y, x, c)`  
   This macro computes the offset from the start of `img.data` to the element designated by coordinates [y,x,c].
1. `PIXEL(img, y, x, y)`  
   This macro accesses the element designated by coordinates [y,x,c] in the `img.data` array. Internally, it uses the `INDEX()` macro to compute the offset.
   The `PIXEL()` macro can be used for reading or writing the array, i.e.,  
   ```C
   PIXEL(img, y, x, c) = 73;
   ```
   and
   ```C
   uint8 v = PIXEL(img, y, x, c);
   ```
   are both valid.

### Explicit Dynamic Memory Allocation

Unlike Python, C programmers are responsible for memory management. Allocating memory is easy, (correctly) freeing memory a bit more complicated. 

When setting up the `struct Image` in the `blur()` and `blend()` functions, we need to generate and return a new image. The allocation of the struct and the assignment of the scalar fields is easy:

```C
  struct Image output;

  output.height   = ...;
  output.width    = ...;
  output.channels = ...;
```

The `data` field, however, has to be allocated dynamically. We can achieve this with the `malloc()` or the `calloc()` function that are both part of the C standard library. Read the man pages for both functions to learn what header file to include and how to call the functions.

```C
  output.data = (uint8*)malloc(...)
  if (output.data == NULL) abort();
```

Since dynamic memory allocations can fail (for example, if there is not enough free memory available), you *always* have to check the result of these functions. In our case, we are not really interested in gracefully handling the error, but rather forcefully terminate the program - which is still a lot better than a segmentation fault.

After you have allocated the data of an image as shown above, you can use the `PIXEL(output, y, x, c)` macro to read/write its elements.

Once we do not need a dynamically allocated memory block anymore, we should free it. This can be achieved with `free(output.data)`. For short-running programs such as our blend/blur drivers, freeing memory is not absolutely necessary.

### Implicit Dynamic Memory Allocation

If the compiler knows the dimensions of an array `A`, we can conveniently access the array with the square bracket notation `A[y][x]`. Since C99 (the [C standard first published in 1999](https://web.archive.org/web/20170428013610if_/http://www.open-std.org/jtc1/sc22/wg14/www/docs/n1256.pdf)), variable length arrays (VLAs) can be automatically allocated on the stack of a function. Another benfit of stack-allocated VLAs are that the compile will automatically deallocate them at the end of the function, so no explicit memory management is necessary.

The follwing code demonstrates the use of VLAs:
```C
void foo(int k)
{
  float kernel[k][k];

  for (int y=0; y<k; y++)
    for (int x=0; x<k; x++)
      kernel[y][x] = ...;

  ...
}
```


## Your Task

Your task is to implement the image blurring and image blending algorithms that you developed in phase 1 in the C language.

Again, we implement two variants of each operation
* a (standard) floating point variant
* a (faster) fixed-point integer variant

The handout contains the skeletons for both manipulations and variants (`blur_float.c`, `blur_int.c`, `blend_float.c`, and `blend_int.c`). Your code will only go into these functions. Similar to part 1, we provide driver programs that call your functions and an image library to read and write CSAP RAW images. 

A note on data types: CPython, the standard Python runtime, uses the `double` data type for floating-point computations. You are free to use `float` or `double` in your floating-point C code, but in the former case, the output of your C implementation may diverge slightly from the images generated by the Python implementation. 

Once you have finished your implementation, compare the performance of the Python implementation from phase 1 to your C code in phase 2. First, edit the `Makefile` in the root directory of this phase and switch from debugging code to optimized code by commenting out the fifth line and uncommenting line 3:
```Makefile
# C compile flags
# - performance
CFLAGS=-O2
# - debugging
#CFLAGS=-g

...
```

Now, rebuild your project:
```bash
$ make clean
$ make
```

Depending on the system load, the execution time of your algorithms may vary considerably. Execute each test case 5-10 times, drop the fastest and the slowest measurement, then take the arithmetic average for the C and the Python implementation. You can add your analysis to this README file right here.




<div align="center" style="font-size: 1.75em;">

**Happy coding!**
</div>


## Time Analysis ( C vs. Python )

| Type | Operation | Input | Avg.Time : C | Avg.Time : Python |
|:---  |:---  |:---  |:---  |:---  |
| int | Blur 3x3 | cat_256.raw | 3.99 ms | 493.70 ms |
| float | Blur 3x3 | cat_256.raw | 4.58 ms | 465.67 ms |
| int | Blur 5x5 | cat_256.raw | 7.53 ms | 1072.21 ms |
| float | Blur 5x5 | cat_256.raw | 8.79 ms | 995.14 ms |
| int | Blur 7x7 | cat_256.raw | 12.47 ms | 1871.23 ms |
| float | Blur 7x7 | cat_256.raw | 16.58 ms | 1725.58 ms |
| int | Blur 3x3 | cat_512.raw | 12.08 ms | 2185.06 ms |
| float | Blur 3x3 | cat_512.raw | 10.98 ms | 2083.25 ms |
| int | Blur 5x5 | cat_512.raw | 14.88 ms | 5033.73 ms |
| float | Blur 5x5 | cat_512.raw | 15.35 ms | 4239.85 ms |
| int | Blur 7x7 | cat_512.raw | 40.696 ms | 8172.32 ms |
| float | Blur 7x7 | cat_512.raw | 53.980 ms | 7599.07 ms |
| int | Blur 3x3 | cat_1024.raw | 39.59 ms | 8875.19 ms |
| float | Blur 3x3 | cat_1024.raw | 28.71 ms | 8211.27 ms |
| int | Blur 5x5 | cat_1024.raw | 41.79 ms | 19307.53 ms |
| float | Blur 5x5 | cat_1024.raw | 50.80 ms | 17868.04 ms |
| int | Blur 7x7 | cat_1024.raw | 73.49 ms | 34296.43 ms |
| float | Blur 7x7 | cat_1024.raw | 106.92 ms | 31919.25 ms |
| int | Merge 0.5 | 301_256.raw , SNU_256.raw | 0.17 ms | 116.99 ms |
| float | Merge 0.5 | 301_256.raw , SNU_256.raw | 0.29 ms | 113.72 ms |
| int | Merge 0.5 | 301_512.raw , SNU_512.raw | 1.97 ms | 483.35 ms |
| float | Merge 0.5 | 301_512.raw , SNU_512.raw | 3.28 ms | 481.69 ms |
| int | Merge 0.5 | 301_1024.raw , SNU_1024.raw | 3.61 ms | 1909.27 ms |
| float | Merge 0.5 | 301_1024.raw , SNU_1024.raw | 4.41 ms | 1927.65 ms |
| int | Overlay 0.5 | 301_256.raw , SNU_256.raw | 0.11 ms | 88.46 ms |
| float | Overlay 0.5 | 301_256.raw , SNU_256.raw | 0.26 ms | 108.28 ms |
| int | Overlay 0.5 | 301_512.raw , SNU_512.raw | 1.24 ms | 376.91 ms |
| float | Overlay 0.5 | 301_512.raw , SNU_512.raw | 2.84 ms | 464.44 ms |
| int | Overlay 0.5 | 301_1024.raw , SNU_1024.raw | 1.80 ms | 1487.18 ms |
| float | Overlay 0.5 | 301_1024.raw , SNU_1024.raw | 4.06 ms | 1789.45 ms |

<div align="center" style="font-size: 1.75em;">

**C is much faster than Python!**
</div>


