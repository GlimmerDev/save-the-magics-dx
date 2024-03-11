/* jansson_private_config.h for Android */
#ifndef JANSSON_PRIVATE_CONFIG_H
#define JANSSON_PRIVATE_CONFIG_H

/* Standard integer types are available */
#define HAVE_STDINT_H 1
#define HAVE_INTTYPES_H 1

/* Common POSIX headers and functions */
#define HAVE_UNISTD_H 1
#define HAVE_SYS_TYPES_H 1
#define HAVE_SYS_STAT_H 1
#define HAVE_FCNTL_H 1

/* Functions commonly available */
#define HAVE_CLOSE 1
#define HAVE_OPEN 1
#define HAVE_READ 1

/* Use /dev/urandom for random number generation */
#define USE_URANDOM 1

/* Atomic builtins - check based on compiler version */
#if defined(__clang__) || defined(__GNUC__)
#define HAVE_ATOMIC_BUILTINS 1
#define HAVE_SYNC_BUILTINS 1
#endif

/* Additional configurations can be added here based on specific needs and testing */

#endif /* JANSSON_PRIVATE_CONFIG_H */