/* SPDX-FileCopyrightText: 2026 Jiffly */
/* SPDX-License-Identifier: MIT OR Apache-2.0 */

#ifndef MEOW_BASE_H
#define MEOW_BASE_H

#include <stddef.h>
#include <stdint.h>

#define MEOW_ABI_REVISION UINT32_C(1)
#define MEOW_EXPORT __attribute__((visibility("default")))
#define MEOW_CALL

typedef int32_t meow_status_t;
typedef uint64_t meow_handle_t;

#define MEOW_INVALID_HANDLE UINT64_C(0)
#define MEOW_OK INT32_C(0)
#define MEOW_ERROR_INVALID_ARGUMENT (-INT32_C(1))
#define MEOW_ERROR_INCOMPATIBLE_ABI (-INT32_C(2))
#define MEOW_ERROR_BUFFER_TOO_SMALL (-INT32_C(3))
#define MEOW_ERROR_NOT_FOUND (-INT32_C(4))
#define MEOW_ERROR_INVALID_STATE (-INT32_C(5))
#define MEOW_ERROR_INVALID_HANDLE (-INT32_C(6))
#define MEOW_ERROR_BUSY (-INT32_C(7))
#define MEOW_ERROR_TIMEOUT (-INT32_C(8))
#define MEOW_ERROR_INTERNAL (-INT32_C(9))

typedef struct meow_header {
    uint32_t struct_size;
    uint32_t abi_revision;
} meow_header_t;

typedef struct meow_bytes {
    const uint8_t *data;
    size_t size;
} meow_bytes_t;

typedef struct meow_buffer {
    uint8_t *data;
    size_t capacity;
} meow_buffer_t;

typedef struct meow_error {
    meow_buffer_t buffer;
    size_t size;
    size_t required;
} meow_error_t;

#endif
