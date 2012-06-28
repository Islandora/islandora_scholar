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
    $query = db_query('SELECT count(id) FROM {%s} WHERE pid = "%s" AND uid = "%d"', self::table, $pid, $uid);
    $count = db_result($query);
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
    db_query('DELETE FROM {%s} WHERE pid = "%s" AND uid = "%d"', self::table, $pid, $uid);
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
    $query = db_query('SELECT pid FROM {%s} WHERE uid = "%d"', self::table, $uid);
    while ($row = db_fetch_array($query)) {
      $output[] = $row['pid'];
    }
    return $output;
  }

}
