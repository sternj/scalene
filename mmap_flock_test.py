import mmap
import fcntl
import os
from time import sleep
mapped_file = open('test.txt', 'r')

map = mmap.mmap(mapped_file.fileno(), 0, mmap.MAP_SHARED, mmap.PROT_READ)
pid = os.fork()

with open('test.lock', 'w+') as f:
    fcntl.lockf(f, fcntl.LOCK_EX)
    if pid != 0:
        map.seek(6)
    sleep(0.1)
    print(map.readline().rstrip().decode("ascii"))

    fcntl.lockf(f, fcntl.LOCK_UN)