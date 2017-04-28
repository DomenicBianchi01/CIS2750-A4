#!/usr/bin/python

import os
import sys
import operator
import subprocess

#An object of this class represents the byte locations and user of a single post
class Post:

	stream = ""
	sender = ""
	text = ""
	date = ""
	intDate = 0

	def __init__(self, stream, sender, intDate, text, date):
		
		self.stream = stream
		self.sender = sender
		self.text = text
		self.date = date
		self.intDate = intDate

	def __str__(self):

		return tempStreamName + "<br>Sender: " + self.sender + "<br>" + self.date + "<br>" + str(self.text)

#This function takes in a date in the format of a string and converts that string into a number that represents the date.
#For example, Feb. 14, 2017 10:23 will be converted to 021420171023
def parseDate(date):
	
	newDate = ""

	date = date.strip("\n")
	date = date.split(" ")

	#Conver the month name into its equivalent integer value
	if (date[0] == "Jan."):
		newDate = "01"
	elif (date[0] == "Feb."):
		newDate = "02"
	elif (date[0] == "Mar."):
		newDate = "03"
	elif (date[0] == "Apr."):
		newDate = "04"
	elif (date[0] == "May."):
		newDate = "05"
	elif (date[0] == "Jun."):
		newDate = "06"
	elif (date[0] == "Jul."):
		newDate = "07"
	elif (date[0] == "Aug."):
		newDate = "08"
	elif (date[0] == "Sep."):
		newDate = "09"
	elif (date[0] == "Oct."):
		newDate = "10"
	elif (date[0] == "Nov."):
		newDate = "11"
	elif (date[0] == "Dec."):
		newDate = "12"

	#date[1] contains the day of the month. Remove the comma.
	#date[2] contains the year
	#date[3] contains the time (24-hour clock). Remove the : from the time
	newDate = date[2] + newDate + date[1].strip(",") + date[3].replace(":", "")

	return newDate

def loadAllStreams(allFileNames, username, timeStamp, toggleMode, pageMode):

	unreadArray = []
	readArray = []
	userArray = []
	streamArray = []
	dataInfo = []
	unreadArray = []
	postOffset = []
	parsedDate = ""
	textToAdd = ""
	lineCount = 0
	counter = -1
	startByte = 0
	endByte = 0
	i = 0
	j = 0

	for name in allFileNames:
		userArray.append(name)
		streamArray.append(name)

	#Get data that tells the program what the last post read was in each stream
	for userFile in userArray:
		#Open the user file
		postOutput = subprocess.check_output(["./db", "-getNumOfPosts", userFile, username])
		newArray = postOutput.split("\n")
		postOffset.append(newArray[1])
		dataInfo.append(newArray[0])

	#Get unread posts
	for streamName in streamArray:

		j = 0
		postToAdd = ""
		streamNameToAdd = ""
		dateToAdd = ""
		senderToAdd = ""

		outputResult = subprocess.Popen(["./db", "-getPost", streamName, postOffset[i], dataInfo[i], username, str(pageMode), str(toggleMode), "0", str(dataInfo[i]), "1"], stdout=subprocess.PIPE)

		for element in outputResult.stdout:

			if (j == 0):
				streamNameToAdd = element
			elif (j == 1):
				senderToAdd = element
			elif (j == 2):
				dateToAdd = element
			elif (j == 3):
				j = j + 1
				continue
			elif (element != "ENDOFPOST\n"):
				postToAdd = postToAdd + element
			else:
				intDate = parseDate(dateToAdd)

				if (int(intDate) < timeStamp):
					post = Post(streamNameToAdd, senderToAdd, intDate, postToAdd, dateToAdd);
					unreadArray.append(post)

				postToAdd = ""
				streamNameToAdd = ""
				dateToAdd = ""
				senderToAdd = ""
				j = -1

			j = j + 1

		j = 0
		outputResult = subprocess.Popen(["./db", "-getPost", streamName, "0", dataInfo[i], username, str(pageMode), str(toggleMode), "0", str(postOffset[i]), "1"], stdout=subprocess.PIPE)

		for element in outputResult.stdout:

			if (j == 0):
				streamNameToAdd = element
			elif (j == 1):
				senderToAdd = element
			elif (j == 2):
				dateToAdd = element
			elif (j == 3):
				j = j + 1
				continue
			elif (element != "ENDOFPOST\n"):
				postToAdd = postToAdd + element
			else:
				intDate = parseDate(dateToAdd)

				if (int(intDate) < timeStamp):
					post = Post(streamNameToAdd, senderToAdd, intDate, postToAdd, dateToAdd);
					readArray.append(post)

				postToAdd = ""
				streamNameToAdd = ""
				dateToAdd = ""
				senderToAdd = ""
				j = -1

			j = j + 1
			
		i = i + 1

	unreadArray = sorted(unreadArray, key=operator.attrgetter('intDate'))
	readArray = sorted(readArray, key=operator.attrgetter('intDate'))

	return {"postCount": len(unreadArray) + len(readArray), "startingIndex": len(readArray)}

def getNum(username, streamName, timeStamp, toggleMode, pageMode):

	lastPostRead = 0
	count = 0
	offsetCounter = 0
	fileCount = 0
	postCount = 0
	streamNames = []

	if (streamName == "all"):

		outputResult = subprocess.Popen(["./db", "-getStreams", username], stdout=subprocess.PIPE)

		for name in outputResult.stdout:
			name = name.strip("\n")
			streamNames.append(name)

		returnArray = loadAllStreams(streamNames, username, timeStamp, toggleMode, pageMode)

		print returnArray["postCount"]
		print returnArray["startingIndex"]

	else:

		postOutput = subprocess.check_output(["./db", "-getNumOfPosts", streamName, username])

		print postOutput

def main():

	getNum(sys.argv[2], sys.argv[1], sys.argv[3], sys.argv[4], sys.argv[5]);

if __name__ == "__main__":

	main()