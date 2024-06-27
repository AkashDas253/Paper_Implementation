# Implementation of "An Efficient Detail-Preserving Approach for Removing Impulse Noise in Images"

## Overview

This repository contains the implementation of the algorithm described in the paper "An Efficient Detail-Preserving Approach for Removing Impulse Noise in Images" by Wenbin Luo, published in IEEE Signal Processing Letters, Vol. 13, No. 7, July 2006. The algorithm focuses on removing impulse noise from images while preserving image details.

## Paper Abstract

In this paper, a new efficient algorithm for the removal of impulse noise from corrupted images while preserving image details is presented. The algorithm is based on the alpha-trimmed mean, which is a special case of the order-statistics filter. Unlike some previous techniques involving order-statistics filters, the proposed method uses the alpha-trimmed mean only in impulse noise detection instead of pixel value estimation. Once a noisy pixel is identified, its value is replaced by a linear combination of its original value and the median of its local window. Extensive computer simulations indicate that this algorithm provides a significant improvement over many other existing techniques.

## Implementation Details

The algorithm has been implemented in C and works on PGM (Portable Gray Map) files. The implementation includes both the impulse noise detection and cancellation steps, ensuring efficient and detail-preserving noise removal.

## Tools Used

- C programming language
- Standard C libraries for file handling and image processing

## Usage

### Prerequisites

- GCC (GNU Compiler Collection) or any C compiler
- Make utility (optional, for convenience)

