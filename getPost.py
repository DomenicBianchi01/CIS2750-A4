#!/usr/bin/python

import os
import sys
import operator
import commands
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

		return self.stream + "<br>Sender: " + self.sender + "<br>" + self.date + "<br>" + str(self.text) + "<br>"

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
	dataArray = []
	userArray = []
	streamArray = []
	newByteIndexs = []
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

	return {"unreadArray": unreadArray, "readArray": readArray}

def getPost(username, streamName, lastPostRead, toggleMode, maxCount, timeStamp, pageMode):

	textToAdd = ""
	streamName2 = ""
	lines = []
	streamNames = []
	unreadPostArray = []
	readPostArray = []
	postArray = []

	if (streamName == "all"):

		mergedArray = []
		readPostCount = 0

		outputResult = subprocess.Popen(["./db", "-getStreams", username], stdout=subprocess.PIPE)

		for name in outputResult.stdout:
			name = name.strip("\n")
			streamNames.append(name)

		returnArray = loadAllStreams(streamNames, username, timeStamp, toggleMode, pageMode)
		unreadPostArray = returnArray["unreadArray"]
		readPostArray = returnArray["readArray"]

		mergedArray = readPostArray + unreadPostArray

		if (toggleMode == 2):
			mergedArray = sorted(mergedArray, key=operator.attrgetter('sender'))

		try:

			for postIndex in range(0,lastPostRead+1):
				if (mergedArray[postIndex].stream.strip("\n") == mergedArray[lastPostRead].stream.strip("\n")):
					readPostCount = readPostCount + 1

			maxCount = 0

			for singlePost in mergedArray:
				if (singlePost.stream == mergedArray[lastPostRead].stream):
					maxCount = maxCount + 1

			mergedArray[lastPostRead].stream = mergedArray[lastPostRead].stream.replace("\n", "<br>")
			mergedArray[lastPostRead].sender = mergedArray[lastPostRead].sender.replace("\n", "<br>")
			mergedArray[lastPostRead].date = mergedArray[lastPostRead].date.replace("\n", "<br>")
			mergedArray[lastPostRead].text = mergedArray[lastPostRead].text.replace("\n", "<br>")

			print(mergedArray[lastPostRead].stream)
			print(mergedArray[lastPostRead].sender)
			print(mergedArray[lastPostRead].date)
			print(mergedArray[lastPostRead].text)
			
			if (toggleMode == 1):
				subprocess.check_output(["./db", "-markRead", mergedArray[lastPostRead].stream[8:-4], username, str(readPostCount), str(pageMode), str(toggleMode), str(maxCount)])
		except IndexError:
			print("No unread messages")

	else:

		#Call database program (db.c) to get post
		postOutput = subprocess.check_output(["./db", "-getPost", streamName, str(lastPostRead), str(maxCount), username, str(pageMode), str(toggleMode), "1", "1", "0"])
		postOutput = postOutput.replace("\n", "<br>")

		textToAdd = "<p>" + postOutput + "</p>"

		print(textToAdd)

def programLoop():

	username = ""
	streamName = ""
	lastPostRead = 0

	username = sys.argv[1]
	streamName = sys.argv[2]
	postNum = int(sys.argv[3])
	toggleMode = int(sys.argv[4])
	maxCount = int(sys.argv[5])
	timeStamp = int(sys.argv[6])
	pageMode = int(sys.argv[7])
	
	getPost(username, streamName, postNum, toggleMode, maxCount, timeStamp, pageMode)

def main():

	programLoop()

if __name__ == "__main__":

	main()