class Vector2f:
    x: float
    y: float

    def __init__(self, x: float, y: float):
        self.x = x
        self.y = y

    def __add__(self, other):
        if isinstance(other, Vector2f):  # Ensure the other operand is a Vector2f
            return Vector2f(self.x + other.x, self.y + other.y)