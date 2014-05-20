<?php

/**
 * @file
 * Database interface.
 */

/**
 * Stash a set of PIDs in the database.
 *
 * @param array $pids
 *   An array of PIDs to stash.
 *
 * @return string
 *   A string identifying the queue into which things have been stashed.
 */
function citation_exporter_create_set(array $pids = array()) {
  $queue_id = citation_exporter_get_queue_id();
  $queue = DrupalQueue::get($queue_id, TRUE);
  $queue->createQueue();
  array_map(array($queue, 'createItem'), $pids);

  return $queue_id;
}

/**
 * Get the name of the queue, which should be unique.
 *
 * @return string
 *   An ID to use for a queue.
 */
function citation_exporter_get_queue_id($set_id) {
  global $user;
  return format_string('citation_exporter:set:!uid:!microtime', array(
    '!uid' => $user->uid,
    '!microtime' => microtime(TRUE),
  ));
}
