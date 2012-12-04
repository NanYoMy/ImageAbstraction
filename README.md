Image Abstraction
=================

Final project for CSC 205 2D Graphics and Image Processing. The algorithm used on 
images in this program is based on the process described in "Real-Time Video Abstraction".

This program creates a cartoon-like image abstraction from a given input image. This is achieved with 
several applications of a seperable approximation to a bilateral filter, lightness quantization, 
and overlayed difference of Gaussians edges.

The CIE L*a*b* color space is used to faciliate determining photometric similarity between colors. 
Photometric similarity is used in the bilateral filtering process and can be easily obtained by 
taking the Euclidean distance between two L*a*b* colors.

Two sliders allow the user to adjust the prominence of edges and the number of quantization bins. 
The user can choose to omit edges and quantization entirely by setting their respective sliders to their
minimum and maximum value.

All image processing is done on a seperate thread to ensure the user interface remains responsive at all times.

__Note__: This project use ARC (automatic reference counting) and can only be compiled with Xcode versions 4.2+.

References
==========
- "Real-Time Video Abstraction": www.cs.northwestern.edu/~holger/Research/papers/videoabstraction.pdf
- Colorspace conversion formulas: www.easyrgb.com