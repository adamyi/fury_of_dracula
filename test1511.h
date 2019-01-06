////////////////////////////////////////////////////////////////////////
// Test suite tools for student code at UNSW CSE.
// Used in COMP1511 17s2, COMP2521 18x1.
//
// This file contains several horrible hacks to make it easier to
// write tests for student code.  I've collected together many
// interesting bits of cruft along the way.
//
// The most recent version of this file can be found at
//     <https://wiki.jashankj.space/Code/test1511/>
// along with some hopefully useful documentation.
//
//
// The most useful thing here is the `TEST` macro, which takes two or
// more arguments: first, an expression to test; and following that, a
// constant format string and format arguments.  It either prints to
// stderr and runs assert(3), or produces a stdout complying with the
// Test Anything Protocol, TAP.
//
//     TEST (6 * 9 == 42,
//         "life, the universe, everything (base 10s)");
//     TEST (6 * 9 == 54,
//         "life, the universe, everything (base 13s)");
//
// In assert(3) mode, on stderr:
//
//     -- t1: life, the universe, everything (base 10s)
//     Assertion failed
//     [and, if that didn't happen:]
//     -- t2: life, the universe, everything (base 13s)
//
// In TAP mode, on stdout:
//
//     not ok 1 # life, the universe, everything (base 10s)
//     ok 2 # life, the universe, everything (base 13s)
//
//
// For testing floating-point operations, I have `feq`, because `==`
// doesn't do what you expect.  It's a static inline function, and
// works a treat with the `TEST` macro.
//
//     TEST (feq (0.0, 0.0, 1),
//         "0.0 == 0.0 (± 1.0)")
//
//
// I have `have_symbol`, a function that tests if a symbol exists in an
// executable; it uses dlopen(3), dlsym(3), and ugly hacks.  It may not
// be very portable...
//
//     const char *sym = "imageAsBMP";
//     if (! have_symbol (sym))
//         errx (EX_SOFTWARE, "this test needs '%s'", sym);
//
//
// Various memory protection checks, wrapping ASan's interface in a
// slightly more pleasant (and less `ifdef`-heavy) one.
//
//  - mem_address_is_poisoned :: Addr# -> Bool
//  - mem_region_is_poisoned :: (Addr#, Word#) -> Bool
//  - mem_poison_region :: (Addr#, Word#) -> ()
//  - mem_unpoison_region :: (Addr#, Word#) -> ()
//  - mem_describe_address :: Addr# -> IO ()
//
//     String s = newString ("");
//     destroyString (s);
//     TEST (mem_address_is_poisoned (s),
//         "s is now gone")
//
//
// TODO: write some linked-list test goop.
//
// 2014-05-??	Jashank Jeremy <z5017851@cse.unsw.edu.au>
//      * (... the distant past: "test_tap.h", a terrible set of hacks
//        that have evolved into the current `TEST` macro.)
// 2017-09-27	Jashank Jeremy <{jashankj,z5017851}@cse.unsw.edu.au>
//      * Refactor `TEST` macro into the two forms here: one that
//        produces Test Anything Protocol (TAP) output, and one that
//        `assert(3)`'s and dumps information on stderr.
// 2017-10-10	Jashank Jeremy <{jashankj,z5017851}@cse.unsw.edu.au>
//      * Add `feq`, to do floating-point equality.
//        <https://wiki.jashankj.space/CSE/Hints/COMP1511FloatEqual/>
// 2017-10-19	Jashank Jeremy <{jashankj,z5017851}@cse.unsw.edu.au>
//      * Add `have_symbol`, to do symbol presence analysis in ADT
//        implementations using `dlopen` and `dlsym`.
// 2017-10-28	Jashank Jeremy <{jashankj,z5017851}@cse.unsw.edu.au>
//      * `mem_*` wrappers around the `#ifdef` mess for sanitizers.
// 2018-05-17	Jashank Jeremy <{jashankj,z5017851}@cse.unsw.edu.au>
//      * Add diagnostic '-Wno-unused-function'.
//      * Move header guard within '-Wno-unused-macros'
//      * List a canonical source for test1511.h!  Given how often I use
//        it for various student-code-testing projects, it makes sense
//        to publish it on my website.

////////////////////////////////////////////////////////////////////////
// Suppress some compiler warnings.

#ifdef __clang__
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wreserved-id-macro"
#pragma clang diagnostic ignored "-Wunused-macros"
#pragma clang diagnostic ignored "-Wunused-function"
#pragma clang diagnostic ignored "-Wgnu-zero-variadic-macro-arguments"

// glibc's headers are utterly fucked with clang, likely because
// the tgmath isn't handled correctly, so everything turns into:

// implicit conversion increases floating-point precision:
// 'double' to 'long double': isnan (x), isnan (y), isnan (eps)
#pragma clang diagnostic ignored "-Wdouble-promotion"
// implicit conversion loses floating-point precision:
// 'double' to 'float': isnan (x), isnan (y), isnan (eps)
#pragma clang diagnostic ignored "-Wconversion"
#endif

#ifndef TEST1511_H_
#define TEST1511_H_

////////////////////////////////////////////////////////////////////////
// Suppress stupid anti-<features.h>.
#ifndef _POSIX_C_SOURCE
#define _POSIX_C_SOURCE 200809L
#endif
#ifndef _GNU_SOURCE
#define _GNU_SOURCE 1
#endif
#ifndef _XOPEN_SOURCE
#define _XOPEN_SOURCE 700
#endif
#ifndef _BSD_SOURCE
#define _BSD_SOURCE 1
#endif
#ifndef _DEFAULT_SOURCE
#define _DEFAULT_SOURCE 1
#endif

#include <sys/cdefs.h>
#include <sys/param.h>

// While we're here:
#ifndef __unused
#define __unused __attribute__((unused))
#endif

#ifndef nitems
#define nitems(x) (sizeof((x)) / sizeof((x)[0]))
#endif

#include <assert.h>
#include <dlfcn.h>
#include <err.h>
#include <math.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <sysexits.h>

////////////////////////////////////////////////////////////////////////
// Test a thing.
//
// @param status
//     Test 'passed' if a true value.
// @param descr
//     Short description of test.

// The test number.
static int _test_ = 1;

#ifdef NDEBUG
#define TEST(status, descr, ...)                                           \
  ({                                                                       \
    _Bool passed_test = status;                                            \
    printf("%sok %d # " descr "\n", (passed_test ? "" : "not "), _test_++, \
           ##__VA_ARGS__);                                                 \
    if (__builtin_expect((!passed_test), 0)) {                             \
      _fails_++;                                                           \
    }                                                                      \
  })

// The number of tests failed.
static int _fails_ = 0;

#else  // remain calm and assert(3)
#include <assert.h>
#define TEST(status, descr, ...)                                     \
  ({                                                                 \
    fprintf(stderr, "-- t%d: " descr "\n", _test_++, ##__VA_ARGS__); \
    assert(status);                                                  \
  })
#endif

////////////////////////////////////////////////////////////////////////
// Floating-point equality.  Compares the left-hand side with the
// right-hand side, with a margin of error around the right.
//
// @param x
//     Left-hand side
// @param y
//     Right-hand side
// @param eps
//     Margin of error (taken on the right-hand side)

static inline bool feq(double x, double y, double eps) {
  return (!isnan(x) && !isnan(y) && !isnan(eps) && isgreaterequal(x, y - eps) &&
          islessequal(x, y + eps));
}

////////////////////////////////////////////////////////////////////////
// Symbol existence.  Does the named symbol exist?
//
// Uses dlopen(3) and dlsym(3).  May need -ldl, maybe also -rdynamic...?
// dcc's ASan dependency may give us -rdynamic, so we'd only need -ldl
// on silly platforms.
//
// @param sym
//     A symbol name to resolve in the currently-executing process.

static struct _dl_ {
  void *dl;
  size_t loaded;
} _dl_self = {
    NULL,
    false,
};

static inline void _dl_load_self(void) {
  assert(_dl_self.loaded == false);
  _dl_self.dl = dlopen(NULL, RTLD_LAZY | RTLD_LOCAL);
  if (_dl_self.dl == NULL)
    errx(EX_OSERR, "couldn't dlopen(3) ourself: %s", dlerror());
  _dl_self.loaded = true;
}

static inline void *get_symbol(const char *sym) {
  if (_dl_self.loaded != true) _dl_load_self();
  void *fptr = dlsym(_dl_self.dl, sym);
  if (fptr == NULL) warnx("couldn't resolve '%s': %s", sym, dlerror());
  return fptr;
}

static inline bool have_symbol(const char *sym) {
  return get_symbol(sym) != NULL;
}

////////////////////////////////////////////////////////////////////////
// Memory protections.
//
// If we're running with the Address Sanitizer, we can use its exposed
// interface to do memory checks.  Wrap the useful bits of ASan in a
// slightly more pleasant and less `ifdef`-heavy interface.
//
//  - mem_address_is_poisoned :: Addr# -> Bool
//    Report if this address is "poisoned" -- whether a one-byte read
//    will cause a memory error, either by ASan tracking or not.
//
//  - mem_region_is_poisoned :: (Addr#, Word#) -> Bool
//    Report if this region of memory is poisoned, as above.
//
//  - mem_poison_region :: (Addr#, Word#) -> ()
//    Mark this region of memory as "poisoned".
//
//  - mem_unpoison_region :: (Addr#, Word#) -> ()
//    Mark this region of memory as not poisoned.
//
//  - mem_describe_address :: Addr# -> IO ()
//    Print out what's known about the given address.
//
// For more details, see <sanitizer/asan_interface.h>.
// (llvm/projects/compiler-rt/include/sanitizer/asan_interface.h)

#ifdef __has_feature
#if __has_feature(address_sanitizer) || defined(__SANITIZE_ADDRESS__)
#include <sanitizer/asan_interface.h>
#define IN_ASAN 1
#define __used_in_asan
#endif
#endif

#ifndef __used_in_asan
#define __used_in_asan __attribute__((unused))
#endif

static inline bool mem_address_is_poisoned(
    const void volatile *addr __used_in_asan) {
#ifdef IN_ASAN
  return __asan_address_is_poisoned(addr);
#else
  return false;
#endif
}

static inline bool mem_region_is_poisoned(void *addr __used_in_asan,
                                          size_t size __used_in_asan) {
#ifdef IN_ASAN
  return __asan_region_is_poisoned(addr, size);
#else
  return false;
#endif
}

static inline void mem_poison_region(const void volatile *addr __used_in_asan,
                                     size_t size __used_in_asan) {
#ifdef IN_ASAN
  __asan_poison_memory_region(addr, size);
#endif
}

static inline void mem_unpoison_region(const void volatile *addr __used_in_asan,
                                       size_t size __used_in_asan) {
#ifdef IN_ASAN
  __asan_unpoison_memory_region(addr, size);
#endif
}

static inline void mem_describe_address(void *addr __used_in_asan) {
#ifdef IN_ASAN
  __asan_describe_address(addr);
#endif
}

////////////////////////////////////////////////////////////////////////
#ifdef __clang__
#pragma clang diagnostic pop
#endif

#endif  // !defined(TEST1511_H_)
