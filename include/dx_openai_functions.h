#pragma once

typedef struct
{
    const char *assistant_msg;
    const char *user_msg;
    float temperature;
    int max_tokens;
    void (*openai_function_handler)(const char *content, const char *finish_reason, const char *function_call_name, const char *function_call_arguments);
    void *json_root;
    struct curl_slist *headers;
} DX_OPENAI_FUNCTION_CTX;

void dx_openai_function_free(DX_OPENAI_FUNCTION_CTX *ctx);
void dx_openai_function_init(DX_OPENAI_FUNCTION_CTX *ctx, const char *filename, const char *openai_api_key);
void dx_openai_function_post_request(DX_OPENAI_FUNCTION_CTX *ctx);