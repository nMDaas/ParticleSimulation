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
        self.substeps = 8

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

    def checkCollisions(self):
        for i in range(len(self.objects)):
            obj_i = self.objects[i]
            for j in range(len(self.objects)):
                if (i == j): continue
                else:
                    obj_j = self.objects[j]
                    v = obj_i.position - obj_j.position
                    dist = math.sqrt(v.x * v.x + v.y * v.y)
                    min_dist = obj_i.radius + obj_j.radius
                    if (dist < min_dist):
                        n = v / dist # normalize
                        total_mass = obj_i.radius * obj_i.radius + obj_j.radius * obj_j.radius
                        mass_ratio = (obj_i.radius * obj_i.radius)/total_mass
                        delta = 0.5 * (min_dist - dist)
                        # Larger particles move less
                        obj_i.position += (n * (1 - mass_ratio) * delta)/self.substeps
                        obj_j.position -= (n * mass_ratio * delta)/self.substeps

    def update(self):
        substep_dt = self.step_dt / self.substeps

        for i in range(self.substeps):
            self.applyGravity()
            self.applyBoundary()
            self.checkCollisions()
            self.updateObjects(substep_dt)
            
    def setParticleVelocity(self, object: Particle, v: Vector2f):
        object.setVelocity(v, self.step_dt/self.substeps)


        