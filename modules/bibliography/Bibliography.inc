<?php

/**
 * @file
 *
 * Implements a class for interacting with the bibilography's stored citations.
 */

/**
 * Only this class should be used externally.
 */
class Bibliography {

  /**
   * Gets the the current user id if non is provided.
   *
   * @global object $user
   *   The drupal user object.
   *
   * @param int $uid
   *   A drupal user id.
   *
   * @return int
   *   A drupal user id.
   */
  private static function GetUID($uid = NULL) {
    global $user;
    $uid = isset($uid) ? $uid : $user->uid;
    return $uid;
  }

  /**
   * Get the class used to store the pids in the bibliography.
   *
   * @param int $uid
   *   A drupal user id.
   */
  private static function GetSource($uid) {
    if ($uid == 0) { // Anonymous
      module_load_include('inc', 'islandora_bibliography', 'BibliographySession');
      return 'BibliographySession';
    }
    else {
      module_load_include('inc', 'islandora_bibliography', 'BibliographyDatabase');
      return 'BibliographyDatabase';
    }
  }

  /**
   * Checks to see if the given pid is an citation object.
   *
   * @param string $pid
   */
  private static function IsCitation($pid) {
    module_load_include('inc', 'fedora_repository', 'api/fedora_item');
    $item = new Fedora_Item($pid);
    if ($item->exists()) {
      module_load_include('inc', 'fedora_repository', 'ObjectHelper');
      $objectHelper = new ObjectHelper();
      $content_models = $objectHelper->get_content_models_list($pid);
      foreach ($content_models as $content_model) {
        if ($content_model->pid == 'ir:citationCModel') {
          return TRUE;
        }
      }
      return FALSE;
    }
    return FALSE;
  }

  /**
   * Checks to see if the given fedora object is stored in the user bibliography.
   *
   * @param string $pid
   *   The pid of the fedora object.
   * @param int $uid
   *   The drupal users id, this user's bibliography will be checked.
   *
   * @return boolean
   *   TRUE if the given fedora object exists within the bibliography, FALSE otherwise.
   */
  public static function Exists($pid, $uid = NULL) {
    $uid = self::GetUID($uid);
    return call_user_func(array(self::GetSource($uid), 'Exists'), $pid, $uid);
  }

  /**
   * Adds the given fedora object to the set of citations if possible.
   *
   * @param string $pid
   *   The pid of the fedora object.
   * @param int $uid
   *   The drupal users id, this user's bibliography will be checked.
   *
   * @return boolean
   *   TRUE if the given fedora object exists within the bibliography, FALSE otherwise.
   */
  public static function AddCitation($pid, $uid = NULL, $check = TRUE) {
    $add = $check ? self::IsCitation($pid) : TRUE;
    if ($add && !self::Exists($pid)) {
      $uid = self::GetUID($uid);
      return call_user_func(array(self::GetSource($uid), 'AddCitation'), $pid, $uid);
    }
    return FALSE;
  }

  /**
   * Adds the colleciton of given fedora object to the set of citations if possible.
   *
   * @param array $pids
   *   A collection of pids for citation objects.
   * @param int $uid
   *   The drupal users id, this user's bibliography will be checked.
   * @param boolean $check
   *   Check the PID before adding it.
   *
   * @return int
   *   The number of Citations Added.
   */
  public static function AddCitations(array $pids, $uid = NULL, $check = TRUE) {
    $count = 0;
    foreach ($pids as $pid) {
      if (self::AddCitation($pid, $uid, $check)) {
        $count++;
      }
    }
    return $count;
  }

  /**
   * Removes the given fedora object from the set of citations if possible.
   *
   * @param string $pid
   *   The pid of the fedora object.
   * @param int $uid
   *   The drupal users id, this user's bibliography will be checked.
   *
   * @return boolean
   *   TRUE if the given fedora object exists within the bibliography, FALSE otherwise.
   */
  public static function RemoveCitation($pid, $uid = NULL) {
    $uid = self::GetUID($uid);
    return call_user_func(array(self::GetSource($uid), 'RemoveCitation'), $pid, $uid);
  }

  /**
   * Removes the collection of given fedora object from the set of citations if possible.
   *
   * @param array $pids
   *   A collection of pids for citation objects.
   * @param int $uid
   *   The drupal users id, this user's bibliography will be checked.
   *
   * @return int
   *   The number of Citations Removed.
   */
  public static function RemoveCitations(array $pids, $uid = NULL) {
    $count = 0;
    foreach ($pids as $pid) {
      if (self::RemoveCitation($pid, $uid)) {
        $count++;
      }
    }
    return $count;
  }

  /**
   * Gets the pids of all the citations in the users bibliography.
   *
   * @param int $uid
   *   The drupal users id, this user's bibliography will be checked.
   *
   * @return array
   *
   */
  public static function GetCitations($uid = NULL) {
    $uid = self::GetUID($uid);
    return call_user_func(array(self::GetSource($uid), 'GetCitations'), $uid);
  }

}