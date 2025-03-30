import pygame
import sys
from Circle import Circle

# Initialize Pygame
pygame.init()

# Set up the screen
screen = pygame.display.set_mode((800, 600))
pygame.display.set_caption("Particle Simulation")

# Define the circle's initial position and properties
color = (255, 255, 0)  # Yellow color
circle = Circle(100, 250, color, 30, 5)

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
    screen.fill((135, 206, 250))  # Light blue background

    # Draw the circle
    pygame.draw.circle(screen, color, (circle.x, circle.y), circle.radius)

    # Update the screen
    pygame.display.flip()

    # Delay to control the frame rate (about 60 FPS)
    pygame.time.Clock().tick(60)

# Quit Pygame
pygame.quit()
sys.exit()
