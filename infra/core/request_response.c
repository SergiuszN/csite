// Request ----------------------------------------------------------------------------------------------------

typedef struct Request {
    String *method;
    String *path;
    String *contentType;
    String *cookies;
    String *body;
} Request;

void request_free(Request *request) {
    string_free(request->method);
    string_free(request->path);
    string_free(request->contentType);
    string_free(request->cookies);
    string_free(request->body);
    free(request);
}

Request *request_new(String *method, String *path, String *contentType, String *cookies, String *body) {
    Request *_request = malloc(sizeof(Request));
    _request->method = method;
    _request->path = path;
    _request->contentType = contentType;
    _request->cookies = cookies;
    _request->body = body;

    return _request;
}

// Response ----------------------------------------------------------------------------------------------------

typedef struct Response {
    String *statusCode;
    String *redirectUri;
    String *body;
} Response;

void response_free(Response *response) {
    string_free(response->statusCode);
    string_free(response->redirectUri);
    string_free(response->body);
    free(response);
}

Response *response_new(String *statusCode, String *redirectUri, String *body) {
    Response *_response = malloc(sizeof(Response));
    _response->statusCode = statusCode;
    _response->redirectUri = redirectUri;
    _response->body = body;

    return _response;
}

Response *response_ok(String *body) {
    Response *_response = malloc(sizeof(Response));
    _response->statusCode = string_new("200 OK");
    _response->redirectUri = string_new("");
    _response->body = body;

    return _response;
}

Response *response_not_found() {
    Response *_response = malloc(sizeof(Response));
    _response->statusCode = string_new("404 Not Found");
    _response->redirectUri = string_new("");
    _response->body = string_new("");

    return _response;
}

Response *response_redirect(String *redirectUri) {
    Response *_response = malloc(sizeof(Response));
    _response->statusCode = string_new("302");
    _response->redirectUri = redirectUri;
    _response->body = string_new("");

    return _response;
}
