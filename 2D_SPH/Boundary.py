from Vector2f import Vector2f
import pygame


class Boundary():
    center: Vector2f
    radius: float

    def __init__(self, center: Vector2f, radius: float):
        self.center = center
        self.radius = radius

    def getBoundaryCenter(self):
        return self.center
    
    def getBoundaryRadius(self):
        return self.radius
    
    def render(self, screen: pygame.Surface):
        pygame.draw.circle(screen, (0,0,0), (self.center.x,self.center.y), self.radius)
