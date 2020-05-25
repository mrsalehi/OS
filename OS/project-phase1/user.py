import subprocess
import os
import time
n = int(input('Input pid:'))
period = int(input('Input period:'))

command = "echo %d > /dev/getpcb" % n
command2 = "cat /dev/getpcb > /dev/getpcb.txt"

i = 0

while True:
	print('Time: %d' % i)
	os.system(command)
	os.system(command2)

	with open('/dev/getpcb.txt', 'r') as f:
	    print(f.read())
	
	
	print("\n------------------------------------------------------------\n")
	time.sleep(period)
	i += period
	
