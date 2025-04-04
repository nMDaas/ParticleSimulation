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
        
    def __rmul__(self, scalar: float):
        # This is called when the scalar is on the left side
        return self.__mul__(scalar)
        
    def __truediv__(self, scalar: float):
        if isinstance(scalar, (int, float)):  # Ensure the scalar is a number
            # Ensure we are not dividing by zero
            if scalar == 0:
                raise ValueError("Cannot divide by zero.")
            return Vector2f(self.x / scalar, self.y / scalar)
        
    def __str__(self):
        return f"({self.x}, {self.y})"