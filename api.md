**API, what is it made of and how it works**

I will use the Libmicrohttpd library for C a request parsing engine that sits on top of raw socket.

Part 1 - Socket Layer: An HTTP request is just a text that needs to be sent over TCP connection. The way it works:
1. Open socket
2. Bind to a port
3. listen
4. accept incoming connection
5. read raw bytes data until it get full HTTP request (header, body, request line, etc..)

MHD_start_daemon from the library does that all for us.

Part 2 - Callback model: Now what do we do when there is no request? Instead of having loop that waits for connections, we give libmicrohttpd a pointer
function, and calls the function whenever theres work to do. So the library itself owns the loop, we just react to it. 
The critical detail: libmicrohttpd calls your callback more than once per single HTTP request. Once when headers are parsed. Then if there is body(POST/PUT) it gets called again,it can be several times until full body is processed. We have the upload_data_size where we check when its 0 meaning no more data to process. Then we need to stash the per-connection state across these repeated calls. We cant rely on local variables persisting these states, they can get lost or truncated.

Part 3 - Routing: Turning URL into a decision, since there is no router object in C we need to handle that. Routing is url string inspection. We get raw const char *url and *method, and we decide what to do by comparing them.

Part 4 - Method dispatch: Smae route, different verbs: Once we know the URL matches a resource for example /users, the HTTP method decides the action: GET, POST etc..

Part 5 - Handler Functions: handler functions are getting us what we want or what to do when we get request. they never touch the MHD_Connection directly, they take strings and return the struct HTTP_response, a body string + status code. This decoupling matters because the handler itself doesn't know about HTTP, or that it is involved.

Part 6 - Data layer turning rows into JSON: Handlers for persistance layer, in other words storing and interacting with data. This library has libpq which is postgres library for c. It takes a query string and executes it and manually converts the result into JSON. 
     * Serialization: a database result is a grid of rows/columns in memory; JSON is text. Someone has to walk that grid and build the string by hand (or with a library) — nothing does this automatically in C.
     * Query construction: this reference project builds SQL by directly inserting strings into a query with snprintf — that's a classic SQL injection hole. Worth knowing why that's dangerous (a malicious id/body value can change the query's meaning) even before you fix it, since parameterized queries are the standard answer.
Part 7 - Building HTTP response: Once handler returns body plus status something has to turn that back into real HTTP bytes: set the Content-Type header, set CORS headers if needed, wrap the body in an MHD_Response object, and queue it with the connection. MHD_response has different memory mode, since C makes us manage memory ourselfs, we need to know who owns the JSONs string memory and when its safe to free it.

Part 8 - Status codesa as real API Contract: The codes are basically communicating semantic language to us with 200, 500, 401 etc..instead of just throwing back plain text.

Part 9 - Error handling for flow control: Since C has no extentions we use setjmp and longjmp to fake try and catch.