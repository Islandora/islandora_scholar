<?php

/**
 * @file
 *
 * Class that encompases the Bibutils tools.
 */
class Bibutils {

  /**
   * A table where the keys are the source file types and their value is an array where 
   * the destination file type is the keys and the required command the value.
   *  
   * @var array
   */
  private static $conversionTable = array(
    'EndNoteXML' => array(
      'MODS' => 'endx2xml -i unicode',
    ),
    'RIS' => array(
      'MODS' => 'ris2xml -i unicode',
    ),
    'MODS' => array(
      'RIS' => 'xml2ris',
    )
  );

  /**
   * Tests if the file format is convertable.
   * 
   * @param string $type 
   * 
   * @return boolean
   *   TRUE if convertiable by Bibutils FALSE otherwise.
   */
  private static function IsConvertable($type) {
    return array_key_exists($type, self::$conversionTable);
  }

  /**
   * Gets the Bibutil Command for performing a conversion between the given file format types, if possible.
   * 
   * @param string $src_format
   *   The file format of the source document.
   * @param string $dest_format 
   *   The file format the source document will be converted into.
   *
   * @return string
   *   The command that will perform the conversion if possible FALSE otherwise.
   */
  private static function GetCommand($src_format, $dest_format) {
    $conversion = isset(self::$conversionTable[$src_format]) ? self::$conversionTable[$src_format] : array();
    if (isset($conversion[$dest_format])) {
      return $conversion[$dest_format];
    }
    return FALSE;
  }

  /**
   * Bibutils doesn't produce valid MODS, this function cleans up the generated mods and makes it valid.
   * 
   * @param string $filename
   *   The name of the file to clean.
   * 
   * @return boolean
   *   TRUE if succesful FALSE otherwise.
   */
  public static function CleanModsFile($filename) {
    $transform_filename = drupal_get_path('module', 'bibutils') . '/xsl/FixBibutilsModsOutput.xsl';
    $transform = new DOMDocument();
    $transform->load($transform_filename);
    $doc = new DOMDocument();
    $doc->load($filename);
    if (isset($doc->documentElement)) {
      $xslt = new XSLTProcessor();
      $xslt->importStyleSheet($transform);
      $doc = $xslt->transformToDoc($doc->documentElement);
      return $doc->save($filename) !== FALSE;
    }
  }

  /**
   * Executes the given command and the exit status and output of the command.
   * 
   * @param string $command
   *   The command to execute.
   * 
   * @return array
   *   Where the first parameter was the exit status of the command and the second is the commands output.
   */
  private static function Execute($command) {
    $return_value = FALSE;
    ob_start();
    passthru($command, $return_value);
    $output = ob_get_contents();
    ob_end_clean();
    return array($return_value, $output);
  }

  /**
   * Converts the given file to the destination format, and saves it to the given destination.
   *  
   * @param string $src
   *   The filename of the source file.
   * @param string $src_format
   *   The format of the source file.
   * @param string $dest
   *   The filename of the destination file.
   * @param string $dest_format 
   *   The format of the destination file.
   * 
   * @return boolean
   *   TRUE if successful, FALSE otherwise.
   */
  public static function Convert($src, $src_format, $dest, $dest_format) {
    $command = self::GetCommand($src_format, $dest_format);
    if ($command) {
      $command = "$command $src 2>&- 1> $dest";
      module_invoke_all('preprocess_bibutils_convert', $src, $src_format, $dest_format);
      list($exit_status, $output) = self::Execute($command);
      if ($exit_status == 0) {
        $results = module_invoke_all('postprocess_bibutils_convert', $src, $src_format, $dest, $dest_format);
        error_log(print_r($results, TRUE));
        return!in_array(FALSE, $results, TRUE); // No post process function returned false.
      }
    }
    return FALSE;
  }

}