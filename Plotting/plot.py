import matplotlib.pyplot as plt

def main():
	file1 = open("queue_density.txt")
	file2 = open("dynamic_density.txt")
	file3 = open("frame_num.txt")
	queue = (file1.read()).split(",")[:-1]
	queue = [float(x) for x in queue]
	dyna = (file2.read()).split(",")[:-1]
	dyna = [float(x) for x in dyna]
	frames = (file3.read()).split(",")[:-1]
	frames = [int(x) for x in frames]
	# frames = [x/15 for x in frames]
	plt.figure()
	plt.xlabel("Time (in seconds)")
	plt.ylabel("Density")
	plt.plot(frames,queue,label="Queue Density")
	plt.plot(frames,dyna,label="Dynamic Density")
	plt.legend()
	plt.savefig("plot.png",dpi=200)
	plt.show()
if __name__=="__main__":
	main()
