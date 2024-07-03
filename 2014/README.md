# Implementation of "Removal of High-Density Salt-and-Pepper Noise in Images With an Iterative Adaptive Fuzzy Filter Using Alpha-Trimmed Mean"

## Overview

This repository contains the implementation of the algorithm described in the paper "Removal of High-Density Salt-and-Pepper Noise in Images With an Iterative Adaptive Fuzzy Filter Using Alpha-Trimmed Mean" by Faruk Ahmed and Swagatam Das, published in IEEE Transactions on Fuzzy Systems, Vol. 22, No. 5, October 2014. The algorithm focuses on effectively removing high-density salt-and-pepper noise from images while preserving image details.

## Paper Abstract

Suppression of impulse noise in images is a critical task in image processing. This paper proposes an adaptive iterative fuzzy filter for denoising images corrupted by impulse noise. The algorithm operates in two stages—detecting noisy pixels using an adaptive fuzzy detector and denoising using a weighted mean filter on the identified "good" pixels in the filter window. Experimental results demonstrate that the algorithm outperforms state-of-the-art filters, even at noise levels as high as 97%.

## Implementation Details

The algorithm has been implemented in C and works specifically on PGM (Portable Gray Map) image files. The implementation includes both the detection of noisy pixels with an adaptive fuzzy detector and the subsequent denoising step using the mean of k-middle approach. The parameters such as window size and adaptive thresholds are configurable within the code to optimize performance.

## Tools Used

- C programming language
- Standard C libraries for file handling and image processing



