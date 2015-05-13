import random
import sys

N = int(sys.argv[1])
C = int(sys.argv[2])

a = []
for i in range(N):
    a.append(str(random.randint(0, C - 1)))

f = open("parts_file", "w")
f.write("\n".join(a))
f.close()
