import math
from Particle import Particle
from Vector2f import Vector2f
from Boundary import Boundary
import random

class Solver:
    boundary: Boundary

    def __init__(self, boundary: Boundary):
        self.objects = []
        self.gravity = Vector2f(0.0, 200.0)
        self.step_dt = 1.0/60
        self.boundary = boundary
        self.substeps = 8
        self.mouseStrength = 2000.0

        self.weberThreshold = 400000.0

        self.smoothing_length = 30.0 # the higher this value, the more neighbouring particles contribute to a particle's simulation
        self.rest_density = 1000.0
        self.gas_constant = 2000.0
        self.viscosity = 250.0

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
                obj.setVelocity(Vector2f(v.x, v.y * -1))
            if ((obj.position.y - obj.radius) <= 0): # upper boundary of box
                obj.position.y = 0 + obj.radius  # reposition to be inside boundary
                v = obj.getVelocity()
                obj.setVelocity(Vector2f(v.x, v.y * -1))
            if ((obj.position.x + obj.radius) >= 800): # right boundary of box
                obj.position.x = 800 - obj.radius  # reposition to be inside boundary
                v = obj.getVelocity()
                obj.setVelocity(Vector2f(v.x * -1, v.y))
            if ((obj.position.x - obj.radius) <= 0): # left boundary of box
                obj.position.x = 0 + obj.radius  # reposition to be inside boundary
                v = obj.getVelocity()
                obj.setVelocity(Vector2f(v.x * -1, v.y))

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
                obj.setVelocity(2.0 * (incident_velocity.x * surface.x + incident_velocity.y * surface.y) * surface - incident_velocity)

    def updateObjects(self, dt: float):
        self.integrateForces(dt)
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
            #print("weber: %s" % str(weberNumber))
            #print("currentTime: %s" % str(currentTime))
            if (weberNumber > self.weberThreshold and currentTime > 10.0):
                self.splitParticle(self.objects[i])
                particlesToRemove.append(self.objects[i])

        # Remove particles that have split safely after the loop is complete
        for obj in particlesToRemove:
            self.objects.remove(obj)

    def checkCollisions(self):
        restitution = 0.3  # bounciness (0 = no bounce, 1 = perfect elastic)

        for i in range(len(self.objects)):
            obj_i = self.objects[i]
            for j in range(i + 1, len(self.objects)):
                obj_j = self.objects[j]

                # Vector from obj_j to obj_i
                v = obj_i.position - obj_j.position
                dist = v.distance()
                min_dist = obj_i.radius + obj_j.radius

                if dist < min_dist and dist > 0:
                    # Normalize collision normal
                    n = v / dist

                    # Positional correction to resolve overlap
                    total_mass = obj_i.radius**2 + obj_j.radius**2
                    mass_ratio_i = (obj_i.radius**2) / total_mass
                    mass_ratio_j = (obj_j.radius**2) / total_mass

                    delta = 0.5 * (min_dist - dist)
                    obj_i.position += n * (1 - mass_ratio_i) * delta / self.substeps
                    obj_j.position -= n * mass_ratio_j * delta / self.substeps

                    # Velocity correction to reduce bouncing

                    # Relative velocity
                    relative_velocity = obj_i.getVelocity() - obj_j.getVelocity()
                    velocity_along_normal = relative_velocity.x * n.x + relative_velocity.y * n.y

                    # Only correct if particles move toward each other
                    if velocity_along_normal < 0:
                        # Approximate mass with radius squared if no mass property
                        mass_i = obj_i.radius**2
                        mass_j = obj_j.radius**2

                        impulse_magnitude = -(1 + restitution) * velocity_along_normal
                        impulse_magnitude /= (1/mass_i + 1/mass_j)

                        impulse = n * impulse_magnitude

                        obj_i.setVelocity(obj_i.getVelocity() + impulse / mass_i)
                        obj_j.setVelocity(obj_j.getVelocity() - impulse / mass_j)

            
    def setParticleVelocity(self, object: Particle, v: Vector2f):
        object.setVelocity(v)

    # Kernel Functions: 

    """
    To compute the density contribution of a neighboring particle
    r = distance between 2 particles
    h = smoothing radius (can be thought as maximum distance at which a particle can affect others)
    (h**2 - r**2)**3 
        --> smaller r (closer particles) means this value is large --> more influence
        --> When r = h, this becomes 0 --> no influence
    (4 / (math.pi * h**8))
        --> normalization constant used in the 2D Poly6 kernel
        --> ensures that the total influence across space integrates to 1
        --> This constant is different in 3D!
    """
    def poly6_kernel(self, r, h):
        if 0 <= r < h:
            return (4 / (math.pi * h**8)) * (h**2 - r**2)**3
        return 0.0

    """
    To compute the pressure force direction and magnitude
    r_vec = 2D vector that points from one particle to another
    h = the smoothing radius, or the "influence range" of the kernel.
    -30 / (math.pi * h**5) --> constant derived from the math of the Spiky kernel
    (h - r)**2
        --> r is close to 0 (particles are close) --> large influence
        --> r is close to h (particles farther) --> smaller influence
    """
    def spiky_gradient(self, r_vec: Vector2f, h):
        r = r_vec.distance() # magnitude of r_vec

        # check if within range
        if 0 < r < h: 
            factor = -30 / (math.pi * h**5) * (h - r)**2
            return r_vec.normalize() * factor
        return Vector2f(0, 0) # if outside range, gradient is 0 so particles don't influence each other
    
    # Calculate density and pressure for each particle in solver
    def compute_densities_and_pressures(self):
        for pi in self.objects:
            density = 0.0 
            for pj in self.objects:
                r = (pi.position - pj.position).distance() # get distance between pi and pj

                # this calculation comes from how density is defined in SPH
                #   --> kernel tells us how much that mass influences the surrounding area, depending on distance
                #   --> density is the sum of nearby fluid mass, weighted by proximity
                density += pj.mass * self.poly6_kernel(r, self.smoothing_length)

            pi.setDensity(density)
            pi.setPressure(self.gas_constant * (density - self.rest_density))

    # To compute the viscosity force magnitude
    # smaller r --> h - r is bigger (smaller distance between particles) --> greater influence 
    def viscosity_laplacian(self, r, h):
        if 0 <= r < h: # if within range
            return 20 / (3 * math.pi * h**5) * (h - r)
        return 0.0 # if r is negative or greater than h, no influence

    def compute_forces(self):
        for pi in self.objects:
            pressure_force = Vector2f(0, 0)
            viscosity_force = Vector2f(0, 0)
            for pj in self.objects:
                if pi == pj: # skip
                    continue
                r_vec = pi.position - pj.position # vector from pj pointing to pi
                r = r_vec.distance() # magnitude of vector
                if r < self.smoothing_length: # If within range (to only interact with nearby particles)
                    # Pressure force
                    pressure = (pi.getPressure() + pj.getPressure()) / 2.0 # get average pressure

                    # spiky_gradient returns a direction (based on SPH kernel gradient) in which pressure acts
                    # The result is scaled by -mass / density * pressure, which follows SPH fluid mechanics
                    # - sign because pressure repels particles
                    pressure_force += self.spiky_gradient(r_vec, self.smoothing_length) * (-pj.getMass() / pj.getDensity() * pressure)

                    # Viscosity (resists relative motion)
                    # If two particles move differently, viscosity tries to smooth that out
                    # Scaled by SPH viscosity_laplacian (which depends on distance)
                    # Also scaled by mass / density, similar to pressure
                    viscosity = self.viscosity * (pj.getVelocity() - pi.getVelocity())
                    viscosity_force += viscosity * (pj.mass / pj.density) * self.viscosity_laplacian(r, self.smoothing_length)

            # Add up all forces: pressure + viscosity + gravity
            pi.setForce(pressure_force + viscosity_force + self.gravity * pi.getDensity())

    def integrateForces(self, dt):
        for p in self.objects:
            a = p.getForce() / p.getDensity()
            p.setAcceleration(a)
            p_velocity = p.getVelocity()  + p.getAcceleration() * dt
            p.setVelocity(p_velocity)
            p.position += p.getVelocity() * dt

    def update(self, currentTime):
        substep_dt = self.step_dt / self.substeps

        for i in range(self.substeps):
            #self.applyGravity()
            #self.applyBoundary()
            self.applyBorder()
            self.checkSplitting(currentTime)
            self.checkCollisions()

            self.compute_densities_and_pressures()
            self.compute_forces()

            self.updateObjects(substep_dt)


        