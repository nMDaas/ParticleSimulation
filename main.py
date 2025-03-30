import pygame
import sys
from Particle import Particle
from Vector2f import Vector2f

# Initialize Pygame
pygame.init()

# Set up the screen
screen = pygame.display.set_mode((800, 600))
pygame.display.set_caption("Particle Simulation")

# Define the circle's initial position and properties
color = (52, 63, 120)
circle = Particle(Vector2f(400, 300), color, 10, 0)

# Main game loop
running = True
while running:
    # Handle events
    for event in pygame.event.get():
        if event.type == pygame.QUIT:
            running = False

    # Move the circle
    circle.move()

    # Fill the screen with a background color
    screen.fill((0,0,0))  # Light blue background

    # Draw the circle
    pygame.draw.circle(screen, color, (circle.position.x, circle.position.y), circle.radius)

    # Update the screen
    pygame.display.flip()

    # Delay to control the frame rate (about 60 FPS)
    pygame.time.Clock().tick(60)

# Quit Pygame
pygame.quit()
sys.exit()
