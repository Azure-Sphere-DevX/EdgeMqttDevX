/* Copyright (c) Microsoft Corporation. All rights reserved.
   Licensed under the MIT License. */

#include "dx_async.h"
#include "dx_terminate.h"
#include <stdbool.h>

// http://docs.libuv.org/en/v1.x/async.html

void dx_asyncInit(DX_ASYNC_BINDING *binding)
{
	if (uv_async_init(uv_default_loop(), &binding->async, binding->handler) < 0)
	{
		printf("uv_async_init failed for %s\n", binding->name);
		dx_terminate(DX_ExitCode_Async_Init_Failed);
	}
}

void dx_asyncSend(DX_ASYNC_BINDING *binding, void *data)
{
	binding->async.data = data;
	if (uv_async_send(&binding->async) < 0)
	{
		printf("uv_async_send failed for %s\n", binding->name);
		dx_terminate(DX_ExitCode_Async_Send_Failed);
	}
}

void dx_asyncSetInit(DX_ASYNC_BINDING *asyncSet[], size_t asyncCount)
{
	for (int i = 0; i < asyncCount; i++)
	{
		dx_asyncInit(asyncSet[i]);
	}
}
