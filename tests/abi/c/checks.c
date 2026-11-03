/* SPDX-FileCopyrightText: 2026 Jiffly */
/* SPDX-License-Identifier: MIT OR Apache-2.0 */

#include <meow/checks.h>
#include <assert.h>
#include <stddef.h>
#include <string.h>

int main(void) {
    meow_plugin_api_t api = {0};
    meow_plugin_entry_fn *entry = &meow_plugin_entry_v0;
    uint8_t short_header = 0;
    struct {
        meow_plugin_api_t api;
        uint64_t tail;
    } extended = {0};

    assert(meow_check_array(NULL, 0, 1) == MEOW_OK);
    assert(meow_check_array(NULL, 1, 1) == MEOW_ERROR_INVALID_ARGUMENT);
    assert(meow_check_array(&api, 1, 0) == MEOW_ERROR_INVALID_ARGUMENT);
    assert(meow_check_array(&api, SIZE_MAX, 2) == MEOW_ERROR_INVALID_ARGUMENT);

    assert(
        meow_check_header(&short_header, 1, sizeof(api), MEOW_ABI_REVISION) ==
        MEOW_ERROR_BUFFER_TOO_SMALL
    );
    assert(
        meow_check_header(NULL, sizeof(api), sizeof(api), MEOW_ABI_REVISION) ==
        MEOW_ERROR_INVALID_ARGUMENT
    );
    assert(
        meow_check_header(&api, sizeof(api), 0, MEOW_ABI_REVISION) == MEOW_ERROR_INVALID_ARGUMENT
    );
    assert(entry(MEOW_ABI_REVISION + 1, sizeof(api), &api, NULL) == MEOW_ERROR_INCOMPATIBLE_ABI);
    assert(entry(MEOW_ABI_REVISION, sizeof(api) - 1, &api, NULL) == MEOW_ERROR_BUFFER_TOO_SMALL);
    assert(api.header.struct_size == 0);
    assert(entry(MEOW_ABI_REVISION, sizeof(api), &api, NULL) == MEOW_OK);
    assert(meow_check_plugin_api(&api, sizeof(api)) == MEOW_OK);
    assert(meow_check_plugin_api(&api, sizeof(api) - 1) == MEOW_ERROR_BUFFER_TOO_SMALL);

    extended.api = api;
    extended.api.header.struct_size = sizeof(extended);
    assert(meow_check_plugin_api(&extended.api, sizeof(extended)) == MEOW_OK);
    api.header.abi_revision++;
    assert(meow_check_plugin_api(&api, sizeof(api)) == MEOW_ERROR_INCOMPATIBLE_ABI);
    api.header.abi_revision = MEOW_ABI_REVISION;
    api.header.struct_size = sizeof(meow_header_t);
    assert(meow_check_plugin_api(&api, sizeof(api)) == MEOW_ERROR_BUFFER_TOO_SMALL);
    api.header.struct_size = sizeof(api);

#define MISSING(member)                                                                          \
    do {                                                                                         \
        meow_plugin_api_t missing = api;                                                         \
        missing.member = NULL;                                                                   \
        assert(meow_check_plugin_api(&missing, sizeof(missing)) == MEOW_ERROR_INVALID_ARGUMENT); \
    } while (0)
    MISSING(describe);
    MISSING(create);
    MISSING(query_interface);
    MISSING(bind);
    MISSING(start);
    MISSING(stop);
    MISSING(destroy);
#undef MISSING

    {
        uint8_t storage[] = {0, 0, 0, 0x7f};
        const meow_bytes_t message = {(const uint8_t *)"hello", 5};
        const meow_bytes_t empty = {NULL, 0};
        const meow_bytes_t invalid = {NULL, 1};
        meow_error_t error = {{storage, 3}, 0, 0};
        assert(meow_set_error(&error, &message) == MEOW_OK);
        assert(error.size == 3 && error.required == 5);
        assert(memcmp(storage, "hel", 3) == 0 && storage[3] == 0x7f);
        assert(meow_set_error(&error, &empty) == MEOW_OK);
        assert(error.size == 0 && error.required == 0);
        assert(meow_set_error(&error, &invalid) == MEOW_ERROR_INVALID_ARGUMENT);
        assert(error.size == 0 && error.required == 0);
        error.buffer.data = NULL;
        assert(meow_set_error(&error, &message) == MEOW_ERROR_INVALID_ARGUMENT);
        error.buffer.capacity = 0;
        assert(meow_set_error(&error, &message) == MEOW_OK);
        assert(error.size == 0 && error.required == 5);
        assert(meow_set_error(NULL, NULL) == MEOW_OK);
    }
    return 0;
}
