/**
 * This is an example of doing chunked requests, where you do not need to load the full request body
 * into memory. You do this by adding a callback to the `send` function of the HTTP/HTTPS request.
 */

#include "select-demo.h"

#if DEMO == DEMO_HTTPS_CHUNKED_REQUEST

#include "mbed.h"
#include "mbed_trace.h"
#include "https_request.h"
#include "network-helper.h"

/* List of trusted root CA certificates
 * currently one: Comodo, the CA for reqres.in
 *
 * To add more root certificates, just concatenate them.
 */
const char SSL_CA_PEM[] = "-----BEGIN CERTIFICATE-----\n"
    "MIICiTCCAg+gAwIBAgIQH0evqmIAcFBUTAGem2OZKjAKBggqhkjOPQQDAzCBhTEL\n"
    "MAkGA1UEBhMCR0IxGzAZBgNVBAgTEkdyZWF0ZXIgTWFuY2hlc3RlcjEQMA4GA1UE\n"
    "BxMHU2FsZm9yZDEaMBgGA1UEChMRQ09NT0RPIENBIExpbWl0ZWQxKzApBgNVBAMT\n"
    "IkNPTU9ETyBFQ0MgQ2VydGlmaWNhdGlvbiBBdXRob3JpdHkwHhcNMDgwMzA2MDAw\n"
    "MDAwWhcNMzgwMTE4MjM1OTU5WjCBhTELMAkGA1UEBhMCR0IxGzAZBgNVBAgTEkdy\n"
    "ZWF0ZXIgTWFuY2hlc3RlcjEQMA4GA1UEBxMHU2FsZm9yZDEaMBgGA1UEChMRQ09N\n"
    "T0RPIENBIExpbWl0ZWQxKzApBgNVBAMTIkNPTU9ETyBFQ0MgQ2VydGlmaWNhdGlv\n"
    "biBBdXRob3JpdHkwdjAQBgcqhkjOPQIBBgUrgQQAIgNiAAQDR3svdcmCFYX7deSR\n"
    "FtSrYpn1PlILBs5BAH+X4QokPB0BBO490o0JlwzgdeT6+3eKKvUDYEs2ixYjFq0J\n"
    "cfRK9ChQtP6IHG4/bC8vCVlbpVsLM5niwz2J+Wos77LTBumjQjBAMB0GA1UdDgQW\n"
    "BBR1cacZSBm8nZ3qQUfflMRId5nTeTAOBgNVHQ8BAf8EBAMCAQYwDwYDVR0TAQH/\n"
    "BAUwAwEB/zAKBggqhkjOPQQDAwNoADBlAjEA7wNbeqy3eApyt4jf/7VGFAkK+qDm\n"
    "fQjGGoe9GKhzvSbKYAydzpmfz1wPMOG+FDHqAjAU9JM8SaczepBGR7NjfRObTrdv\n"
    "GDeAU/7dIOA1mjbRxwG55tzd8/8dLDoWV9mSOdY=\n"
    "-----END CERTIFICATE-----\n";

void dump_response(HttpResponse* res) {
    printf("Status: %d - %s\n", res->get_status_code(), res->get_status_message().c_str());

    printf("Headers:\n");
    for (size_t ix = 0; ix < res->get_headers_length(); ix++) {
        printf("\t%s: %s\n", res->get_headers_fields()[ix]->c_str(), res->get_headers_values()[ix]->c_str());
    }
    printf("\nBody (%lu bytes):\n\n%s\n", res->get_body_length(), res->get_body_as_string().c_str());
}

// Spread the message out over 3 different chunks
const char * chunks[] = {
    "{\"message\":",
    "\"this is an example",
    " of chunked encoding\"}"
};

int chunk_ix = 0;

// Callback function, grab the next chunk and return it
const void * get_chunk(uint32_t* out_size) {
    // If you don't have any data left, set out_size to 0 and return a null pointer
    if (chunk_ix == (sizeof(chunks) / sizeof(chunks[0]))) {
        *out_size = 0;
        return NULL;
    }
    const char *chunk = chunks[chunk_ix];
    *out_size = strlen(chunk);
    chunk_ix++;

    return chunk;
}

int main() {
    NetworkInterface* network = connect_to_default_network_interface();
    if (!network) {
        printf("Cannot connect to the network, see serial output\n");
        return 1;
    }

    mbed_trace_init();

    // This example also logs the raw request, you can do this by calling 'set_request_log_buffer' on the request
    uint8_t *request_buffer = (uint8_t*)calloc(2048, 1);

    // POST request to reqres.in
    {
        HttpsRequest* post_req = new HttpsRequest(network, SSL_CA_PEM, HTTP_POST, "https://reqres.in/api/users");
        post_req->set_header("Content-Type", "application/json");
        post_req->set_request_log_buffer(request_buffer, 2048);

        // If you pass a callback here, the Transfer-Encoding header is automatically set to chunked
        HttpResponse* post_res = post_req->send(&get_chunk);
        if (!post_res) {
            printf("HttpsRequest failed (error code %d)\n", post_req->get_error());
            return 1;
        }

        // Log the raw request that went over the line (if you decode the hex you can see the chunked parts)
        // e.g. in Node.js (take the output from below):
        // '50 4f 53 54 20'.split(' ').map(c=>parseInt(c,16)).map(c=>String.fromCharCode(c)).join('')
        printf("\n----- Request buffer -----\n");
        for (size_t ix = 0; ix < post_req->get_request_log_buffer_length(); ix++) {
            printf("%02x ", request_buffer[ix]);
        }
        printf("\n");

        printf("\n----- HTTPS POST response -----\n");
        dump_response(post_res);

        delete post_req;
    }

    wait(osWaitForever);
}

#endif
