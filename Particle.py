from Vector2f import Vector2f

class Particle:
    position: Vector2f
    color: tuple
    radius: float
    speed: float

    def __init__(self, position: Vector2f, color: tuple, radius: float, speed: float):
        # Initialize the circle's properties
        self.position = position
        self.color = color
        self.radius = radius
        self.speed = speed

    def move(self):
        # Move the circle horizontally by its speed value
        self.position += Vector2f(self.speed, 0)