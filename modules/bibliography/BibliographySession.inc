<?php

/**
 * @file
 *
 * Session backend for the Bibliography.
 */

/**
 * Session backend for the Bibliography
 */
class BibliographySession {

  /**
   * Gets the storage location of the bibilography from the SESSION table.
   *
   * @return array
   *   The storage location of the bibliography.
   */
  private static function &GetSessionTable() {
    if (!isset($_SESSION['bibliography']['pids'])) {
      $_SESSION['bibliography'] = array('pids' => array());
    }
    return $_SESSION['bibliography']['pids'];
  }

  /**
   * Checks if the given citation is already in the bibliography.
   *
   * @param string $pid
   *   The citations object's pid.
   *
   * @return boolean
   */
  public static function Exists($pid) {
    $table = &self::GetSessionTable();
    return isset($table[$pid]);
  }

  /**
   * Adds a citation to the bibliography.
   *
   * @param string $pid
   *   The citations object's pid.
   */
  public static function AddCitation($pid) {
    $table = &self::GetSessionTable();
    if(!isset($table[$pid])) {
      $table[$pid] = $pid;
      return TRUE;
    }
    else {
      return FALSE;
    }
  }

  /**
   * Removes a citation from the bibliography
   *
   * @param string $pid
   *   The citations object's pid.
   */
  public static function RemoveCitation($pid) {
    $table = &self::GetSessionTable();
    unset($table[$pid]);
  }

  /**
   * Gets all the citations from the users bibliography.
   *
   * @return array
   *   Where the keys are the pids of the citation objects within the users bib liography.
   */
  public static function GetCitations() {
    $table = &self::GetSessionTable();
    return $table;
  }

}