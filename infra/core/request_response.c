// Request ----------------------------------------------------------------------------------------------------

struct Request {
    struct String *method;
    struct String *path;
    struct String *contentType;
    struct String *cookies;
    struct String *body;
};

void request_free(struct Request *request) {
    string_free(request->method);
    string_free(request->path);
    string_free(request->contentType);
    string_free(request->cookies);
    string_free(request->body);
    free(request);
}

struct Request* request_new(struct String *method, struct String *path, struct String *contentType, struct String *cookies, struct String *body) {
    struct Request *_request = malloc(sizeof(struct Request));
    _request->method = method;
    _request->path = path;
    _request->contentType = contentType;
    _request->cookies = cookies;
    _request->body = body;

    return _request;
}

// Response ----------------------------------------------------------------------------------------------------

struct Response {
    struct String *statusCode;
    struct String *redirectUri;
    struct String *body;
};

void response_free(struct Response *response) {
    string_free(response->statusCode);
    string_free(response->redirectUri);
    string_free(response->body);
    free(response);
}

struct Response* response_new(struct String *statusCode, struct String *redirectUri, struct String *body) {
    struct Response *_response = malloc(sizeof(struct Response));
    _response->statusCode = statusCode;
    _response->redirectUri = redirectUri;
    _response->body = body;

    return _response;
}

struct Response* response_ok(struct String *body) {
    struct Response *_response = malloc(sizeof(struct Response));
    _response->statusCode = string_new("200 OK");
    _response->redirectUri = string_new("");
    _response->body = body;

    return _response;
}

struct Response* response_not_found() {
    struct Response *_response = malloc(sizeof(struct Response));
    _response->statusCode = string_new("404 Not Found");
    _response->redirectUri = string_new("");
    _response->body = string_new("");

    return _response;
}

struct Response* response_redirect(struct String *redirectUri) {
    struct Response *_response = malloc(sizeof(struct Response));
    _response->statusCode = string_new("302");
    _response->redirectUri = redirectUri;
    _response->body = string_new("");

    return _response;
}