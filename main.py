import pygame
import sys
from Vector2f import Vector2f
from Solver import Solver
from Renderer import Renderer
from Boundary import Boundary

# Initialize Pygame
pygame.init()

window_width = 800
window_height = 600

# Set up the screen
screen = pygame.display.set_mode((window_width, window_height))
pygame.display.set_caption("Particle Simulation")

#boundary = Boundary(Vector2f(window_width/2, window_height/2), (window_width / 20.0) / 2)
boundary = Boundary(Vector2f(400,300), 290)

solver = Solver(boundary)
solver.addObject(Vector2f(400, 300), 10)

renderer = Renderer(solver, screen, boundary)

# Main game loop
running = True
while running:
    # Handle events
    for event in pygame.event.get():
        if event.type == pygame.QUIT:
            running = False

    solver.update()

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
