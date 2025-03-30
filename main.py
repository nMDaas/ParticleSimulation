import pygame
import sys
from Particle import Particle
from Vector2f import Vector2f
from Solver import Solver
from Renderer import Renderer

# Initialize Pygame
pygame.init()

# Set up the screen
screen = pygame.display.set_mode((800, 600))
pygame.display.set_caption("Particle Simulation")

solver = Solver()
solver.addObject(Vector2f(400, 300), 10)

renderer = Renderer(solver, screen)

# Main game loop
running = True
while running:
    # Handle events
    for event in pygame.event.get():
        if event.type == pygame.QUIT:
            running = False

    solver.update()

    # Fill the screen with a background color
    screen.fill((0,0,0))  # Light blue background

    renderer.render(screen)

    # Update the screen
    pygame.display.flip()

    # Delay to control the frame rate (about 60 FPS)
    pygame.time.Clock().tick(60)

# Quit Pygame
pygame.quit()
sys.exit()
