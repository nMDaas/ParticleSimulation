import math
from Particle import Particle
from Vector2f import Vector2f
from Boundary import Boundary

class Solver:
    boundary: Boundary

    def __init__(self, boundary: Boundary):
        self.objects = []
        self.gravity = Vector2f(0.0, 1000.0)
        self.step_dt = 1.0/60
        self.boundary = boundary

    def addObject(self, position: Vector2f, radius: float):
        newParticle = Particle(position, radius)
        self.objects.append(newParticle)
        return newParticle

    def getObjects(self):
        return self.objects

    def mousePull(self, pos: Vector2f):
        for obj in self.objects:
            dir = pos - obj.position
            dist = math.sqrt(dir.x * dir.x + dir.y * dir.y)
            obj.accelerate(dir * max(0,10 * (120 - dist)))

    def mousePush(self, pos: Vector2f):
        for obj in self.objects:
            dir = obj.position - pos
            dist = math.sqrt(dir.x * dir.x + dir.y * dir.y)
            obj.accelerate(dir * max(0,10 * (120 - dist)))

    def applyGravity(self):
        for obj in self.objects:
            obj.accelerate(self.gravity)

    def applyBoundary(self):
        for obj in self.objects:
            r = self.boundary.center - obj.position # the vector from the object position to center of boundary
            dist = math.sqrt((r.x * r.x) + (r.y * r.y)) # calculate that vector distance
            if (dist > self.boundary.radius - obj.radius):
                # bounce
                normalized_r = r / dist # normalize the vector
                surface = Vector2f(-normalized_r.y, normalized_r.x) # surface vector that is tangent to r
                incident_velocity = obj.getVelocity()
                obj.position = self.boundary.center - normalized_r * (self.boundary.radius - obj.radius)
                obj.setVelocity(2.0 * (incident_velocity.x * surface.x + incident_velocity.y * surface.y) * surface - incident_velocity, 1.0)

    def updateObjects(self, dt: float):
        for obj in self.objects:
            obj.update(dt)

    def update(self):
        self.applyGravity()
        self.applyBoundary()
        self.updateObjects(self.step_dt)

    def setParticleVelocity(self, object: Particle, v: Vector2f):
        object.setVelocity(v, self.step_dt)


        