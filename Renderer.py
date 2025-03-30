import pygame
from Solver import Solver

class Renderer:
    solver: Solver
    screen: pygame.Surface

    def __init__(self, solver: Solver, screen: pygame.Surface):
        self.solver = solver
        self.screen = screen

    def render(self, screen):
        for obj in self.solver.objects:
            obj.render(self.screen)