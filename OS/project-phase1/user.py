import subprocess
import os
import time
n = int(input())
period = int(input())

command = "echo %d > /dev/getpcb" % n
command2 = "cat /dev/getpcb > /dev/getpcb.txt"

i = 0

while i < period:
	

	os.system(command)
	os.system(command2)




	with open('/dev/getpcb.txt', 'r') as f:
	    print(f.read())
	
	
	i += 1
	print("------------------------------------------------------------")
	time.sleep(60)
	
