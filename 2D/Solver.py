import math
from Particle import Particle
from Vector2f import Vector2f
from Boundary import Boundary
import random

class Solver:
    boundary: Boundary

    def __init__(self, boundary: Boundary):
        self.objects = []
        self.gravity = Vector2f(0.0, 1000.0)
        self.step_dt = 1.0/60
        self.boundary = boundary
        self.substeps = 8
        self.mouseStrength = 70.0

        self.weberThreshold = 400000.0

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
            obj.accelerate(dir * max(0,self.mouseStrength * (120 - dist)))

    def mousePush(self, pos: Vector2f):
        for obj in self.objects:
            dir = obj.position - pos
            dist = math.sqrt(dir.x * dir.x + dir.y * dir.y)
            obj.accelerate(dir * max(0,self.mouseStrength * (120 - dist)))

    def applyGravity(self):
        for obj in self.objects:
            obj.accelerate(self.gravity)

    # Apply border of the window
    def applyBorder(self):
        for obj in self.objects:
            if ((obj.position.y + obj.radius) >= 600): # lower boundary of box
                obj.position.y = 600 - obj.radius # reposition to be inside boundary
                v = obj.getVelocity()
                obj.setVelocity(Vector2f(v.x, v.y * -1), 1.0)
            if ((obj.position.y - obj.radius) <= 0): # upper boundary of box
                obj.position.y = 0 + obj.radius  # reposition to be inside boundary
                v = obj.getVelocity()
                obj.setVelocity(Vector2f(v.x, v.y * -1), 1.0)
            if ((obj.position.x + obj.radius) >= 800): # right boundary of box
                obj.position.x = 800 - obj.radius  # reposition to be inside boundary
                v = obj.getVelocity()
                obj.setVelocity(Vector2f(v.x * -1, v.y), 1.0)
            if ((obj.position.x - obj.radius) <= 0): # left boundary of box
                obj.position.x = 0 + obj.radius  # reposition to be inside boundary
                v = obj.getVelocity()
                obj.setVelocity(Vector2f(v.x * -1, v.y), 1.0)

    # Apply circular boundary
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

    def splitParticle(self, obj: Particle):
        numNewParticles = random.randint(1,2) # Create 2-4 new particles of smaller radius
        newRadius = obj.radius / math.sqrt(numNewParticles)
        objSpeed = obj.getVelocity().distance()

        for n in range(numNewParticles):
            angle = random.random() * 2 * math.pi
            speed = objSpeed * 0.6 + random.random() * 0.4
            vx = speed * math.cos(angle)
            vy = speed * math.sin(angle)

            # Small offset to avoid particles starting at the exact same position
            offset_x = random.uniform(-0.1, 0.1)
            offset_y = random.uniform(-0.1, 0.1)
            new_position = obj.position + Vector2f(offset_x, offset_y)  # Apply offset

            newParticle = self.addObject(new_position, newRadius)
            self.setParticleVelocity(newParticle, Vector2f(vx, vy))

    def checkSplitting(self, currentTime):
        particlesToRemove = []

        for i in range(len(self.objects)):
            weberNumber = self.objects[i].getWeberNumber()
            print("weber: %s" % str(weberNumber))
            print("currentTime: %s" % str(currentTime))
            if (weberNumber > self.weberThreshold and currentTime > 10.0):
                self.splitParticle(self.objects[i])
                particlesToRemove.append(self.objects[i])

        # Remove particles that have split safely after the loop is complete
        for obj in particlesToRemove:
            self.objects.remove(obj)

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

    def update(self, currentTime):
        substep_dt = self.step_dt / self.substeps

        for i in range(self.substeps):
            self.applyGravity()
            #self.applyBoundary()
            self.applyBorder()
            self.checkSplitting(currentTime)
            self.checkCollisions()
            self.updateObjects(substep_dt)
            
    def setParticleVelocity(self, object: Particle, v: Vector2f):
        object.setVelocity(v, self.step_dt/self.substeps)


        