import pygame
from Solver import Solver
from Boundary import Boundary

class Renderer:
    solver: Solver
    screen: pygame.Surface
    boundary: Boundary

    def __init__(self, solver: Solver, screen: pygame.Surface, boundary: Boundary):
        self.solver = solver
        self.screen = screen
        self.boundary = boundary

    def render(self):
        # self.boundary.render(self.screen)
        for obj in self.solver.objects:
            obj.render(self.screen)