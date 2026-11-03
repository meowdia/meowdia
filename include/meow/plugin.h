/* SPDX-FileCopyrightText: 2026 Jiffly */
/* SPDX-License-Identifier: MIT OR Apache-2.0 */

#ifndef MEOW_PLUGIN_H
#define MEOW_PLUGIN_H

#include <meow/base.h>

#define MEOW_PLUGIN_ENTRY_SYMBOL "meow_plugin_entry_v0"
#define MEOW_REQUIREMENT_OPTIONAL UINT32_C(1)

#define MEOW_LOG_ERROR UINT32_C(1)
#define MEOW_LOG_WARN UINT32_C(2)
#define MEOW_LOG_INFO UINT32_C(3)
#define MEOW_LOG_DEBUG UINT32_C(4)

/* Uses nonempty UTF-8 IDs and names without NUL bytes. */
typedef struct meow_capability {
    meow_bytes_t id;
    uint32_t revision;
    uint32_t reserved;
} meow_capability_t;

typedef struct meow_requirement {
    meow_bytes_t name;
    meow_capability_t capability;
    uint32_t flags;
    uint32_t reserved;
} meow_requirement_t;

/* Library-owned metadata, valid until process exit. IDs and requirement names must be unique. */
typedef struct meow_plugin_descriptor {
    meow_header_t header;
    meow_bytes_t id;
    meow_bytes_t version;
    const meow_capability_t *provides;
    size_t provides_count;
    const meow_requirement_t *requirements;
    size_t requirements_count;
} meow_plugin_descriptor_t;

typedef struct meow_host_api {
    meow_header_t header;
    void *context;
    /* Required, thread-safe logging. Never block or call back into the plugin.
     * Copy messages you keep. context can be NULL.
     */
    void(MEOW_CALL *log)(void *context, uint32_t level, const meow_bytes_t *message);
} meow_host_api_t;

/* A dependency's handle and function table. Copy anything kept after bind returns.
 * The provider must outlive the consumer.
 */
typedef struct meow_binding {
    meow_bytes_t name;
    meow_capability_t capability;
    meow_handle_t provider;
    const void *api;
    uint32_t api_size;
    uint32_t reserved;
} meow_binding_t;

/* All functions are required. Run lifecycle calls one at a time per instance.
 * Only stop may overlap capability calls. describe may run concurrently.
 * Exceptions and panics must stay inside the plugin even if Meow handles them.
 */
typedef struct meow_plugin_api {
    meow_header_t header;

    /* Copy plugin metadata into out.
     * Leave out unchanged if capacity is too small or the call fails.
     */
    meow_status_t(MEOW_CALL *describe)(
        uint32_t capacity, meow_plugin_descriptor_t *out, meow_error_t *error
    );

    /* Create an instance from JSON config; empty config means {}. */
    meow_status_t(MEOW_CALL *create)(
        const meow_host_api_t *host,
        const meow_bytes_t *config,
        meow_handle_t *out,
        meow_error_t *error
    );

    /* Copy a capability's function table into out. */
    meow_status_t(MEOW_CALL *query_interface)(
        meow_handle_t instance,
        const meow_capability_t *capability,
        uint32_t capacity,
        void *out,
        meow_error_t *error
    );

    /* Bind dependencies before starting, even if there are none. Copy retained bindings.
     * On failure, leave the instance ready for retry or cleanup.
     */
    meow_status_t(MEOW_CALL *bind)(
        meow_handle_t instance, const meow_binding_t *bindings, size_t count, meow_error_t *error
    );

    /* Start after dependencies have started. If this fails, stop before destroying;
     * do not retry start.
     */
    meow_status_t(MEOW_CALL *start)(meow_handle_t instance, meow_error_t *error);

    /* Stop work and wait up to timeout_ms. On timeout, keep the instance alive and retry.
     * A stopped instance cannot restart.
     */
    meow_status_t(MEOW_CALL *stop)(
        meow_handle_t instance, uint32_t timeout_ms, meow_error_t *error
    );

    /* Free an instance that never attempted start or has stopped successfully.
     * Invalidate its handle.
     */
    meow_status_t(MEOW_CALL *destroy)(meow_handle_t instance, meow_error_t *error);
} meow_plugin_api_t;

/* Return the plugin API for the requested revision and buffer size.
 * Leave out unchanged on failure.
 */
typedef meow_status_t(MEOW_CALL meow_plugin_entry_fn)(
    uint32_t revision, uint32_t capacity, meow_plugin_api_t *out, meow_error_t *error
);
MEOW_EXPORT meow_plugin_entry_fn meow_plugin_entry_v0;

#endif
