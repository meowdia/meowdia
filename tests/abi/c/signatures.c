/* SPDX-FileCopyrightText: 2026 Jiffly */
/* SPDX-License-Identifier: MIT OR Apache-2.0 */

#include <meow/plugin.h>
#include <stddef.h>

static meow_status_t
describe(uint32_t capacity, meow_plugin_descriptor_t *out, meow_error_t *error) {
    (void)capacity;
    (void)out;
    (void)error;
    return MEOW_ERROR_INTERNAL;
}

static meow_status_t create(
    const meow_host_api_t *host, const meow_bytes_t *config, meow_handle_t *out, meow_error_t *error
) {
    (void)host;
    (void)config;
    (void)out;
    (void)error;
    return MEOW_ERROR_INTERNAL;
}

static meow_status_t query(
    meow_handle_t instance,
    const meow_capability_t *capability,
    uint32_t capacity,
    void *out,
    meow_error_t *error
) {
    (void)instance;
    (void)capability;
    (void)capacity;
    (void)out;
    (void)error;
    return MEOW_ERROR_INTERNAL;
}

static meow_status_t
bind(meow_handle_t instance, const meow_binding_t *bindings, size_t count, meow_error_t *error) {
    (void)instance;
    (void)bindings;
    (void)count;
    (void)error;
    return MEOW_ERROR_INTERNAL;
}

static meow_status_t lifecycle(meow_handle_t instance, meow_error_t *error) {
    (void)instance;
    (void)error;
    return MEOW_ERROR_INTERNAL;
}

static meow_status_t stop(meow_handle_t instance, uint32_t timeout_ms, meow_error_t *error) {
    (void)timeout_ms;
    return lifecycle(instance, error);
}

meow_status_t meow_plugin_entry_v0(
    uint32_t revision, uint32_t capacity, meow_plugin_api_t *out, meow_error_t *error
) {
    const meow_plugin_api_t api = {
        {sizeof(meow_plugin_api_t), MEOW_ABI_REVISION},
        describe,
        create,
        query,
        bind,
        lifecycle,
        stop,
        lifecycle
    };
    (void)error;
    if (out == NULL) {
        return MEOW_ERROR_INVALID_ARGUMENT;
    }
    if (revision != MEOW_ABI_REVISION) {
        return MEOW_ERROR_INCOMPATIBLE_ABI;
    }
    if (capacity < sizeof(api)) {
        return MEOW_ERROR_BUFFER_TOO_SMALL;
    }
    *out = api;
    return MEOW_OK;
}
