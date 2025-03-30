import pygame
import sys

# Initialize Pygame
pygame.init()

# Set up the screen
screen = pygame.display.set_mode((800, 600))
pygame.display.set_caption("Circle Animation with Pygame")

# Define the circle's initial position and properties
x, y = 100, 250
radius = 30
color = (255, 255, 0)  # Yellow color
speed = 5

# Main game loop
running = True
while running:
    # Handle events (e.g., window close)
    for event in pygame.event.get():
        if event.type == pygame.QUIT:
            running = False

    # Move the circle
    x += speed
    if x > 800:  # Reset the circle position when it moves off the screen
        x = -radius

    # Fill the screen with a background color
    screen.fill((135, 206, 250))  # Light blue background

    # Draw the circle
    pygame.draw.circle(screen, color, (x, y), radius)

    # Update the screen
    pygame.display.flip()

    # Delay to control the frame rate (about 60 FPS)
    pygame.time.Clock().tick(60)

# Quit Pygame
pygame.quit()
sys.exit()
