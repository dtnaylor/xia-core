/* ts=4 */
/*
** Copyright 2011 Carnegie Mellon University
**
** Licensed under the Apache License, Version 2.0 (the "License");
** you may not use this file except in compliance with the License.
** You may obtain a copy of the License at
**
**    http://www.apache.org/licenses/LICENSE-2.0
**
** Unless required by applicable law or agreed to in writing, software
** distributed under the License is distributed on an "AS IS" BASIS,
** WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
** See the License for the specific language governing permissions and
** limitations under the License.
*/

/* xtods - a stupidly simple gettimeofday server that returns the time in a string the result using localtime */

#include <time.h>
#include "Xsocket.h"
#include "dagaddr.hpp"

#define SID0 "SID:0f00000000000000000000000000000123456789"
#define DAG  "RE %s %s %s"
#define SNAME "tod_s.testbed.xia"

int main()
{
    int sock;
    size_t len;
    char buf[XIA_MAXBUF];
	sockaddr_x client;
	time_t now;
	struct tm *t;

    // create a datagram socket
    if ((sock = Xsocket(AF_XIA, SOCK_DGRAM, 0)) < 0) {
		printf("error: unable to create the listening socket.\n");
		exit(1);
	}

	struct addrinfo *ai;
	if (Xgetaddrinfo(NULL, SID0, NULL, &ai) != 0) {
    	printf("error: unable to create source dag.");
		exit(1);
	}

	sockaddr_x *sa = (sockaddr_x*)ai->ai_addr;

    //Register this service name to the name server
    if (XregisterName(SNAME, sa) < 0) {
    	printf("error: unable to register name/dag combo");
		exit(1);
	}

    // bind to the DAG
    if (Xbind(sock, (struct sockaddr*)sa, sizeof(sockaddr_x)) < 0) {
		Xclose(sock);
		printf("error: unable to bind to %s\n", SNAME);
		exit(1);
	}

    while (1) {
		len = sizeof(client);
		if (Xrecvfrom(sock, buf, sizeof(buf), 0, (struct sockaddr*)&client, &len) < 0) {
			printf("error receiving client request\n");
			// assume it's ok, and just keep listening
			continue;
		}

		// we don't care what the client said, so we'll just ignore it

		now = time(NULL);
		t = gmtime(&now);
		strftime(buf, sizeof(buf), "%c %Z", t);
		
		Graph g(&client);
		printf("request from:\n%s\n", g.dag_string().c_str());
			
		//Reply to client
		if (Xsendto(sock, buf, strlen(buf) + 1, 0, (struct sockaddr*)&client, sizeof(client)) < 0)
			printf("error sending time to the client\n");
    }

	Xclose(sock);
    return 0;
}

