import psutil
import os
import subprocess

def main():
	with open("ss.txt", "w") as outfile:
		subprocess.call("top", shell=True, stdout=outfile)
if __name__=="__main__":
	main()