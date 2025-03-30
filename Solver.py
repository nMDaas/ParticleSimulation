from Particle import Particle
from Vector2f import Vector2f

class Solver:
    def __init__(self):
        self.objects = []
        self.gravity = Vector2f(0.0, 1000.0)
        self.step_dt = 1.0/60

    def addObject(self, position: Vector2f, radius: float):
        newParticle = Particle(position, radius)
        self.objects.append(newParticle)

    def applyGravity(self):
        for obj in self.objects:
            obj.accelerate(self.gravity)

    def updateObjects(self, dt: float):
        for obj in self.objects:
            obj.update(dt)

    def update(self):
        self.applyGravity()
        self.updateObjects(self.step_dt)

        
        