/**
 * @brief Runs a player's game turn, applying an execution time limit,
 *   and saving their best plays (`register_best_play`).
 *
 * Can produce either a Hunter player or a Dracula player,
 * depending on whether `I_AM_DRACULA` is set.
 *
 * Takes in JSON on stdin, to build the player's view:
 *     {"past_plays": "...", "messages": ["messageHere", ...]}
 *
 * Outputs JSON on stdout, when either `decide_{hunter,dracula}_move`
 * finishes or a timeout occurs; in both situations, the program exits
 * after outputting:
 *     {"move": "..", "message": "my-message"}
 *
 * Outputs any stdout or stderr made by the player onto stderr.
 *
 * Compiles with:
 *     gcc -o hunter player.c hunter.c hunter_view.c ... -ljansson
 *     gcc -DI_AM_DRACULA -o dracula player.c dracula.c dracula_view.c ...
 * -ljansson where ... includes GameView.c along with any other player's files.
 *
 * Requires the jansson library: <http://www.digip.org/jansson/>
 *
 * Assume that students submit all of the relevant files
 * and a Makefile that uses them
 *
 * Code mostly from David Collien, written in 2012
 * 2017-12-04	v1.1	Team Dracula <cs2521@cse.unsw.edu.au>
 * 2018-01-21	v2.0	Team Dracula <cs2521@cse.unsw.edu.au>
 * 2018-12-31	v2.1	Team Dracula <cs2521@cse.unsw.edu.au>
 */

#include <err.h>
#include <signal.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <sysexits.h>
#include <unistd.h>

#define JSON_INTEGER_IS_LONG_LONG
#include <jansson.h>

#include "game.h"
#ifdef I_AM_DRACULA
#include "dracula.h"
#include "dracula_view.h"
#else
#include "hunter.h"
#include "hunter_view.h"
#endif

#ifndef _unused
#define _unused __attribute__((unused))
#endif

#define USEC_TO_SEC 1000000LL

/// convert milliseconds into seconds component
#define LIMIT_SEC (TURN_LIMIT_MSECS / 1000)

/// extract milliseconds component and convert to microseconds
#define LIMIT_USEC ((TURN_LIMIT_MSECS % 1000) * 1000)

/// moves given by register_best_play are this long (including terminator)
#define MOVE_SIZE 3

// Options here include:
//   SIGVTALRM, ITIMER_VIRTUAL
//     limit the hunter's processing time (CPU seconds)
//
//   SIGPROF, ITIMER_PROF
//     limit the hunter's system time
//     (processing + anything system needs to do to run the hunter)
//
//   SIGALRM, ITIMER_REAL
//     real-world seconds over all currently running processes
//
#define TIMER_SIGNAL SIGPROF
#define TIMER_TYPE ITIMER_PROF

#define EXIT_TIMEOUT 42

// A pseudo-generic interface, which defines
// - a type `view_t',
// - functions `view_new', `decide_move', `view_drop',
#ifdef I_AM_DRACULA
typedef DraculaView view_t;
#define view_new dv_new
#define decide_move decide_dracula_move
#define view_drop dv_drop
#else
typedef HunterView view_t;
#define view_new hv_new
#define decide_move decide_hunter_move
#define view_drop hv_drop
#endif

// The minimum static globals I can get away with
static struct player_state {
  struct timeval time_start;
  struct timeval time_end;

  struct itimerval timer;
  struct sigaction timer_action;

  // stores input data
  struct input_data {
    char *past_plays;
    player_message *messages;
    size_t messages_len;
  } input_data;

  int fd;
  fpos_t pos;
  bool is_timing;
  bool is_redirected;
  char latest_play[MOVE_SIZE];
  char latest_message[MESSAGE_SIZE];
} *_ps_ = NULL;

// pipe redirection
static void stdout_redirect(struct player_state *);
static void stdout_reset(struct player_state *);

static void get_input(struct input_data *);
static void free_input(struct input_data *);
static void output_move(struct player_state *);

static void timer_start(struct player_state *);
static void timer_stop(struct player_state *);
static void timeout_handler(int signalID);

static void finish(struct player_state *ps);

////////////////////////////////////////////////////////////////////////
int main(void) {
  struct player_state ps = {
      .is_timing = false,
      .is_redirected = false,
      .latest_play = "",
      .latest_message = "",
  };
  _ps_ = &ps;

  // we're using stdout for move output
  // user's stdout is redirected to stderr
  stdout_redirect(&ps);
  get_input(&ps.input_data);

  // start timer and enable timer interrupt handler
  timer_start(&ps);

  view_t state = view_new(ps.input_data.past_plays, ps.input_data.messages);
  decide_move(state);
  view_drop(state);

  // stop timing straight away after decide_move finishes;
  finish(&ps);

  return EXIT_SUCCESS;
}

////////////////////////////////////////////////////////////////////////
/// Saves characters from play (and appends a terminator),
/// and saves characters from message (and appends a terminator)
void register_best_play(char *play, player_message message) {
  strncpy(_ps_->latest_play, play, MOVE_SIZE - 1);
  _ps_->latest_play[MOVE_SIZE - 1] = '\0';

  strncpy(_ps_->latest_message, message, MESSAGE_SIZE);
  _ps_->latest_message[MESSAGE_SIZE - 1] = '\0';
}

////////////////////////////////////////////////////////////////////////
/// Redirect stdout to stderr, if not already redirected
static void stdout_redirect(struct player_state *ps) {
  if (ps->is_redirected) {
    warnx("player output is already redirected");
    return;
  }

  fflush(stdout);
  fgetpos(stdout, &ps->pos);
  ps->fd = dup(fileno(stdout));
  dup2(fileno(stderr), fileno(stdout));

  ps->is_redirected = true;
}

/// Undo stdout redirect, if it's been redirected
static void stdout_reset(struct player_state *ps) {
  if (!ps->is_redirected) {
    warnx("player output isn't redirected");
    return;
  }

  fflush(stdout);
  dup2(ps->fd, fileno(stdout));
  close(ps->fd);
  clearerr(stdout);
  fsetpos(stdout, &ps->pos);

  ps->is_redirected = false;
}

////////////////////////////////////////////////////////////////////////
/// Output the last registered move as JSON
static void output_move(struct player_state *ps) {
  double start_time_uS =
      ps->time_start.tv_sec * USEC_TO_SEC + (ps->time_start.tv_usec);
  double end_time_uS =
      ps->time_end.tv_sec * USEC_TO_SEC + (ps->time_end.tv_usec);
  json_int_t uS_taken = (json_int_t)end_time_uS - (json_int_t)start_time_uS;

  json_t *output_json = json_object();

  // create new JSON Strings from the last registered move;
  // store them inside the JSON Object (and automatically clean references)
  json_t *move_json = json_string(ps->latest_play);
  json_object_set_new(output_json, "move", move_json);

  json_t *message_json = json_string(ps->latest_message);
  json_object_set_new(output_json, "message", message_json);

  json_t *timer_json = json_integer(uS_taken);
  json_object_set_new(output_json, "timer", timer_json);

  json_dumpf(output_json, stdout, 0);
  json_decref(output_json);

  puts("");
  fflush(stdout);
}

////////////////////////////////////////////////////////////////////////
/// Allocate input data and get JSON from stdin
static void get_input(struct input_data *input_data) {
  // get JSON object from STDIN
  json_error_t error;
  json_t *rootJSON = json_loadf(stdin, 0, &error);
  if (rootJSON == NULL)
    errx(EX_DATAERR, "input error: on line %d: %s", error.line, error.text);

  // get past plays string from the JSON object
  json_t *past_plays_json = json_object_get(rootJSON, "past_plays");
  if (!json_is_string(past_plays_json))
    errx(EX_DATAERR, "input error: $.past_plays is not a string");

  // get messages array from the JSON object
  json_t *messages_json = json_object_get(rootJSON, "messages");
  if (!json_is_array(messages_json))
    errx(EX_DATAERR, "input error: $.messages is not an array");

  // allocate and copy past plays
  const char *past_plays = json_string_value(past_plays_json);
  input_data->past_plays = strdup(past_plays);
  if (input_data->past_plays == NULL)
    errx(EX_OSERR, "couldn't strdup $.past_plays");

  // allocate messages in struct
  size_t messages_len = json_array_size(messages_json);
  input_data->messages_len = messages_len;
  input_data->messages = calloc(messages_len, sizeof(player_message));
  if (input_data->messages == NULL)
    errx(EX_OSERR, "couldn't allocate input_data->messages");

  for (size_t i = 0; i < messages_len; i++) {
    json_t *message_item_json = json_array_get(messages_json, i);

    if (!json_is_string(message_item_json))
      errx(EX_DATAERR, "input error: $.messages[%zu] is not a string", i);

    // copy over message string
    const char *message = json_string_value(message_item_json);
    strncpy(input_data->messages[i], message, MESSAGE_SIZE);
    input_data->messages[i][MESSAGE_SIZE - 1] = '\0';
  }

  json_decref(rootJSON);
}

/// Clean up memory allocated for the input data
static void free_input(struct input_data *input_data) {
  free(input_data->past_plays);
  free(input_data->messages);
}

////////////////////////////////////////////////////////////////////////
/// Start the process timer
static void timer_start(struct player_state *ps) {
  // tell it to fire the timeout_handler interrupt
  // when TIMER_SIGNAL is received
  memset(&ps->timer_action, 0, sizeof(struct sigaction));
  ps->timer_action.sa_handler = &timeout_handler;
  sigaction(TIMER_SIGNAL, &ps->timer_action, NULL);

  // set up timer's timing time
  ps->timer.it_value.tv_usec = LIMIT_USEC;
  ps->timer.it_value.tv_sec = LIMIT_SEC;

  // don't re-fire on an interval
  ps->timer.it_interval.tv_usec = 0;
  ps->timer.it_interval.tv_sec = 0;

  // flag that we're now timing
  ps->is_timing = true;

  gettimeofday(&ps->time_start, NULL);

  // start your engines
  setitimer(TIMER_TYPE, &ps->timer, NULL);
}

/// Stop the process timer.
static void timer_stop(struct player_state *ps) {
  ps->is_timing = false;
  gettimeofday(&ps->time_end, NULL);
}

/// Called when the `TIMER_SIGNAL` is received.
static void timeout_handler(int signalID _unused) {
  struct player_state *ps = _ps_;
  if (ps->is_timing) {
    finish(ps);
    exit(EXIT_TIMEOUT);
  }
}

//////////////////////////////////////////////////////////////////////////////////
/// what to do if interrupted by timer:
/// let stdout be used again
/// print out the move data, and bail.
static void finish(struct player_state *ps) {
  timer_stop(ps);
  stdout_reset(ps);
  output_move(ps);
  free_input(&ps->input_data);
}
