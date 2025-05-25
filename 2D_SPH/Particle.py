from Vector2f import Vector2f
import pygame

class Particle:
    position: Vector2f
    position_last: Vector2f
    radius: float
    acceleration: Vector2f
    velocity: Vector2f
    mass: Vector2f
    density: float
    pressure: float

    def __init__(self, position: Vector2f, radius: float):
        # Initialize the circle's properties
        self.position = position
        self.position_last = position
        self.radius = radius
        self.acceleration = Vector2f(10,10)

        self.density = 998 # for water (in kg/m^3)
        self.surfaceTension = 0.0728 # for water (in N/m)
        self.color = (13, 124, 189) # Currently not set from constructor

        self.velocity = Vector2f(0,0)
        self.mass = 1.0
        self.density = 0.0
        self.pressure = 0.0

    # Move particle based on verlet integration formula
    def update(self, dt: float):
        self.position += self.velocity * dt

        self.acceleration = Vector2f()

    def getMass(self):
        return self.mass

    def getDensity(self):
        return self.density

    def setDensity(self, d: float):
        self.density = d

    def setPressure(self, p: float):
        self.pressure = p

    def getPressure(self):
        return self.pressure
    
    def setForce(self, f: float):
        self.force = f

    def getForce(self):
        return self.force

    def setAcceleration(self, a: Vector2f):
        self.acceleration = a

    def getAcceleration(self):
        return self.acceleration

    def accelerate(self, a: Vector2f):
        self.acceleration += a

    def setVelocity(self, v: Vector2f):
        self.velocity = v

    def getVelocity(self):
        return self.velocity
    
    def getColorFromVelocity(self):
        return (0,0,255)
        """
        magnitude = self.velocity.distance()

        # Based on simulation scale
        max_velocity = 200.0
        
        # Normalize brightness (clamped between 0 and 1)
        brightness = min(magnitude / max_velocity, 1)

        # Apply a non-linear curve (ease-in) for smoother transition
        brightness = brightness ** 0.5  # square root to make changes smoother at low speeds

        r, g, b = self.color

        # Mix original color with white based on brightness (linear interpolation)
        r = int(r + (255 - r) * brightness)
        g = int(g + (255 - g) * brightness)
        # Keep blue as is or slightly increase for more water feel
        b = int(b + (255 - b) * (brightness * 0.3))  # subtle increase in blue

        # Clamp final colors to 0-255 (just in case)
        r = max(0, min(255, r))
        g = max(0, min(255, g))
        b = max(0, min(255, b))

        return (r, g, b)
        """

    # physically inspired but computationally lightweight 
    def getWeberNumber(self):
        speed = self.getVelocity().distance()
        weberNumber = (self.density * speed * speed * self.radius) / self.surfaceTension
        return weberNumber

    def render(self, screen: pygame.Surface):
        r, g, b = self.getColorFromVelocity()
        pygame.draw.circle(screen, (r, g, b), (self.position.x, self.position.y), self.radius)
