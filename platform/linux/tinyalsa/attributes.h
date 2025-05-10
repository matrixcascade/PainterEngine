#ifndef TINYALSA_ATTRIBUTES_H
#define TINYALSA_ATTRIBUTES_H

/** @defgroup libtinyalsa-attributes
 * @brief GCC attributes to issue diagnostics
 * when the library is being used incorrectly.
 * */

// FIXME: Disable the deprecated attribute in Android temporarily. pcm_read/write are marked as
//   deprecated functions in the latest tinyalsa in GitHub. However, there are lots of libraries in
//   Android using these functions and building with -Werror flags. Besides build breakage, the
//   behavior and interface of the successors, pcm_readi/writei, are also changed. Once all have
//   been cleaned up, we will enable this again.
#if defined(__GNUC__) && !defined(ANDROID)

/** Issues a warning when a function is being
 * used that is now deprecated.
 * @ingroup libtinyalsa-attributes
 * */
#define TINYALSA_DEPRECATED __attribute__((deprecated))

/** Issues a warning when a return code of
 * a function is not checked.
 * @ingroup libtinyalsa-attributes
 * */
#define TINYALSA_WARN_UNUSED_RESULT __attribute__((warn_unused_result))

#else /* __GNUC__ && !ANDROID */

/** This is just a placeholder for compilers
 * that aren't GCC or Clang.
 * @ingroup libtinyalsa-attributes
 * */
#define TINYALSA_DEPRECATED

/** This is just a placeholder for compilers
 * that aren't GCC or Clang.
 * @ingroup libtinyalsa-attributes
 * */
#define TINYALSA_WARN_UNUSED_RESULT

#endif /* __GNUC__ && !ANDROID */

#endif /* TINYALSA_ATTRIBUTES_H */
