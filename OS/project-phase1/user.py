import subprocess
import os

n = int(input())


command = "echo %d > /dev/getpcb" % n
command2 = "cat /dev/getpcb > /dev/getpcb.txt"


os.system(command)
os.system(command2)



with open('/dev/getpcb.txt', 'r') as f:
    print(f.read())
