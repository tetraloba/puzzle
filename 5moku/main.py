size = 15
num = 0
world = list()
for i in range(size):
    world.append(list())
    for j in range(size):
        world[i].append('.')
def display():
    print('', end="\t")
    for i in range(size):
        print(i, end="\t")
    print()
    for i in range(size):
        print(i, end="\t")
        for j in range(size):
            print(world[i][j], end="\t")
        print()

def test5(r, c):
    dist = ((0, 1), (-1, 1), (-1, 0), (-1, -1), (0, -1), (1, -1), (1, 0), (1, 1))
    max_count = 0
    for d in range(4):
        count = 0
        t_r, t_c = r, c
        while 0 <= t_r < size and 0 <= t_c < size and world[t_r][t_c] == world[r][c]:
            count += 1
            t_r += dist[d][0]
            t_c += dist[d][1]
        d += 4 # d = (d + 4) % 8
        count -= 1
        t_r, t_c = r, c
        while 0 <= t_r < size and 0 <= t_c < size and world[t_r][t_c] == world[r][c]:
            count += 1
            t_r += dist[d][0]
            t_c += dist[d][1]
        if max_count < count:
            max_count = count
    if 5 <= max_count:
        return True
    return False

def put(r, c):
    global num
    if world[r][c] != '.':
        print("you can't put there")
        return -1
    world[r][c] = 'O' if num % 2 else 'X'
    if test5(r, c):
        return 1
    num += 1
    return 0

def world_has_blank():
    for row in world:
        for point in row:
            if point == '.':
                return True
    return False

while world_has_blank():
    display()
    print(f"{num}:", end=' ')
    r, c = map(int, input().split())
    if put(r, c) == 1:
        display()
        print("game end")
