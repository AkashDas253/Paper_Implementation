# Image Processing Paper Implementations

This repository contains implementations of various image processing papers. Each implementation includes a PDF of the paper and a C program that implements the paper's methods.

## Usage

### use.c

The `use.c` program takes the paths to the corrupted and enhanced images as arguments. Here's how to run it:

```bash
./use path_to_corrupted_image path_to_enhanced_image
'''

### Test
The 'test.c' program takes the paths to the real, corrupted, and enhanced images as arguments. It evaluates the images using Root Mean Square Error (RMSE), Peak Signal-to-Noise Ratio (PSNR), Image Enhancement Factor (IEF), and Structural Similarity Index Measure (SSIM). Here's how to run it:

'''
./test path_to_real_image path_to_corrupted_image path_to_enhanced_image
'''

## Implementations

Each implementation is in its own directory and includes a PDF of the paper and two C program that implements the paper's methods.

### Paper 1

- [Paper PDF](link_to_pdf)
- [Implementation](link_to_implementation)

### Paper 2

- [Paper PDF](link_to_pdf)
- [Implementation](link_to_implementation)

## Dependencies

- GCC (GNU Compiler Collection) or any C compiler
- PGM image files for testing