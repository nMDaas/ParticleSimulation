class Vector2f:
    x: float
    y: float

    def __init__(self, x = 0.0, y = 0.0):
        self.x = x
        self.y = y

    def __add__(self, other):
        if isinstance(other, Vector2f):  # Ensure the other operand is a Vector2f
            return Vector2f(self.x + other.x, self.y + other.y)
        
    def __sub__(self, other):
        if isinstance(other, Vector2f):  # Ensure the other operand is a Vector2f
            return Vector2f(self.x - other.x, self.y - other.y)
        
    def __mul__(self, scalar: float):
        if isinstance(scalar, (int, float)):  # Ensure the scalar is a number
            return Vector2f(self.x * scalar, self.y * scalar)