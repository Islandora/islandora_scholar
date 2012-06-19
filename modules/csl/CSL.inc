<?php

/**
 * @file
 *
 * Implements a class for interacting with the stored CSL files.
 */
class CSL {
  /**
   * The name of the table where the CSL styles are stored.
   */
  const table = 'csl';

  /**
   * Checks to see if the given CSL style is a valid definition.
   *
   * @param DOMDocument $style
   *   A CSL 1.0 style file to check to see if it is valid.
   *
   * @return boolean
   *   TRUE if the given style is valid, FALSE otherwise.
   */
  public static function Valid($style_in) {
    $schema_filename = drupal_get_path('module', 'csl') . '/schema/csl.rng';
    $style = NULL;
    if ($style_in instanceof DOMDocument) {
      $style = $style_in;
    }
    else {
      $style = new DOMDocument();
      $style->loadXML($style_in);
    }
    /**
     * We don't want to show any of this errors to the end users, but we do want to log them.
     */
    libxml_use_internal_errors(TRUE); // Prevent Drupal from catching errors.  
    if (!($valid = $style->relaxNGValidate($schema_filename))) {
      self::loglibxmlErrors();
    }
    libxml_use_internal_errors(TRUE); // Allow Drupal to catching errors.  
    return $valid;
  }

  /**
   * Log any xml errors to the watchdog.
   */
  private static function loglibxmlErrors() {
    $errors = libxml_get_errors();
    foreach ($errors as $error) {
      $levels = array(
        LIBXML_ERR_WARNING => WATCHDOG_WARNING,
        LIBXML_ERR_ERROR => WATCHDOG_ERROR,
        LIBXML_ERR_FATAL => WATCHDOG_ERROR
      );
      $severity = $levels[$error->level];
      $message = t('code: %code message: %message file: %file line: %line column: %column', array(
        '%code' => $error->code,
        '%message' => $error->message,
        '%file' => $error->file,
        '%line' => $error->line,
        '%column' => $error->column)
      );
      watchdog('CSL', $message, NULL, $severity);
    }
    libxml_clear_errors();
  }

  /**
   * Checks to see if the given CSL style exists in the database.
   *
   * @param string $style_name
   *   The name of the CSL style.
   *
   * @return boolean
   *   TRUE if the given style exists, FALSE otherwise.
   */
  public static function Exists($style_name) {
    $query = db_query('SELECT count(name) FROM {%s} WHERE name = "%s"', self::table, $style_name);
    $count = db_result($query);
    return $count == 1;
  }

  /**
   * Gets a list of all the availible styles.
   *
   * @return array
   *   An array of defined style's names, where both the key and the value are the style's name.
   *   EX. array( 'name' => 'name' );
   */
  public static function GetNames() {
    $names = array();
    $result = db_query('SELECT name FROM {%s}', self::table);
    while ($data = db_fetch_object($result)) {
      $names[$data->name] = $data->name;
    }
    return $names;
  }

  /**
   * Sets the given style name to default if it exists.
   * 
   * @param string $style_name 
   *   The name of the style to set to the default.
   */
  public static function SetDefaultName($style_name) {
    if (self::Exists($style_name)) {
      variable_set('csl_default_style', $style_name);
    }
  }

  /**
   * Gets the default style name, if it has not been previously set it will be the first
   * name from self::GetNames().
   *
   * @return string
   *   The default style name
   */
  public static function GetDefaultName() {
    $default = variable_get('csl_default_style', array_shift(self::GetNames()));
    return $default;
  }

  /**
   * Gets the CSL style definition identified by name.
   *
   * @param string $style_name
   *   The name of the CSL style.
   * @param boolean $as_string
   *   Returns the CSL as a string (default is a DOMDocument)
   *
   * @return mixed
   *   The CSL style definition if found, FALSE otherwise.
   */
  public static function Get($style_name, $as_string=FALSE) {
    if (self::Exists($style_name)) {
      $query = db_query('SELECT style FROM {%s} WHERE name = "%s"', self::table, $style_name);
      $xml = db_result($query);
      $xml = trim($xml);
      if (!empty($xml)) {
        $definition = NULL;
        if (!$as_string) {
          $definition = new DOMDocument();
          $definition->loadXML($xml);
        }
        else {
          $definition = $xml;
        }
        return $definition;
      }
    }
    return FALSE;
  }

  /**
   * Gets the default style.
   * 
   * @param boolean $as_string
   *   Return a string if TRUE, a DOMDocument if FALSE.
   * @return mixed
   *   Returns teh default style as a string or DOMDocument depending on the $as_string argument.
   */
  public static function GetDefault($as_string = FALSE) {
    $style_name = self::GetDefaultName();
    return self::Get($style_name, $as_string);
  }

  /**
   * Creates a CSL style with the given style name and definition.
   *
   * If the CSL style already exists it will fail.
   *
   * @param string $style_name
   *   The name of the CSL style.
   * @param mixed $definition
   *   A CSL style definition. (DOMDocument or string)
   *
   * @return boolean
   *  TRUE if successful, otherwise FALSE.
   */
  public static function Create($style_name, $definition) {
    if (!self::Exists($style_name)) {
      if (!self::Valid($definition)) {
        drupal_set_message("The $style_name is not valid CSL 1.0 problems may occur.", 'warning');
      }
      $object = new stdClass();
      $object->name = $style_name;
      if ($definition instanceof DOMDocument) {
        $object->style = $definition->saveXML();
      }
      else { //Assume a string.
        $object->style = trim($definition);
      }
      return drupal_write_record(self::table, $object) !== FALSE;
    }
    return FALSE;
  }

  /**
   * Deletes the style with the given name.
   *
   * @param string $style_name
   *   The name of the CSL style definition.
   *
   * @return boolean
   *   TRUE if successful FALSE otherwise.
   */
  public static function Delete($style_name) {
    if (self::Exists($style_name)) {
      db_query('DELETE FROM {%s} WHERE name = "%s"', self::table, $style_name);
      return TRUE;
    }
    return FALSE;
  }

}
