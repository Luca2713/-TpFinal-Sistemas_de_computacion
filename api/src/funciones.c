#include <stdio.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <ulfius.h>
#include <jansson.h>
#include <unistd.h>

/**
 * Callback function for the web application on /helloworld url call
 */

static int gpio(const struct _u_request *request,
                struct _u_response *response, void *user_data)
{
    (void)request;
    (void)user_data;

    int fd;
    char path[1024];
    char readBuf[128];

    json_t *json_request = ulfius_get_json_body_request(request, NULL);

    if (json_request == NULL)
    {
        long long status = 500;
        json_int_t *j_status = &status;

        json_t *json_body = json_object();
        json_object_set_new(json_body, "code", json_integer(*j_status));
        json_object_set_new(json_body, "description",
                            json_string("No se ingreso datos"));

        ulfius_set_json_body_response(response, (uint)status, json_body);
        json_decref(json_body);
        json_decref(json_request);

        return 1;
    }

    int gpio = (int)json_integer_value(json_object_get(json_request, "in"));

    snprintf(path, sizeof(path), "/dev/raspiGpio%d", gpio);
    fd = open(path, O_RDWR);

    if (read(fd, readBuf, 128) == -1)
    {
        perror("write, set pin input");
        return U_CALLBACK_ERROR;
    }

    long long valor = atoll(readBuf);

    json_t *json_body = NULL;

    json_int_t *out = &valor;

    long long status = 200;
    json_int_t *j_status = &status;

    json_body = json_object();
    json_object_set_new(json_body, "code", json_integer(*j_status));
    json_object_set_new(json_body, "out", json_integer(*out));

    ulfius_add_header_to_response(response, ".json", "application/json");
    ulfius_set_json_body_response(response, (uint)status, json_body);
    json_decref(json_body);

    close(fd);

    return U_CALLBACK_CONTINUE;
}

int no_encontrado(const struct _u_request *request,
                  struct _u_response *response, void *user_data)
{
    (void)request;
    (void)user_data;

    json_t *json_body = NULL;

    long long status = 404;
    json_int_t *j_status = &status;

    json_body = json_object();
    json_object_set_new(json_body, "code", json_integer(*j_status));
    json_object_set_new(json_body, "description", json_string("NOT FOUND"));

    ulfius_set_json_body_response(response, (uint)status, json_body);
    json_decref(json_body);

    return U_CALLBACK_CONTINUE;
}
