count = 0
val = 0.0
thread = 0
with open("ss.txt") as infile:
    for line in infile:
        if "5_method" in line:
        	l = line.split()
        	count += 1
        	thread = (l[4].split("/"))
        	if len(thread)>1:
        		thread = int(thread[1])
        	else:
        		thread = 1
        	val += (float(l[2]))/thread
val = val / count
print(val/2)