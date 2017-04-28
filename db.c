/**********************
 Assignment #4
 Domenic Bianchi
 0921557
 March 31, 2017
 This program accesses a mysql server which replaces the text file storage system used in A3
 ************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <mysql/mysql.h>

/*This function comes from the sample mysql program Professor Calvert posted on CourseLink*/
void clrstr(char *buf){
	buf[0] = '\0';
}

int main(int argc, char *argv[]) {

	if (argc == 1) {

		printf("Not enough arguments\n");
		return 1;
	}

	MYSQL mysql;
	MYSQL_RES *res = NULL;
	MYSQL_ROW row;
	char query[512];

	/*Open the connection to the mysql database*/
	mysql_init(&mysql);
	mysql_options(&mysql, MYSQL_READ_DEFAULT_GROUP, "db");

	if (!mysql_real_connect(&mysql, "dursley.socs.uoguelph.ca", "dbianchi", "0921557", "dbianchi", 0, NULL, 0)) {
		printf("Could not connect to the host.\n");
		return 1;
	}

	if (strcmp(argv[1], "-removeAuthor") == 0) {

		/*Delete the user from the specific stream*/
		clrstr(query);
		strcat(query, "delete from ");
		strcat(query, argv[2]);
		strcat(query, "StreamUsers where name = \"");
		strcat(query, argv[3]);
		strcat(query, "\"");

		if (mysql_query(&mysql, query)){
			printf("Could not remove user.\n");
			return 1;
		}

		/*Decrement the counter that keeps track of how many streams the user is part of*/
		clrstr(query);
		strcat(query, "update Users set streamCount = streamCount-1 where name = '");
		strcat(query, argv[3]);
		strcat(query, "'");

		if (mysql_query(&mysql, query)){
			printf("Could not remove user.\n");
			return 1;
		}

		/*Remove the user all together if they no longer belong to any streams*/
		clrstr(query);
		strcat(query, "select * from Users where name = '");
		strcat(query, argv[3]);
		strcat(query, "'");

		if (mysql_query(&mysql, query)){
			printf("Could not remove user.\n");
			return 1;
		}

		if (!(res = mysql_store_result(&mysql))){
			printf("Could not remove user.\n");
			return 1;
		}

		while ((row = mysql_fetch_row(res))) {

			if (atoi(row[1]) == 0) {

				/*Delete the user*/
				clrstr(query);
				strcat(query, "delete from Users where name = \"");
				strcat(query, argv[3]);
				strcat(query, "\"");

				if (mysql_query(&mysql, query)){
					printf("Could not remove user.\n");
					return 1;
				}
			}
		}
	}
	else if (strcmp(argv[1], "-addAuthor") == 0) {

		/*First add the stream to the list of streams if it doesn't already exist*/
		clrstr(query);
		strcat(query, "create table if not exists Streams (name varchar(256) unique)");

		if (mysql_query(&mysql, query)){
			printf("Could not create table!\n");
			return 1;
		}

		clrstr(query);
		strcat(query, "insert ignore into Streams (name) values (\"");
		strcat(query, argv[2]);
		strcat(query, "\")");

		if (mysql_query(&mysql, query)){
			printf("Query failed!\n");
			return 1;
		}

		/*Now add the author to the StreamUsers table*/
		clrstr(query);
		strcat(query, "create table if not exists ");
		strcat(query, argv[2]);
		strcat(query, "StreamUsers (name varchar(256) unique, lastRead int)");

		if (mysql_query(&mysql, query)){
			printf("Could not create table!\n");
			return 1;
		}

		clrstr(query);
		strcat(query, "insert ignore into ");
		strcat(query, argv[2]);
		strcat(query, "StreamUsers (name, lastRead) values (\"");
		strcat(query, argv[3]);
		strcat(query, "\", 0)");

		if (mysql_query(&mysql, query)){
			printf("Query failed!\n");
			return 1;
		}

		/*Create the stream if it doesn't already exist*/
		clrstr(query);
		strcat(query, "create table if not exists ");
		strcat(query, argv[2]);
		strcat(query, "Stream (name varchar(256), date varchar(256), message varchar(5000))");

		if (mysql_query(&mysql, query)){
			printf("Query failed!\n");
			return 1;
		}

		/*Create generic user table and add user if it doesn't already exist*/
		clrstr(query);
		strcat(query, "create table if not exists Users (name varchar(256) unique, streamCount int)");

		if (mysql_query(&mysql, query)){
			printf("Could not create table!\n");
			return 1;
		}

		clrstr(query);
		strcat(query, "insert ignore into Users (name, streamCount) values (\"");
		strcat(query, argv[3]);
		strcat(query, "\",0)");

		if (mysql_query(&mysql, query)){
			printf("Query failed!\n");
			return 1;
		}

		clrstr(query);
		strcat(query, "update Users set streamCount = streamCount+1 where name = '");
		strcat(query, argv[3]);
		strcat(query, "'");

		if (mysql_query(&mysql, query)){
			printf("Query failed!\n");
			return 1;
		}
	}
	else if (strcmp(argv[1], "-addPost") == 0) {

		/*Add the post to the stream*/
		clrstr(query);
		strcat(query, "insert into ");
		strcat(query, argv[2]);
		strcat(query, "Stream (name, date, message) values (\"");
		strcat(query, argv[3]);
		strcat(query, "\", \"");
		strcat(query, argv[4]);
		strcat(query, "\", \"");
		strcat(query, argv[5]);
		strcat(query, "\")");

		if (mysql_query(&mysql, query)){
			printf("Could not add post!\n");
			return 1;
		}
	}
	else if (strcmp(argv[1], "-streams") == 0) {

		/*Get all the stream names*/
		clrstr(query);
		strcat(query, "select * from Streams");

		if (mysql_query(&mysql, query)) {
			printf("Could not get streams\n");
			return 1;
		}

		if (!(res = mysql_store_result(&mysql))) {
			printf("Could not get streams\n");
			return 1;
		}

		while ((row = mysql_fetch_row(res))) {

			printf("%s\n", row[0]);
		}
	}
	else if (strcmp(argv[1], "-getStreams") == 0) {

		/*Get stream names that the user has access to*/
		clrstr(query);
		strcat(query, "select * from Streams");

		if (mysql_query(&mysql, query)){
			printf("No Streams\n");
			return 1;
		}

		if (!(res = mysql_store_result(&mysql))){
			printf("No Streams\n");
			return 1;
		}

		/*Loop through all stream users and search for a match for the username*/
		while ((row = mysql_fetch_row(res))) {

			MYSQL_RES *res2;
			MYSQL_ROW row2;

			clrstr(query);
			strcat(query, "select * from ");
			strcat(query, row[0]);
			strcat(query, "StreamUsers");

			if (mysql_query(&mysql, query)){
				printf("No Streams\n");
				return 1;
			}

			if (!(res2 = mysql_store_result(&mysql))){
				printf("No Streams\n");
				return 1;
			}

			while ((row2 = mysql_fetch_row(res2))) {

				if (strcmp(argv[2], row2[0]) == 0) {

					printf("%s\n", row[0]);
				}
			}

			mysql_free_result(res2);
		}
	}
	/*Same as getStreas except formats it as radio buttons within a form*/
	else if (strcmp(argv[1], "-HTMLStreams") == 0) {

		bool streamsExist = false;

		clrstr(query);
		strcat(query, "select * from Streams");

		if (mysql_query(&mysql, query)){
			printf("No Streams\n");
			return 1;
		}

		if (!(res = mysql_store_result(&mysql))){
			printf("No Streams\n");
			return 1;
		}

		while ((row = mysql_fetch_row(res))) {

			MYSQL_RES *res2;
			MYSQL_ROW row2;

			clrstr(query);
			strcat(query, "select * from ");
			strcat(query, row[0]);
			strcat(query, "StreamUsers");

			if (mysql_query(&mysql, query)){
				printf("No Streams\n");
				return 1;
			}

			if (!(res2 = mysql_store_result(&mysql))){
				printf("No Streams\n");
				return 1;
			}

			while ((row2 = mysql_fetch_row(res2))) {

				if (strcmp(argv[2], row2[0]) == 0) {

					if (streamsExist == false) {

						printf("<form action=\"viewStream.php\" method=\"post\">\n");
						streamsExist = true;
					}

					printf("<input type=\"radio\" name=\"stream\" value=\"%s\">%s<br>\n", row[0], row[0]);
				}
			}

			mysql_free_result(res2);
		}

		if (streamsExist == false) {

			printf("<p>No Streams</p>\n");
		}
		else {

			printf("<input type=\"radio\" name=\"stream\" value=\"all\" checked>all<br>\n");
			printf("<input type=\"submit\">\n");
			printf("</form>\n");
		}

		printf("</body>\n</html>\n");
	}
	else if (strcmp(argv[1], "-clear") == 0 || strcmp(argv[1], "-reset") == 0) {

		clrstr(query);
		strcat(query, "show tables");

		if (mysql_query(&mysql, query)){
			printf("Could not complete request\n");
			return 1;
		}

		if (!(res = mysql_store_result(&mysql))){
			printf("Could not complete request\n");
			return 1;
		}

		/*Clear or delete all tables*/
		while ((row = mysql_fetch_row(res))) {

			clrstr(query);

			if (strcmp(argv[1], "-clear") == 0) {

				strcat(query, "truncate table ");
			}
			else {

				strcat(query, "drop table ");
			}

			strcat(query, row[0]);

			if (mysql_query(&mysql, query)){
				printf("Could not complete request\n");
				return 1;
			}
		}

		if (strcmp(argv[1], "-clear") == 0) {

			printf("All tables cleared\n");
		}
		else {

			printf("All tables deleted\n");
		}
	}
	else if (strcmp(argv[1], "-getPost") == 0) {

		bool foundPost = false;

		clrstr(query);
		strcat(query, "select * from ");
		strcat(query, argv[2]);

		/*Order posts by date*/
		if (atoi(argv[7]) == 1) {

			strcat(query, "Stream order by date limit ");
		}
		/*Order posts by name*/
		else {

			strcat(query, "Stream order by name limit ");
		}

		strcat(query, argv[3]);
		strcat(query, ",");
		strcat(query, argv[9]);

		if (mysql_query(&mysql, query)){
			printf("Could not display tables\n");
			return 1;
		}

		if (!(res = mysql_store_result(&mysql))){
			printf("Could not display tables\n");
			return 1;
		}

		/*Display posts*/
		while ((row = mysql_fetch_row(res)) && atoi(argv[3]) < atoi(argv[4])) {

			foundPost = true;

			printf("Stream: %s\n", argv[2]);
			printf("%s\n", row[0]);
			printf("%s\n", row[1]);
			printf("%s\n", row[2]);

			if (atoi(argv[10]) == 1) {

				printf("ENDOFPOST\n");
			}
		}

		if (foundPost == false) {

			printf("No unread messages\n");
		}
		/*Update read post index*/
		else if (atoi(argv[3]) < atoi(argv[4]) && atoi(argv[6]) == 2 && atoi(argv[7]) == 1 && atoi(argv[8]) == 1) {

			int readPostCount;

			/*Get last read post number from database*/
			/*Now get the num of posts the user has already read*/
			clrstr(query);
			strcat(query, "select * from ");
			strcat(query, argv[2]);
			strcat(query, "StreamUsers where name = '");
			strcat(query, argv[5]);
			strcat(query, "'");

			if (mysql_query(&mysql, query)){
				printf("Could not display tables\n");
				return 1;
			}

			if (!(res = mysql_store_result(&mysql))){
				printf("Could not display tables\n");
				return 1;
			}

			while ((row = mysql_fetch_row(res))) {

				readPostCount = atoi(row[1]);
			}

			/*Update the counter*/
			if (readPostCount <= atoi(argv[3])) {

				clrstr(query);
				strcat(query, "update ");
				strcat(query, argv[2]);
				strcat(query, "StreamUsers set lastRead = lastRead+1 where name = '");
				strcat(query, argv[5]);
				strcat(query, "'");

				if (mysql_query(&mysql, query)){
					printf("Could not update read post number for user\n");
					return 1;
				}
			}
		}
	}
	else if (strcmp(argv[1], "-getNumOfPosts") == 0) {

		char streamName[500];
		clrstr(streamName);

		clrstr(query);
		strcat(query, "select count(*) from ");
		strcat(query, argv[2]);
		strcat(query, "Stream");

		if (mysql_query(&mysql, query)){
			printf("Could not display tables\n");
			return 1;
		}

		if (!(res = mysql_store_result(&mysql))){
			printf("Could not display tables\n");
			return 1;
		}

		while ((row = mysql_fetch_row(res))) {

			printf("%s\n", row[0]);
		}

		/*Now get the num of posts the user has already read*/
		clrstr(query);
		strcat(query, "select * from ");
		strcat(query, argv[2]);
		strcat(query, "StreamUsers where name = '");
		strcat(query, argv[3]);
		strcat(query, "'");

		if (mysql_query(&mysql, query)){
			printf("Could not display tables\n");
			return 1;
		}

		if (!(res = mysql_store_result(&mysql))){
			printf("Could not display tables\n");
			return 1;
		}

		while ((row = mysql_fetch_row(res))) {

			printf("%s\n", row[1]);
		}
	}
	else if (strcmp(argv[1], "-posts") == 0) {

		clrstr(query);
		strcat(query, "select * from Streams");

		if (mysql_query(&mysql, query)){
			printf("Could not get posts\n");
			return 1;
		}

		if (!(res = mysql_store_result(&mysql))){
			printf("Could not get posts\n");
			return 1;
		}

		/*For each stream, get all the posts from that stream*/
		while ((row = mysql_fetch_row(res))) {

			MYSQL_RES *res2;
			MYSQL_ROW row2;

			clrstr(query);
			strcat(query, "select * from ");
			strcat(query, row[0]);
			strcat(query, "Stream");

			if (mysql_query(&mysql, query)){
				printf("Could not get posts\n");
				return 1;
			}

			if (!(res2 = mysql_store_result(&mysql))){
				printf("Could not get posts\n");
				return 1;
			}

			while ((row2 = mysql_fetch_row(res2))) {

				printf("Stream: %s\n", row[0]);
				printf("%s\n", row2[0]);
				printf("%s\n", row2[1]);
				printf("%s\n", row2[2]);
				printf("-----\n");
			}

			mysql_free_result(res2);
		}
	}
	else if (strcmp(argv[1], "-markRead") == 0) {

		if (atoi(argv[4]) <= atoi(argv[7]) && atoi(argv[5]) == 2 && atoi(argv[6]) == 1) {

			int readPostCount;

			/*Get last read post number from database*/
			/*Now get the num of posts the user has already read*/
			clrstr(query);
			strcat(query, "select * from ");
			strcat(query, argv[2]);
			strcat(query, "StreamUsers where name = '");
			strcat(query, argv[3]);
			strcat(query, "'");

			if (mysql_query(&mysql, query)){
				printf("Could not display tables\n");
				return 1;
			}

			if (!(res = mysql_store_result(&mysql))){
				printf("Could not display tables\n");
				return 1;
			}

			while ((row = mysql_fetch_row(res))) {

				readPostCount = atoi(row[1]);
			}

			/*Update counter*/
			if (readPostCount <= atoi(argv[4])) {

				clrstr(query);
				strcat(query, "update ");
				strcat(query, argv[2]);
				strcat(query, "StreamUsers set lastRead = ");
				strcat(query, argv[4]);
				strcat(query, " where name = '");
				strcat(query, argv[3]);
				strcat(query, "'");

				if (mysql_query(&mysql, query)){
					printf("Could not update read post number for user\n");
					return 1;
				}
			}
		}
	}
	else if (strcmp(argv[1], "-markAll") == 0) {

		clrstr(query);
		strcat(query, "update ");
		strcat(query, argv[2]);
		strcat(query, "StreamUsers set lastRead = (select count(*) from ");
		strcat(query, argv[2]);
		strcat(query, "Stream) where name = '");
		strcat(query, argv[3]);
		strcat(query, "'");

		if (mysql_query(&mysql, query)){
			printf("Could not mark all posts\n");
			return 1;
		}
	}
	else if (strcmp(argv[1], "-users") == 0) {

		/*Get all users from all streams*/
		clrstr(query);
		strcat(query, "select * from Users");

		if (mysql_query(&mysql, query)){
			printf("Could not get users\n");
			return 1;
		}

		if (!(res = mysql_store_result(&mysql))){
			printf("Could not get users\n");
			return 1;
		}

		while ((row = mysql_fetch_row(res))) {

			printf("%s\n", row[0]);
		}
	}
	else if (strcmp(argv[1], "-help") == 0) {

		printf("-clear : Removes all of the posts, users, streams and any other information from the tables in the database\n");
		printf("-reset : Deletes the tables from the database\n");
		printf("-posts : Prints out all posts stored in the database\n");
		printf("-users : Prints out all user names stored in the database\n");
		printf("-streams : Prints out all stream names stored in the database\n");
		printf("\nCustom Tags - These tags should only be used by the program, not manually.\n\n");
		printf("-addAuthor \"<stream(s)>\" \"<username>\": Adds a user to a single or multiple streams\n");
		printf("-removeAuthor \"<stream(s)>\" \"<username>\": Removes a user to a single or multiple streams\n");
		printf("-addPost \"<stream>\" \"<username>\" \"<date>\" \"<text>\": Adds a post to a stream\n");
		printf("-getStreams \"<username>\" : Returns a list of streams that the specified user has access to\n");
		printf("-HTMLStreams \"<username>\" : Returns a list of streams that the specified user has access to in the form of radio buttons (grouped within a form)\n");
		printf("-getPost \"<username>\" \"<stream>\" \"<currentPostNum>\" \"<toggleMode>\" \"<totalPostCount>\" \"<timeStamp>\" \"<pageMode>\": Returns a post based of the current settings of the stream being viewed. Running this tag will mark the post as read so this tag should only be run by the program and not manually.\n");
		printf("-getNumOfPosts \"<stream>\" \"<username>\" \"<timeStamp>\" \"<toggleMode>\" \"<pageMode>\": Returns the total number of posts in the stream (or all streams combined) based on the time stamp.\n");
		printf("-markRead \"<stream>\" \"<username>\" \"<lastPostRead>\" \"<pageMode>\" \"<toggleMode>\" \"<maxCount>\": Marks a post as read.\n");
		printf("-markAll \"<stream>\" \"<username>\" \"<totalPostCount>\" \"<viewingMode>\": Marks all posts in the stream as read.\n");
		printf("\n");
	}

	if (res != NULL) {

		mysql_free_result(res);
	}

	mysql_close(&mysql);
    mysql_library_end();

	return 0;
}