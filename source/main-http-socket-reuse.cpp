#include "select-demo.h"

#if DEMO == DEMO_HTTP_SOCKET_REUSE

#include "mbed.h"
#include "network-helper.h"
#include "http_request.h"

void dump_response(HttpResponse* res) {
    printf("Status: %d - %s\n", res->get_status_code(), res->get_status_message().c_str());

    printf("Headers:\n");
    for (size_t ix = 0; ix < res->get_headers_length(); ix++) {
        printf("\t%s: %s\n", res->get_headers_fields()[ix]->c_str(), res->get_headers_values()[ix]->c_str());
    }
    printf("\nBody (%d bytes):\n\n%s\n", res->get_body_length(), res->get_body_as_string().c_str());
}

int main() {
    NetworkInterface* network = connect_to_default_network_interface();
    if (!network) {
        printf("Cannot connect to the network, see serial output\n");
        return 1;
    }

    // Create a TCP socket
    printf("\n----- Setting up TCP connection -----\n");

    TCPSocket* socket = new TCPSocket();
    nsapi_error_t open_result = socket->open(network);
    if (open_result != 0) {
        printf("Opening TCPSocket failed... %d\n", open_result);
        return 1;
    }

    nsapi_error_t connect_result = socket->connect("httpbin.org", 80);
    if (connect_result != 0) {
        printf("Connecting over TCPSocket failed... %d\n", connect_result);
        return 1;
    }

    printf("Connected over TCP to httpbin.org:80\n");

    // Do a GET request to httpbin.org
    {
        HttpRequest* get_req = new HttpRequest(socket, HTTP_GET, "http://httpbin.org/status/418");

        // By default the body is automatically parsed and stored in a string, this is memory heavy.
        // To receive chunked response, pass in a callback as third parameter to 'send'.
        HttpResponse* get_res = get_req->send();
        if (!get_res) {
            printf("HttpRequest failed (error code %d)\n", get_req->get_error());
            return 1;
        }

        printf("\n----- HTTP GET response -----\n");
        dump_response(get_res);

        delete get_req;
    }

    // POST request to httpbin.org
    {
        HttpRequest* post_req = new HttpRequest(socket, HTTP_POST, "http://httpbin.org/post");
        post_req->set_header("Content-Type", "application/json");

        const char body[] = "{\"hello\":\"world\"}";

        HttpResponse* post_res = post_req->send(body, strlen(body));
        if (!post_res) {
            printf("HttpRequest failed (error code %d)\n", post_req->get_error());
            return 1;
        }

        printf("\n----- HTTP POST response -----\n");
        dump_response(post_res);

        delete post_req;
    }

    socket->close();
    delete socket;

    wait(osWaitForever);
}

#endif
