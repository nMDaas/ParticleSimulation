from Vector2f import Vector2f
import pygame

class Particle:
    position: Vector2f
    position_last: Vector2f
    radius: float
    acceleration: Vector2f

    def __init__(self, position: Vector2f, radius: float):
        # Initialize the circle's properties
        self.position = position
        self.position_last = position
        self.radius = radius
        self.acceleration = Vector2f(10,10)

        self.color = (52, 63, 120) # Currently not set from constructor

    # Move particle based on verlet integration formula
    def update(self, dt: float):
        displacement = self.position - self.position_last
        self.position_last = self.position
        self.position = self.position + displacement + self.acceleration * (dt * dt)
        self.acceleration = Vector2f()

    def accelerate(self, a: Vector2f):
        self.acceleration += a

    def setVelocity(self, v: Vector2f, dt: float):
        self.position_last = self.position - (v * dt)

    def getVelocity(self):
        return self.position - self.position_last

    def render(self, screen: pygame.Surface):
        pygame.draw.circle(screen, self.color, (self.position.x, self.position.y), self.radius)
