/* FEEL FREE TO EDIT THIS FILE
 * - test_wlf_sched.c (Wlf Scheduler Library Code)
 * - Copyright of Starter Code: Prof. Kevin Andrea, George Mason University.  All Rights Reserved
 * - Copyright of Student Code: You!
 * - Date: Jan 2025
 *
 *   Framework to demonstrate how you can write a Unit Tester for your Project
 *   - Not a requirement to use, but it may be helpful if you like.
 *   - This only has one example of a test case in it, so it is just to show how to write them.
 *
 *   This lets you test your wlf_sched.c code WITHOUT any of the StrawHat code running.
 *
 *   Why run all tests every time instead of checking a function once to see that it works?
 *   - You could break existing code when adding a new feature.
 *   - Test files like this are useful to check all of the tests every time, so you can make sure
 *     you didn't break something that used to work when you add new code.
 */

/* Standard Library Includes */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
/* Local Includes */
#include "wlf_sched.h" // Your schedule for the functions you're testing.
#include "strawhat_support.h" // Gives PRINT_ERROR, PRINT_WARNING, PRINT_STATUS, PRINT_DEBUG commands

/* Globals (static means it's private to this file only) */
int g_debug_mode = 1; // Hardcodes debug on for the custom print functions
 
/* Local Prototypes */
static void printing_macro_demonstration();
static void test_wlf_initialize();
static void test_queue_initialized(Wlf_queue_s *queue);

/* This is an EXAMPLE tester file, change anything you like!
 * - This shows an example by testing wlf_initialize.
 * - To make this useful, modify this function or create others for the rest of the
 *   functions that you want to test from your code.
 */
int main() {
  /* Setting g_debug_mode to 1 will turn on all debug print macros in your entire proram.
   * Setting g_debug_mode to 0 will turn off all debug print macros in your entire program.
   */
  g_debug_mode = 1;  // These helper functions only print if g_debug_mode is 1

  printing_macro_demonstration();  // Show a demo of some printing Macros you can use!

  test_wlf_initialize(); // Run the wlf_initialize test cases

  // You would add more calls to testing helper functions that you like.
  // Then when done, you can print a nice message an then return.
  PRINT_STATUS("All tests complete!\n");
  return 0;
}

/* This function shows how to use some debugging macros that we created. 
 *   These macros were written for convenience.
 *   Feel free to use any, all, or none of these at your pleasure.
 * 
 *   These will only print if g_debug_mode is set to 1.

 */
static void printing_macro_demonstration() {
  printf("Starting a demo of the macros.  g_debug_mode is currently set to %d\n", g_debug_mode);

  printf("The following will work if g_debug_mode is 0 or 1\n");
  /* PRINT_INFO is a helper macro to print a quick message.  This will always print.
   * - This works just like printf.
   * - g_debug_mode has no effect on PRINT_INFO
   */
  PRINT_INFO("This is a sample PRINT_INFO statement.\n");
  
  /* Note: MARK is a really cool helper macro.  Like the others, it works just like printf.
   * It also prints out the file, function, and line number that it was called from.
   * You can use it to check values throuhgout the code and mark where you printed from.
   * BTW: gdb also does this much better too.
   */
  MARK("I can be used anywhere, even if debug mode is off.\n");
  MARK("I work just like printf! %s %d %lf\n", "Cool!", 42, 3.14);

  printf("The following will only print if g_debug_mode is 1\n");

  /* PRINT_DEBUG, PRINT_STATUS, PRINT_WARNING, PRINT_ERROR, and PRINT_ERROR are all 
   *   helper macros that we wrote to print messages to the screen during debugging.
   * - Feel free to use these, or not, based on your own preferences.
   * - These work just like printf, but print different labels and they add in a newline at the end.
   * - The PRINT_* macros are available for your wlf_sched.c as well, if you want to use them.
   */
  PRINT_STATUS("This is a sample PRINT_STATUS message.\n");

  // You can use these just like printf as well.
  int test_no = 42;
  PRINT_STATUS("Test %d: This is a sample PRINT_STATUS message with args.\n", test_no);
  
  // Quick demos of the others in action
  PRINT_DEBUG("This is a sample PRINT_DEBUG statement.\n");
  PRINT_WARNING("This is a sample PRINT_WARNING statement!  It's like a debug, but looks more important.\n");
  PRINT_ERROR("This is a sample PRINT_ERROR message.  Use this for debugging to check for bad conditions.\n");

  printf("Finished demonstrating the macros.\n\n");
}

/* Local function to test schedule_initialize from wlf_sched.c */
static void test_wlf_initialize() {
  PRINT_INFO("Test 1: Testing wlf_initialize()\n");
  // Begin Testing
  // - You can just call the function you want to test with any arguments needed.
  // - You may need to set up some arguments first, depending on what you want to test.
  Wlf_schedule_s *schedule = wlf_initialize();
  
  // Now that we called it and got the pointer to the schedule, let's test to see if we did it right!
  if(schedule == NULL) {
    // PRINT_ERROR will kill the program when it hits, which is good if you hit a bug.
    PRINT_ERROR("...wlf_initialize returned NULL!\n"); 
  }
  else {
    // Header is good, so let's test the queues to see if they're all initialized properly.
    PRINT_STATUS("...Checking the Ready Queue High\n");
    test_queue_initialized(schedule->ready_queue_high); // This is another helper function I wrote in this file.
    PRINT_STATUS("...Checking the Ready Queue Normal\n");
    test_queue_initialized(schedule->ready_queue_normal); // This is another helper function I wrote in this file.
    PRINT_STATUS("...Checking the Terminated Queue\n");
    test_queue_initialized(schedule->terminated_queue); // This is another helper function I wrote in this file.
  }

  PRINT_STATUS("...wlf_initialize test complete.\n\n");
}

/* Helper function to test if a queue is properly initialized
 * Exits the program with PRINT_ERROR on any failures.
 */
static void test_queue_initialized(Wlf_queue_s *queue) {
  // Always test a pointer before dereferencing it!
  if(queue == NULL) {
    PRINT_ERROR("...tried to test a NULL queue!\n");
  }

  // Check that the head pointer is NULL.
  if(queue->head != NULL) {
    PRINT_ERROR("...the Queue doesn't have a NULL head pointer!\n");
  }
  // Check that the count is 0.
  if(queue->count != 0) {
    PRINT_ERROR("...the Queue's count is not initialized to 0!\n");
  }
}
