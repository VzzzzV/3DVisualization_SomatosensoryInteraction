# Readme

__Author : Zhan Zhan__
__Date: 2019 May__

## 3D visualization with somatosensory interaction based on Leap Motion

This project completes point cloud, line, triangle and texture mapping visualization based on VTK.

Users can interact by hands when a Leap Motion device is connected.

## Dependence

This project is based on Qt5, VTK 8.2.0 and Leap Motion SDK 4.0.0.

## Data

The data used in the project in ./data is obtained by experiment, not a standard 3D file format.


## Somatosensory Interaction
+ Controlling Mouse: The cursor will directly move following your hands and flip your fingers will be considered as click
+ Zooming: The palm of two hands point to each other, when the distance between hands increase: zoom in, otherwise zoom out 
+ Moving: The palm of two hands point to each other, simultaneously moving the hands with the same direction
+ Rotation: When you are in a rotation mode, move one hand and the object will be rotated based on trackball camera controlling model
## Functions

+ Point cloud, line, triangle and texture mapping visualization
+ Somatosensory interaction using Leap motion

## Results

There are some processed images in the ./result fold.