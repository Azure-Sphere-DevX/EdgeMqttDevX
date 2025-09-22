/* Copyright (c) Microsoft Corporation. All rights reserved.
   Licensed under the MIT License. */

#include "dx_openai_functions.h"
#include "parson.h"
#include "string.h"
#include <curl/curl.h>
#include <curl/easy.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static const char *openai_endpoint = "https://api.openai.com/v1/chat/completions";

// Curl stuff.
struct MemoryStruct
{
    char *memory;
    size_t size;
};

static size_t WriteMemoryCallback(void *contents, size_t size, size_t nmemb, void *userp)
{
    size_t realsize = size * nmemb;
    struct MemoryStruct *mem = (struct MemoryStruct *)userp;

    char *ptr = realloc(mem->memory, mem->size + realsize + 1);
    if (!ptr)
    {
        /* out of memory! */
        printf("not enough memory (realloc returned NULL)\n");
        return 0;
    }

    mem->memory = ptr;
    memcpy(&(mem->memory[mem->size]), contents, realsize);
    mem->size += realsize;
    mem->memory[mem->size] = 0;

    return realsize;
}

// https://curl.se/libcurl/c/getinmemory.html
static char *postHttpData(DX_OPENAI_FUNCTION_CTX *ctx, const char *url, long timeout, const char *postData)
{
    CURL *curl_handle;
    CURLcode res;
    static bool curl_initialized = false;
    struct curl_slist *headers = ctx->headers;

#if defined(DUMMY_OPENAI_FUNCTION_RESPONSE)
    const char dummy_response[] = "{\"id\":\"chatcmpl-7mfQxsDXGMqKuxb78oCmWrfZfncL0\",\"object\":\"chat.completion\",\"created\":1691833371,\"model\":\"gpt-3.5-turbo-0613\",\"choices\":[{\"index\":0,\"message\":{\"role\":\"assistant\",\"content\":null,\"function_call\":{\"name\":\"set_light_state\",\"arguments\":\"{\\n  \\\"device\\\": \\\"Lounge\\\",\\n  \\\"state\\\": \\\"on\\\",\\n  \\\"color\\\": \\\"blue\\\"\\n}\"}},\"finish_reason\":\"function_call\"}],\"usage\":{\"prompt_tokens\":376,\"completion_tokens\":31,\"total_tokens\":407}  }";
    int len = sizeof(dummy_response) + 1;

    char *response_copy = malloc(len);
    if (!response_copy)
    {
        return NULL;
    }

    memset(response_copy, 0, len);
    memcpy(response_copy, dummy_response, len);
    
    return response_copy;
#endif // DUMMY_OPENAI_FUNCTION_RESPONSE

    if (!curl_initialized)
    {
        curl_global_init(CURL_GLOBAL_ALL);
        curl_initialized = true;
    }

    struct MemoryStruct chunk;

    chunk.memory = malloc(1); /* will be grown as needed by the realloc above */
    chunk.size = 0;           /* no data at this point */

    /* init the curl session */
    curl_handle = curl_easy_init();

    curl_easy_setopt(curl_handle, CURLOPT_HTTPHEADER, headers);

    /* specify URL to post */
    curl_easy_setopt(curl_handle, CURLOPT_URL, url);

    /* use a CURLOPT_POSTFIELDS to fetch data */
    curl_easy_setopt(curl_handle, CURLOPT_POSTFIELDS, postData);

    /* Set timeout */
    curl_easy_setopt(curl_handle, CURLOPT_TIMEOUT, timeout);

    /* send all data to this function  */
    curl_easy_setopt(curl_handle, CURLOPT_WRITEFUNCTION, WriteMemoryCallback);

    /* we pass our 'chunk' struct to the openai_function_handler function */
    curl_easy_setopt(curl_handle, CURLOPT_WRITEDATA, (void *)&chunk);

    /* some servers do not like requests that are made without a user-agent
       field, so we provide one */
    curl_easy_setopt(curl_handle, CURLOPT_USERAGENT, "libcurl-agent/1.0");

    // based on the libcurl sample - https://curl.se/libcurl/c/https.html
    curl_easy_setopt(curl_handle, CURLOPT_SSL_VERIFYPEER, 0L);

    // https://curl.se/libcurl/c/CURLOPT_NOSIGNAL.html
    curl_easy_setopt(curl_handle, CURLOPT_NOSIGNAL, 1L);

    /* disable progress meter, set to 0L to enable it */
    curl_easy_setopt(curl_handle, CURLOPT_NOPROGRESS, 1L);

    // A long parameter set to 1 tells the library to fail the request if the HTTP code returned is equal to
    // or larger than 400
    curl_easy_setopt(curl_handle, CURLOPT_FAILONERROR, 1L);

    /* get it! */
    res = curl_easy_perform(curl_handle);

    curl_easy_cleanup(curl_handle);

    if (res == CURLE_OK)
    {
        // caller is responsible for freeing this.
        return chunk.memory;
    }

    free(chunk.memory);
    return NULL;
}

static void del_existing_msgs(JSON_Array *message_array, const char *key)
{
    size_t array_size; /* The size of the array */
    size_t i;          /* Loop index */

    if (message_array == NULL)
    {
        return;
    }

    array_size = json_array_get_count(message_array);

    for (int i = 0; i < array_size; i++)
    {
        /* Get the object at index i */
        JSON_Object *message_object = json_array_get_object(message_array, i);
        if (message_object == NULL)
        {
            continue;
        }

        const char *role = json_object_get_string(message_object, "role");
        if (role == NULL)
        {
            continue;
        }

        if (!strcmp(role, key))
        {
            json_array_remove(message_array, i);
            break;
        }
    }
}

static void parse_response(DX_OPENAI_FUNCTION_CTX *ctx, const char *response)
{
    JSON_Array *choices = NULL;
    JSON_Object *root_object;
    JSON_Value *root_value = NULL;

    const char *content = NULL;
    const char *finish_reason = NULL;
    const char *function_call_name = NULL;
    const char *function_call_arguments = NULL;

    root_value = json_parse_string(response);
    if (root_value == NULL)
    {
        printf("json_parse_string failed\n");
        goto cleanup;
    }

    root_object = json_value_get_object(root_value);
    if (root_object == NULL)
    {
        printf("json_value_get_object failed\n");
        goto cleanup;
    }

    choices = json_object_get_array(root_object, "choices");
    if (choices == NULL)
    {
        printf("json_object_get_array failed\n");
        goto cleanup;
    }

    JSON_Object *choice = json_array_get_object(choices, 0);
    if (choice == NULL)
    {
        goto cleanup;
    }

    content = json_object_dotget_string(choice, "message.content");
    finish_reason = json_object_get_string(choice, "finish_reason");
    function_call_name = json_object_dotget_string(choice, "message.function_call.name");
    function_call_arguments = json_object_dotget_string(choice, "message.function_call.arguments");

cleanup:

    if (ctx->openai_function_handler)
    {
        ctx->openai_function_handler(content, finish_reason, function_call_name, function_call_arguments);
    }

    if (root_value)
    {
        json_value_free(root_value);
    }
}

void dx_openai_function_post_request(DX_OPENAI_FUNCTION_CTX *ctx)
{
    JSON_Value *root_value = NULL;
    JSON_Value *new_value = NULL;
    JSON_Object *new_object = NULL;

    if (!ctx->json_root)
    {
        return;
    }

    if (!ctx->headers)
    {
        return;
    }

    // Get a JSON_Object pointer from the JSON_Value pointer
    JSON_Object *root_object = json_value_get_object(ctx->json_root);

    json_object_set_number(root_object, "temperature", ctx->temperature);
    json_object_set_number(root_object, "max_tokens", ctx->max_tokens);

    JSON_Array *message_array = json_object_get_array(root_object, "messages");
    if (message_array == NULL)
    {
        printf("No messages found\n");
    }

    // find existing role = user or assistant messages and delete
    del_existing_msgs(message_array, "user");
    del_existing_msgs(message_array, "assistant");

    if (ctx->user_msg)
    {
        // Create a new JSON value of type object
        new_value = json_value_init_object();
        new_object = json_value_get_object(new_value);
        // Set properties for the new object
        json_object_set_string(new_object, "role", "user");
        json_object_set_string(new_object, "content", ctx->user_msg);
        json_array_append_value(message_array, new_value);
    }

    if (ctx->assistant_msg)
    {
        new_value = json_value_init_object();
        new_object = json_value_get_object(new_value);

        json_object_set_string(new_object, "role", "assistant");
        json_object_set_string(new_object, "content", ctx->assistant_msg);
        json_array_append_value(message_array, new_value);
    }

    char *json = json_serialize_to_string(ctx->json_root);

    char *response = postHttpData(ctx, openai_endpoint, 5, json);

    json_free_serialized_string(json);

    parse_response(ctx, response);

    if (response)
    {
        free(response);
        response = NULL;
    }
}

void dx_openai_function_init(DX_OPENAI_FUNCTION_CTX *ctx, const char *filename, const char *openai_api_key)
{
    if (!ctx->json_root)
    {
        ctx->json_root = json_parse_file(filename);
    }

    if (!ctx->headers && openai_api_key)
    {
        char auth[128];
        snprintf(auth, sizeof(auth), "Authorization: Bearer %s", openai_api_key);

        ctx->headers = curl_slist_append(ctx->headers, auth);
        ctx->headers = curl_slist_append(ctx->headers, "Content-Type: application/json");
    }
}

void dx_openai_function_free(DX_OPENAI_FUNCTION_CTX *ctx)
{
    if (ctx->json_root)
    {
        json_value_free(ctx->json_root);
        ctx->json_root = NULL;
    }

    if (ctx->headers)
    {
        curl_slist_free_all(ctx->headers);
        ctx->headers = NULL;
    }
}
