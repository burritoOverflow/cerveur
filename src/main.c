#include <errno.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#include "HTTP_Server.h"
#include "Response.h"
#include "Routes.h"

#define MSG_SIZE 4096
#define PORT 6969

int main() {
  // initiate HTTP_Server
  HTTP_Server http_server;
  init_server(&http_server, PORT);
  int client_socket;

  // registering Routes
  struct Route *route = addRoute(NULL, "/", "index.html");
  addRoute(route, "/about", "about.html");

  // display all available routes
  printf("Available Routes:");
  traverseRoutes(route);

  while (1) {
    // i.e the Method Headers and Path
    char client_request[MSG_SIZE] = "";

    client_socket = accept(http_server.socket, NULL, NULL);

    read(client_socket, client_request, MSG_SIZE - 1);
    printf("Client message: %s\n", client_request);

    // parsing client socket header to get HTTP method, route
    char *method = "";
    char *urlRoute = "";

    char *client_http_header = strtok(client_request, "\n");
    char *header_token = strtok(client_http_header, " ");
    int header_parse_counter = 0;

    // ignore the last token as it's the HTTP Method
    while (header_token != NULL && header_parse_counter < 2) {
      printf("Header parse counter: %d\nHeader Token: %s\n",
             header_parse_counter, header_token);
      switch (header_parse_counter) {
      case 0:
        method = header_token;
      case 1:
        urlRoute = header_token;
      }

      header_token = strtok(NULL, " ");
      header_parse_counter++;
    }

    printf("The method is %s\n", method);
    printf("The route is %s\n", urlRoute);

    char template[100] = "";
    if (strcmp(method, "GET") != 0) {
      printf("Method: is not GET - got method: %s\n", method);
      // method not supported, so let's Bad Request here
      const char response_content[] = "Method Not Allowed";
      char http_response_content[MSG_SIZE] =
          "HTTP/1.1 405 Method Not Allowed\r\n\r\n";
      strcat(http_response_content, response_content);
      strcat(http_response_content, "\r\n\r\n");
      send(client_socket, http_response_content, sizeof(http_response_content),
           0);
    } else { // GET request, allowed
      if (strstr(urlRoute, "/static/") != NULL) {
        strcat(template, "static/index.css");
      } else {
        struct Route *destination = search(route, urlRoute);
        strcat(template, "templates/");

        if (destination == NULL) {
          strcat(template, "404.html");
        } else {
          strcat(template, destination->value);
        }
      }

      char *response_data = render_static_file(template);

      char http_header[4096] = "HTTP/1.1 200 OK\r\n\r\n";

      strcat(http_header, response_data);
      strcat(http_header, "\r\n\r\n");

      send(client_socket, http_header, sizeof(http_header), 0);
      close(client_socket);
      free(response_data);
    }
  }

  return 0;
}
