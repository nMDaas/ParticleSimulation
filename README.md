# Fluid Particle Simulation

I started this project because I was curious about how fluid particle simulations work in software like Houdini. Like any DCC application, this program supports both a simulation preview (visualizing raw particles) and a rendered preview (using ray marching with volumetric lighting).

- Developed a modular C++/OpenGL simulation engine with both simulation and ray-marched rendered previews, similar to workflows in industry-standard DCC software.
- Applied Verlet integration, particle-based interactions and neighborhood spatial searches (SPH-inspired), and velocity-dependent splashing effects modeled with the Weber number.
- Implemented a ray marching renderer with volumetric absorption with the Beer–Lambert law, visualize particle density and attenuation.
- Optimized performance with multithreading and spatial hashing, achieving a 13× FPS improvement.
- Exposed customizable simulation parameters (particle count, particle sizes/weights, container/obstacle dimensions, velocity damping, restitution, and ray marching blend factor) for flexible experimentation.
- Overcame challenges in handling collisions, managing multithreading safely, and debugging anomalous particle interactions in large-scale simulations.

## 2D Fluid Simulation With PyGame
- First implemented 2D fluid dynamics in Python to prototype simulation physics and particle physics calculations.
- Introduced Verlet integration and Weber number–based splash effects.
<p align="center">
<img src="https://media.giphy.com/media/Nh850QCpykKsL2O4r6/giphy.gif" object-fit="contain" width="381px" height="218px">
<img src="https://media.giphy.com/media/0eIo2VmXNrguzadziV/giphy.gif" object-fit="contain" width="381px" height="218px">
</p>

## 3D Simulation Preview
- 3D simulation preview implemented using C++, OpenGL, and Phong GLSL shading, incorporating Smoothed Particle Hydrodynamics (SPH) concepts.
<p align="center">
<img src="https://media4.giphy.com/media/v1.Y2lkPTc5MGI3NjExcDB3aDFhNTZoNXllbDhheWdmNWJlb3MwbHd2emdkMWhlb3l3YXduaSZlcD12MV9pbnRlcm5hbF9naWZfYnlfaWQmY3Q9Zw/eNWLAH9cCIeKDcad7V/giphy.gif" object-fit="contain" width="381px" height="218px">
<img src="https://media0.giphy.com/media/v1.Y2lkPTc5MGI3NjExdzd4d28zY2FiM3N0dG5id3I0azYwZHNrbDQ3MDBjbTFkazM1b3Q3aSZlcD12MV9pbnRlcm5hbF9naWZfYnlfaWQmY3Q9Zw/6crTpeim594EIOStH8/giphy.gif" object-fit="contain" width="381px" height="218px">
<p>

### Container/Obstacle Collision 
<p align="center">
<img src="https://media3.giphy.com/media/v1.Y2lkPTc5MGI3NjExNGYwMGRoczRlc3RjOWc2cGJqd2t0NnZzcDI5MzFubTRudHE4bzBxOCZlcD12MV9pbnRlcm5hbF9naWZfYnlfaWQmY3Q9Zw/qYZ5yDTiHMAYUsB8Gv/giphy.gif" object-fit="contain" width="381px" height="218px">
<img src="https://media1.giphy.com/media/v1.Y2lkPTc5MGI3NjExZXpsczFnODE2eWE1MzZsdHZrdXZ6dzhqdnV6Yjd3dGplM3Y3aDJ3cSZlcD12MV9pbnRlcm5hbF9naWZfYnlfaWQmY3Q9Zw/t5qhhHMwMXI6b1KG94/giphy.gif" object-fit="contain" width="381px" height="218px">
<img src="https://media3.giphy.com/media/v1.Y2lkPTc5MGI3NjExZHk5c2p1dGJhYXVkc3A2dTQ1aTVxMTY0MDFmaDF6MTMzazFpd2xnYiZlcD12MV9pbnRlcm5hbF9naWZfYnlfaWQmY3Q9Zw/EbkfK2hciICUfB1EJ1/giphy.gif" object-fit="contain" width="381px" height="218px">
<p>


## Optimization & Performance Improvement
- 13× FPS improvement using spatial hashing and multithreading
<div align="center">
No spatial hashing or multithreading
</div>
<div align="center">
<img src="https://media3.giphy.com/media/v1.Y2lkPTc5MGI3NjExcmt6bzA0d2lhejRydDYzdGFxZTR2bDJtN2UwbzkzMWs4N20wc2VtOCZlcD12MV9pbnRlcm5hbF9naWZfYnlfaWQmY3Q9Zw/0R272usy1N6x2ov1JW/giphy.gif" object-fit="contain" width="381px" height="218px">
</div>
<div align="center">
Only spatial hashing
</div>
<div align="center">
<img src="https://media0.giphy.com/media/v1.Y2lkPTc5MGI3NjExampsaHpzYnR4ZzNpajY4eXU0cTVjMGY2dDlwZmdwazN6ZHYxeWEyYiZlcD12MV9pbnRlcm5hbF9naWZfYnlfaWQmY3Q9Zw/UAdP6HtCkONeSzs2Sy/giphy.gif" object-fit="contain" width="381px" height="218px">
</div>
<div align="center">
Spatial hashing and multithreading
</div>
<div align="center">
<img src="https://media2.giphy.com/media/v1.Y2lkPTc5MGI3NjExenpteWpuazRuaXc3ZnlzeXAxd2N4eW5xM2VxdWlzdjlxb3Bkb3pheiZlcD12MV9pbnRlcm5hbF9naWZfYnlfaWQmY3Q9Zw/cbrmYq4YpDcMFSu8Pt/giphy.gif" object-fit="contain" width="381px" height="218px">
</div>

## 3D Ray-Marched Rendered Preview
Shading Breakdown: Volumetric Lighting (GPU, GLSL)
<p align="center">
<img src="https://github.com/nMDaas/ParticleSimulation/blob/main/renders/raymarchHeightGradient.png" object-fit="contain" width="330px" height="243px">
<img src="https://github.com/nMDaas/ParticleSimulation/blob/main/renders/raymarchVisNormals.png" object-fit="contain" width="330px" height="243px">
<img src="https://github.com/nMDaas/ParticleSimulation/blob/main/renders/raymarchDiffuse.png" object-fit="contain" width="330px" height="243px">
<img src="https://github.com/nMDaas/ParticleSimulation/blob/main/renders/raymarchAccumulateDensity.png" object-fit="contain" width="330px" height="243px">
<img src="https://github.com/nMDaas/ParticleSimulation/blob/main/renders/raymarchBeerLambert.png" object-fit="contain" width="330px" height="243px">
<img src="https://github.com/nMDaas/ParticleSimulation/blob/main/renders/raymarchVolumetricLighting.png" object-fit="contain" width="330px" height="243px">
</p>

### Simulation Preview vs. Ray-Marched Rendered Preview
<p align="center">
<img src="https://media.giphy.com/media/bXahEAlMe9sOun4h3Y/giphy.gif" object-fit="contain" width="427px" height="314px">
<img src="https://media.giphy.com/media/yg1CMImWsE7TvkwRBJ/giphy.gif" object-fit="contain" width="330px" height="243px">
<img src="https://media4.giphy.com/media/v1.Y2lkPTc5MGI3NjExcDB3aDFhNTZoNXllbDhheWdmNWJlb3MwbHd2emdkMWhlb3l3YXduaSZlcD12MV9pbnRlcm5hbF9naWZfYnlfaWQmY3Q9Zw/eNWLAH9cCIeKDcad7V/giphy.gif" object-fit="contain" width="330px" height="243px">
<img src="https://media.giphy.com/media/ejwu7rMUroDBy0rxQM/giphy.gif" object-fit="contain" width="330px" height="243px">
</p>

## How to Run

### 2D Fluid Simulation With PyGame
Run the main.py file in 2D/

### 3D Simulation
```
cd 3D_RayMarch_Absorption/
python3 build.py
./prog
```

For the simulation preview:
- Uncomment [main.cpp, line 348](https://github.com/nMDaas/ParticleSimulation/blob/main/3D_RayMarch_Absorption/src/main.cpp#L348) 
- Comment [main.cpp, line 353](https://github.com/nMDaas/ParticleSimulation/blob/main/3D_RayMarch_Absorption/src/main.cpp#L353)

For the rendered simulation:
- Comment [main.cpp, line 348](https://github.com/nMDaas/ParticleSimulation/blob/main/3D_RayMarch_Absorption/src/main.cpp#L348) 
- Uncomment [main.cpp, line 353](https://github.com/nMDaas/ParticleSimulation/blob/main/3D_RayMarch_Absorption/src/main.cpp#L353)

## External Resources Used
- Some OpenGL and starter code provided by Professor Amit Shesh and Professor Mike Shah
- Keyframe Codes' 'Making a particle simulation in C++' series
- 'Making of Moana (the ShaderToy)' blog post by Chris Wallis
- shadertoy.com 'Ray Marching - Basics'
