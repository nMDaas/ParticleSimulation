class Circle:
    x: float
    y: float
    color: tuple
    radius: float
    speed: float

    def __init__(self, x: float, y: float, color: tuple, radius: float, speed: float):
        # Initialize the circle's properties
        self.x = x
        self.y = y
        self.color = color
        self.radius = radius
        self.speed = speed

    def move(self):
        # Move the circle horizontally by its speed value
        self.x += self.speed