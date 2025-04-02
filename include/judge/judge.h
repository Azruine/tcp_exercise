#pragma once
#ifndef JUDGE_H
#define JUDGE_H

#define TEMP_OUTPUT "temp/temp_output"
#define COMPILE_ERROR_FILE "temp/compile_error.txt"
#define IO_DIR "io"

/**
 * @brief Replace all occurrences of substring 'old' in 'str' with 'new_str'.
 *      The result is heap-allocated and should be freed by the caller.
 * @param str input string.
 * @param old substring to replace.
 * @param new_str new substring.
 * @return heap-allocated string with all occurrences of 'old' replaced by
 * 'new_str'.
 */
char *replace_substring(const char *str, const char *old, const char *new_str);

/**
 * @brief Sanitize an error message by masking file paths and file names.
 * @param msg error message to sanitize.
 * @return sanitized error message (heap-allocated), or NULL on error.
 */
char *sanitize_error_message(const char *msg);

/**
 * @brief Compile the submission.
 * @param source_path path to the source file.
 * @param executable_path path to the compiled executable.
 * @return 0 on success, non-zero on compile error.
 */
int compile_submission(const char *source_path, const char *executable_path);
/**
 * @brief Run the compiled submission against a test case.
 *
 * @param in_path path to the input file.
 * @param expected_out path to the expected output file.
 * @param exec_time execution time in ms (output).
 * @param max_rss used memory (output).
 * @param executable_path path to the compiled executable.
 * @return 2 if test passed (Accepted),
 *         1 if output does not match (Wrong Answer),
 *        -1 if runtime error occurred.
 */
int run_test(const char *in_path, const char *expected_out, int *exec_time,
             long *max_rss, const char *executable_path);

#endif  // JUDGE_H