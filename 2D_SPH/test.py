from Vector2f import Vector2f
dt = (1/60)
substeps = 1
position = Vector2f(400,300)
position_last = Vector2f(391.6666666666667, 291.6666666666667)
acceleration = Vector2f(0,1000)

steps = 1

for i in range(steps):
    displacement = position - position_last
    position_last = position
    a = position + displacement
    b = acceleration * (dt * dt)
    position = a + b
    acceleration = Vector2f()
    print(f"{position_last} | {displacement} | {a} | {b} | {position}")

print("")

dt = (1/60)
substeps = 4
dt = substeps/dt
position = Vector2f(400,300)
position_last = Vector2f(391.6666666666667, 291.6666666666667)
acceleration = Vector2f(0,250)

steps = 4

for i in range(steps):
    displacement = position - position_last
    position_last = position
    a = position + displacement
    b = acceleration * (dt * dt)
    position = a + b
    acceleration = Vector2f()
    print(f"{position_last} | {displacement} | {a} | {b} | {position}")
