# HTTP/HTTPS example for Mbed OS 5

This application demonstrates how to make HTTP and HTTPS requests and parse the response from Mbed OS 5.

It consists of six demo's, which you can select in ``source/select-demo.h``.

* HTTP demo:
    * Does a GET request to http://httpbin.org/status/418.
    * Does a POST request to http://httpbin.org/post.
* HTTPS demo:
    * Does a GET request to https://os.mbed.com/media/uploads/mbed_official/hello.txt.
    * Does a POST request to https://httpbin.org/post.
* HTTP demo with socket re-use.
* HTTPS demo with socket re-use.
* HTTP demo over IPv6.
* HTTPS demo with chunked requests.

Response parsing is done through [nodejs/http-parser](https://github.com/nodejs/http-parser).

**Note:** HTTPS requests do not work on targets with less than 128K of RAM due to the size of the TLS handshake. For more background see [mbed-http](https://developer.mbed.org/teams/sandbox/code/mbed-http).

## To build

1. If you're using WiFi, specify the credentials in `mbed_app.json`.
1. Build the project in the online compiler or using Mbed CLI.
1. Flash the project to your development board.
1. Attach a serial monitor to your board to see the debug messages.

## Defining the network interface

This application uses the on-board network interface for your board. If you use an external network interface (f.e. a WiFi module) you need to add the driver to this project. Then, open `network-helper.h` and specify which network driver to use.

More information is in the Mbed OS documentation under [IP Networking](https://os.mbed.com/docs/latest/reference/ip-networking.html).

## Entropy (or lack thereof)

On all platforms that do not have the `TRNG` feature, the application is compiled without TLS entropy sources. This means that your code is inherently unsafe and should not be deployed to any production systems. To enable entropy, remove the `MBEDTLS_NO_DEFAULT_ENTROPY_SOURCES` and `MBEDTLS_TEST_NULL_ENTROPY` macros from mbed_app.json.

## Flash size

Default flash size for HTTPS is very large, as the application is loading the default Mbed TLS configuration. To use a more optimized version, you can disable unused cypher suites and other Mbed TLS features with a custom configuration file. Create a new configuration file, then add in `mbed_app.json`:

```
"MBEDTLS_CONFIG_FILE=\"mbedtls_config.h\""
```

to the `macros` array.

## Running tests

You can run the integration tests from this project via Mbed CLI.

1. In `select-demo.h` set the `DEMO` macro to `DEMO_TESTS`.
1. Set your WiFi credentials in `mbed_app.json`.
1. Then run the tests via:

```
$ mbed test -v -n mbed-http-tests-tests-*
```

## Tested on

* K64F with Ethernet.
* NUCLEO_F411RE with ESP8266.
* ODIN-W2 with WiFi.
* K64F with Atmel 6LoWPAN shield.
* DISCO-L475VG-IOT01A with WiFi (requires the [wifi-ism43362](https://github.com/ARMmbed/wifi-ism43362/) driver).
* [Mbed Simulator](https://github.com/janjongboom/mbed-simulator).
