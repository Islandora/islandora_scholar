<?php

/**
 * @file
 * A generalized testing suite for scholar importers.
 */

class IslandoraScholarImporterTestCase extends IslandoraScholarWebTestCase {

  /**
   * @var string
   * The PID of the collection created for each test.
   */
  protected $collection;

  /**
   * @var string
   * The name of the current module.
   */
  protected $moduleName;

  /**
   * Sets up the test.
   *
   * @see IslandoraScholarWebTestCase::setUp()
   */
  public function setUp() {
    parent::setUp(array($this->moduleName));
    $user = $this->drupalCreateUser(array_keys(module_invoke_all('permission')));
    $this->drupalLogin($user);
    $this->collection = "ir:" . $this->randomName();
    $this->createTestCollection($this->randomName(), 'ir:citationCModel', $this->collection);
  }

  /**
   * Returns a getInfo() array using $module_name and hook_islandora_importer().
   *
   * @param string $module
   *   The name of the module to get the info for.
   *
   * @return array
   *   The standard DrupalWebTestCase::getInfo() content.
   */
  public static function populateGetInfo($module) {
    // Haha procedurally generating getInfo()s, what a card.
    $importer = module_invoke($module, 'islandora_importer');
    $importer = $importer[$module]['title'];
    return array(
      'name' => "$importer Tests",
      'description' => "Tests the capabilities of the $importer.",
      'group' => 'Islandora Scholar',
    );
  }

  /**
   * Imports a file using a selected importer.
   *
   * @param array $import_edit
   *   An associative array of field data on the importer page to be used by its
   *   drupalPost() request, e.g. 'files[file]' => 'path/to/file' when importing
   *   a file, or 'textarea' => 'list of content' when importing objects by
   *   filling in values in the form.
   */
  protected function importObjects($import_edit) {
    $edit = array(
      'importer' => $this->moduleName,
    );
    $this->drupalPost("islandora/object/{$this->collection}/manage/collection/batch_import", $edit, "Submit");
    $this->drupalPost(NULL, $import_edit, "Import");
  }

  /**
   * Gets the PIDs of objects generated using the specified batch.
   *
   * @param int $set_id
   *   The batch ID. Passing 0 will grab results for the most recent batch.
   *
   * @return array|bool
   *   An associative array of PIDs, or FALSE if something went terribly wrong.
   */
  protected function getPIDSFromBatch($set_id = 0) {
    // If no batch ID was given, grab the most recent one.
    if ($set_id === 0) {
      $set_object = db_select('islandora_batch_set', 'i')
        ->fields('i')
        ->orderBy('created', 'DESC')
        ->execute()
        ->fetchObject();
      if (!is_object($set_object)) {
        $this->fail("Error: No batch sets found.", 'Islandora');
        return FALSE;
      }
      $set_id = $set_object->id;
    }

    // Get them there PIDs eh b'y.
    $results = db_select('islandora_batch_queue', 'i')
      ->fields('i', array('id'))
      ->condition('sid', $set_id, '=')
      ->orderBy('id', 'ASC')
      ->execute()
      ->fetchCol();
    if (empty($results)) {
      $this->fail("Error: no results found for set ID $set_id", 'Islandora');
      return FALSE;
    }
    return $results;
  }

  /**
   * Checks a list of PIDs against what their MODS metadata should look like.
   *
   * @param array $pid_list
   *   An associative array of PIDs => paths to files that contain expected MODS
   *   metadata those PIDs should have as MODS datastreams.
   * @param bool $is_pmid
   *   If this is a batch of Pubmed records, it's time to gouge out the date and
   *   time!
   */
  protected function checkMODSAgainstExpected($pid_list, $is_pmid = FALSE) {
    foreach ($pid_list as $pid => $mods_path) {
      $object = islandora_object_load($pid);
      // The is_object($object) assertion has already been taken care of by
      // assertBatchImport().
      if ($object !== FALSE) {
        if (isset($object['MODS'])) {
          $expected_mods = file_get_contents($mods_path);
          $actual_mods = $object['MODS']->content;
          // PMID imported MODS includes a timestamp that designates when the
          // MODS was generated. Rather than trying to divine the exact value
          // of the timestamp, it's easier to just surgically remove it.
          if ($is_pmid) {
            $actual_mods = $this->removeTimeStampFromPMID($actual_mods);
            if (!$actual_mods) {
              $this->fail("Failed to remove the timestamp from the imported PubMed MODS metadata. This likely points to issues with the imported MODS.");
              // If we couldn't even do this, let's not bother trying to assess
              // the imported MODS.
              return;
            }
          }
          $this->assertEqual($expected_mods, $actual_mods, "Imported MODS metadata from object $pid matches expected MODS metadata.", 'Islandora');
        }
        else {
          $this->fail("Object $object contains no MODS datastream.");
        }
      }
    }
  }

  /**
   * Removes the timestamp from an imported PMID record.
   *
   * @param string $pmid_mods
   *   The imported Pubmed MODS.
   *
   * @return string|bool
   *   The MODS, minus the timestamp. FALSE on failure.
   */
  protected function removeTimeStampFromPMID($pmid_mods) {
    $pmid_xml = simplexml_load_string($pmid_mods);
    $pmid_xml->registerXPathNamespace('mods', 'http://www.loc.gov/mods/v3');
    // Cut out the record info.
    list($record_info) = $pmid_xml->xpath('/mods:mods/mods:recordInfo/mods:recordCreationDate');
    // If unsetting this fails, just return FALSE.
    try {
      unset($record_info[0]);
    }
    catch (Exception $e) {
      return FALSE;
    }
    return $pmid_xml->asXML();
  }

  /**
   * Verifies the expected batch result count and expected MODS for each object.
   *
   * @param int $count
   *   The expected count of objects in the set.
   * @param int $set_id
   *   (Optional) The set ID to get the PIDS for. This might not actually get
   *   used, but I'm putting it here just in case. 0 grabs the most recent set.
   * @param bool $is_pmid
   *   If this is a batch of Pubmed records, we need to surgically remove the
   *   date and time from the retrieved MODS.
   */
  protected function assertBatchImport($count, $set_id = 0, $is_pmid = FALSE) {
    $pids = $this->getPIDSFromBatch();

    // First off, are there even the right number of objects in there? If not,
    // let's just stop here, because something's clearly gone terribly wrong.
    $actual = count($pids);
    if ($actual !== $count) {
      $this->fail("Expected count of $count objects from the batch was not met (actual: $actual).", 'Islandora');
      return;
    }

    // Second, do those PIDS lead to actual objects?
    foreach ($pids as $pid) {
      if (!islandora_object_load($pid)) {
        $this->fail("Object $pid was referenced in the batch set, but no such object was found to exist.", 'Islandora');
      }
    }

    // Third, let's assemble a list of PIDS and check against expected MODS.
    $pid_list = array();
    for ($i = 0; $i <= $count - 1; $i++) {
      $pid_list[$pids[$i]] = drupal_get_path('module', 'islandora_scholar') . "/tests/fixtures/{$this->moduleName}_mods_{$i}.xml";
    }
    $this->checkMODSAgainstExpected($pid_list, $is_pmid);

    // Lastly, let's go to the actual object's page and confirm no errors or
    // warnings when trying to display the imported record.
    foreach ($pids as $pid) {
      $this->drupalGet("islandora/object/$pid");
      $this->assertNoError();
      $this->assertNoWarning();
    }
  }

}
