#define _CRT_SECURE_NO_DEPRECATE

#include <hiredis/hiredis.h>
#include <iostream>
#include <fstream>

#include <time.h>
#include <thread>

using namespace std;
using std::thread;

void importData(unsigned char* &b, int &l) {
	ifstream is("grey.png", ifstream::binary);
	b = NULL;

	if (is.is_open()) {
		is.seekg(0, is.end);
		l = (int)is.tellg();
		is.seekg(0, is.beg);

		b = (unsigned char*)malloc(l);

		is.read((char*)b, l);
		is.close();

	}

}


void importJson(string &j, string file) {

	ifstream ist(file);
	if (!ist.is_open()) {
		return;
	}

	for (char p; ist >> p;)
		j += p;
	if (ist.is_open()) {
		ist.close();
	}
}

void connectRedis(redisContext* c) {


	if (c == NULL || c->err) {
		if (c) {
			printf("Connecting Error: %s\n", c->errstr);
		}
		else {
			printf("Can't allocate redis context\n");
		}
		return;
	}
}

void setData(redisReply* reply, redisContext* c, unsigned char* buffer, int length, string jsonMsg) {
	reply = (redisReply*)redisCommand(c, "HMSET %s %s %s %s %s %s %b", "cam1", "index", "123", "json", jsonMsg.c_str(), "image", buffer, (size_t)length);
	//reply = (redisReply*)redisCommand(c, "HMSET %s %s %s %s %s %s %b", "cam2", "index", "456", "json", jsonMsg.c_str(), "image", buffer, (size_t)length);
	if (reply == NULL) {
		printf("redisCommand reply is NULL\n");
		return;
	}
	if (reply->type == REDIS_REPLY_ERROR) {
		printf("Command Error: %s\n", reply->str);
		freeReplyObject(reply);
		return;
	}
	printf("Command reply: %s\n", reply->str);


}


void getData(redisReply* reply, redisContext* c) {

	//reply = (redisReply*)redisCommand(c, "HMGET %s %s %s %s", "cam1", "index", "json", "image");
	reply = (redisReply*)redisCommand(c, "hvals cam1");
	if (reply == NULL) {
		printf("[redis] redisCommand reply is NULL: %s", c->errstr);
		return;
	}
	if (reply->type == REDIS_REPLY_ERROR) {
		printf("[redis] command error : %s", reply->str);
		freeReplyObject(reply);
		return;
	}
	printf("[redis] HMSET result => %s", reply->str);


	for (int i = 0; i < reply->elements; i++) {
		printf("[redis] %s", reply->element[i]->str);
	}

}

void setImg(redisReply* reply, redisContext* c, unsigned char* buffer, int length) {
	reply = (redisReply*)redisCommand(c, "SET image %b", buffer, (size_t)length);
	//reply = (redisReply*)redisCommand(c, "SET %d %b", 11, fp);
	if (reply == NULL) {
		printf("redisCommand reply is NULL: %s\n", c->errstr);
		return ;
	}
	if (reply->type == REDIS_REPLY_ERROR) {
		printf("Command Error: %s\n", reply->str);
		freeReplyObject(reply);
		return ;
	}
	printf("Command reply: %s\n", reply->str);
	freeReplyObject(reply);

}


void getImg(redisReply* reply, redisContext* c) {

	reply = (redisReply*)redisCommand(c, "GET image");

	if (reply == NULL) {
		printf("redisCommand reply is NULL\n");
		return;
	}
	if (reply->type == REDIS_REPLY_ERROR) {
		printf("Command Error: %s\n", reply->str);
		freeReplyObject(reply);
		return;
	}
	printf("Command reply: %s\n", reply->str);
}



int main(int argc, char** argv) {

	unsigned char* buffer = NULL;
	int length = 0;
	string jsonMsg;
	string path = "adjust.json";


	importData(buffer, length);	
	importJson(jsonMsg, path);



	clock_t  start, end;
	double  result;


	start = clock();

	redisReply* reply=NULL;
	redisContext* c = redisConnect("127.0.0.1", 6379);
	connectRedis(c);

	setData(reply, c, buffer, length, jsonMsg);

	getData(reply, c);

	
	//reply = (redisReply*)redisCommand(c, "HMSET %s %s %s %s %s %s %b", "cam1", "index", "123", "json", jsonMsg.c_str(), "image", buffer, (size_t)length);
	////reply = (redisReply*)redisCommand(c, "HMSET %s %s %s %s %s %s %b", "cam2", "index", "456", "json", jsonMsg.c_str(), "image", buffer, (size_t)length);
	//if (reply == NULL) {
	//	printf("redisCommand reply is NULL\n");
	//	return -1;
	//}
	//if (reply->type == REDIS_REPLY_ERROR) {
	//	printf("Command Error: %s\n", reply->str);
	//	freeReplyObject(reply);
	//	return -1;
	//}
	//printf("Command reply: %s\n", reply->str);
	//	

	////reply = (redisReply*)redisCommand(c, "HMGET %s %s %s %s", "cam1", "index", "json", "image");
	//reply = (redisReply*)redisCommand(c, "hvals cam1");
	//if (reply == NULL) {
	//	printf("[redis] redisCommand reply is NULL: %s", c->errstr);
	//	return 0;
	//}
	//if (reply->type == REDIS_REPLY_ERROR) {
	//	printf("[redis] command error : %s", reply->str);
	//	freeReplyObject(reply);
	//	return 0;
	//}
	//printf("[redis] HMSET result => %s", reply->str);


	//for (int i = 0; i < reply->elements; i++) {
	//	printf("[redis] %s", reply->element[i]->str);
	//}


	freeReplyObject(reply);


	end = clock();
	result = (double)(end - start);
	cout << "calc time : " << result << endl;


	//ofstream out;
	//out.open("test.json");
	//out.write(reply->str, reply->len);
	//out.close();


	//ofstream out;
	//out.open("grey_test.jpg", ofstream::binary);
	//out.write(reply->str, reply->len);
	//out.close();


	//freeReplyObject(reply);

	//// GET XXX
	//reply = (redisReply*)redisCommand(c, "GET XXX");
	//if (reply == NULL) {
	//    printf("redisCommand reply is NULL\n");
	//    return -1;
	//}
	//if (reply->type == REDIS_REPLY_ERROR) {
	//    printf("Command Error: %s\n", reply->str);
	//    freeReplyObject(reply);
	//    return -1;
	//}
	//printf("Command reply: %s\n", reply->str);
	//freeReplyObject(reply);

	redisFree(c);


}


