#!/usr/bin/python

import os
import sys
import subprocess

def markPosts(username, streamName, numOfPosts):

	#Mark all posts read in all streams
	if (streamName == "all"):

		outputResult = subprocess.Popen(["./db", "-getStreams", username], stdout=subprocess.PIPE)

		#Loop through each stream
		for name in outputResult.stdout:
			name = name.strip("\n")
			subprocess.check_output(["./db", "-markAll", name, username])

	#Mark all posts in the specified stream read
	else:
		subprocess.check_output(["./db", "-markAll", streamName, username])
		
def main():

	markPosts(sys.argv[2], sys.argv[1], sys.argv[3]);

if __name__ == "__main__":

	main()