import pygame
import sys
from Vector2f import Vector2f
from Solver import Solver
from Renderer import Renderer
from Boundary import Boundary
import random
import math

# TODO
# Bugs when you increase number of particles! Why is that happening?
# Mouse pull/push not working anymore

# Initialize Pygame
pygame.init()

window_width = 800
window_height = 600

# Set up the screen and window
screen = pygame.display.set_mode((window_width, window_height))
pygame.display.set_caption("Particle Simulation")

boundary = Boundary(Vector2f(400,300), 290)

solver = Solver(boundary)

clock = pygame.time.Clock()

# Particle related attributes 
spawn_delay = 0.05
max_objects = 30
last_spawn_time = 0 
min_radius = 20
max_radius = 30
max_angle = 80
spawn_velocity = 700

renderer = Renderer(solver, screen, boundary)

# Main game loop
running = True
while running:
    # Handle events
    for event in pygame.event.get():
        if event.type == pygame.QUIT:
            running = False

        if event.type == pygame.MOUSEBUTTONDOWN:  # Detect mouse press
            ratio = 840.0/window_width
            x, y = pygame.mouse.get_pos()
            #x = float(x)
            #y = float(y)
            pos = Vector2f(x, y)
            if event.button == 1:  # Left Click
                solver.mousePull(pos)
            elif event.button == 3:  # Right Click
                solver.mousePush(pos)

    clock.tick(70) 
    current_time = pygame.time.get_ticks() / 1000

    # Spawn particles
    max_angle = 80  # degrees
    max_angle_rad = math.radians(max_angle)  # convert to radians once

    if (len(solver.getObjects()) < max_objects and (current_time - last_spawn_time) >= spawn_delay):
        last_spawn_time = current_time  # Restart the spawn timer
        random_radius_size = random.randint(min_radius, max_radius)
        newParticle = solver.addObject(Vector2f(400, 300), random_radius_size)

        random_angle = max_angle_rad * math.sin(3 * current_time)
        direction = Vector2f(math.cos(random_angle), math.sin(random_angle))
        initial_velocity = direction * spawn_velocity
        solver.setParticleVelocity(newParticle, initial_velocity)

    solver.update(current_time)

    # Fill the screen with a background color
    screen.fill((255,255,255))  # Light blue background

    renderer.render()

    # Update the screen
    pygame.display.flip()

    # Delay to control the frame rate (about 60 FPS)
    pygame.time.Clock().tick(60)

# Quit Pygame
pygame.quit()
sys.exit()
