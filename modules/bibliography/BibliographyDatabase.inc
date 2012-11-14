<?php

/**
 * @file
 *
 * Database backend for the bibliography
 */

/**
 * Database backend for the bibliography
 */
class BibliographyDatabase {
  /**
   * The name of the table where the citations are stored.
   */
  const table = 'islandora_bibliography';

  /**
   * Checks it the given citation is already in the bibliography.
   *
   * @param string $pid
   *   The citations object's pid.
   * @param string $uid
   *   The user who is bibliography to check.
   *
   * @return boolean
   */
  public static function Exists($pid, $uid) {
    $results = db_select(self::table, 't')
      ->fields('t', array('id'))
      ->condition('pid', $pid)
      ->condition('uid', $uid)
      ->execute();
    $count = $results->rowCount();
    return $count == 1;
  }

  /**
   * Adds a citation to the bibliography.
   *
   * @param string $pid
   *   The citations object's pid.
   * @param int $uid
   *   The user who is storing the $pid.
   *
   * @return boolean
   *   TRUE if successfully stored in the database, FALSE otherwise.
   */
  public static function AddCitation($pid, $uid) {
    $record = new stdClass();
    $record->uid = (int) $uid;
    $record->pid = $pid;
    return drupal_write_record(self::table, $record) !== FALSE;
  }

  /**
   * Remove a citation from the bibliography
   *
   * @param string $pid
   *   The citations object's pid.
   *
   * @param int $uid
   *   The user who is removing the $pid.
   */
  public static function RemoveCitation($pid, $uid) {
    db_delete(self::table)
      ->condition('pid', $pid)
      ->condition('uid', $uid)
      ->execute();
  }

  /**
   * Gets all the citations from the users bibliography.
   *
   * @param int $uid
   *   The user bibliography to use.
   *
   * @return array
   *   All the citations in the users bibliography.
   */
  public static function GetCitations($uid) {
    $output = array();
    $results = db_select(self::table, 't')
      ->fields('t', array('pid'))
      ->condition('uid', $uid)
      ->execute();

    foreach ($results as $data) {
      $output[] = $data->pid;
    }
    return $output;
  }

}
