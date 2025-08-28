# Fluid Particle Simulation

## Introduction

I started this project because I was curious about how fluid particle simulations work in software like Houdini. Like any DCC application, this program supports both a simulation preview (visualizing raw particles) and a renderer (using ray marching with volumetric lighting).

Here are some things that were done here: 
- This included exploring Verlet integration, the Beer–Lambert law, the Weber number, and aspects of Smoothed Particle Hydrodynamics (SPH).
- Using my experience with OpenGL, Phong shading in GLSL, and point lighting
- techniques including ray marching, volumetric lighting
- performance optimizations such as spatial hashing and multithreading. 
____________________________

## 2D Fluid Simulation With PyGame
- Implemented basic fluid dynamics in 2D first to work out simulation physics and math correctly
- Implemented Verlet integration and the Weber number
<p align="center">
<img src="https://media.giphy.com/media/Nh850QCpykKsL2O4r6/giphy.gif" object-fit="contain" width="472px" height="270px">
<img src="https://media.giphy.com/media/0eIo2VmXNrguzadziV/giphy.gif" object-fit="contain" width="472px" height="270px">
</p>

## 3D Simulation Preview
- Used C++, OpenGL, and Phong GLSL shading for the simulation preview
- Implemented Verlet integration, the Weber number and aspects of Smoothed Particle Hydrodynamics (SPH)
<p align="center">
<img src="https://media3.giphy.com/media/v1.Y2lkPTc5MGI3NjExcnZ5ZXptZnhkdnFrc2JreGxzNzMzNGlld3JpbHg1ZDVmdXRlcGR0aSZlcD12MV9pbnRlcm5hbF9naWZfYnlfaWQmY3Q9Zw/9B3VWv9m1Hf5LlnlmG/giphy.gif" object-fit="contain" width="472px" height="270px">
<img src="https://media0.giphy.com/media/v1.Y2lkPTc5MGI3NjExaTNzOWszYzBnMTNpOGg3MHNmMTZ3aDhndjhjZDd6NDk3MXB6dGJnNiZlcD12MV9pbnRlcm5hbF9naWZfYnlfaWQmY3Q9Zw/rtB1VRN5QF2rhylDPW/giphy.gif" object-fit="contain" width="472px" height="270px">
<p>

### Container/Obstacle Collision 
- Customizable container box sizing 
<p align="center">
<img src="https://media.giphy.com/media/XKahXL2urTKMADJJUr/giphy.gif" object-fit="contain" width="472px" height="270px">
<img src="https://media.giphy.com/media/69uYbY5LaAEZjqTuN9/giphy.gif" object-fit="contain" width="472px" height="270px">
<img src="https://media.giphy.com/media/ERDGvqDjTXkYn6UfoJ/giphy.gif" object-fit="contain" width="472px" height="270px">
<p>


## Optimization & Performance Improvement
- Spatial Hashing and multithreading, improving FPS by x13
<img src="https://media3.giphy.com/media/v1.Y2lkPTc5MGI3NjExcmt6bzA0d2lhejRydDYzdGFxZTR2bDJtN2UwbzkzMWs4N20wc2VtOCZlcD12MV9pbnRlcm5hbF9naWZfYnlfaWQmY3Q9Zw/0R272usy1N6x2ov1JW/giphy.gif" object-fit="contain" width="472px" height="270px">

## Ray Marching

## Volumetric lighting

## How to Run
