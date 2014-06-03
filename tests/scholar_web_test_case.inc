<?php

/**
 * @file
 * DrupalWebTestCase customizations for the scholar module tests.
 */

class IslandoraScholarWebTestCase extends IslandoraCollectionWebTestCase {

  /**
   * The words of Robert Frost's "Stopping By Woods On A Snowy Evening".
   *
   * This is  exact output of the FULL_TEXT datastream created when ingesting
   * the test PDF file in the fixtures folder. Some of the tests check the
   * contents of this datastream, so it's just easier to keep it here.
   *
   * @var string
   */
  protected $stoppingByWoods = <<<POEM
Whose woods these are I think I know.
His house is in the village though;
He will not see me stopping here
To watch his woods fill up with snow.
My little horse must think it queer
To stop without a farmhouse near
Between the woods and frozen lake
The darkest evening of the year.
He gives his harness bells a shake
To ask if there is some mistake.
The only other sound’s the sweep
Of easy wind and downy flake.
The woods are lovely, dark and deep.
But I have promises to keep,
And miles to go before I sleep,
And miles to go before I sleep.
POEM;

  /**
   * Make sure islandora_scholar and collections are always enabled.
   *
   * @see IslandoraWebTestCase::setUp()
   */
  public function setUp() {
    $args = func_get_args();
    $args = (isset($args[0]) && is_array($args[0])) ? $args[0] : $args;
    // Always enable certain modules.
    $args[] = 'islandora_scholar';
    $args[] = 'islandora_basic_collection';
    parent::setUp($args);
  }

  /**
   * Creates a scholar object.
   *
   * @XXX: The scholar object is currently being generated outside of the Tuque
   * wrapper to prevent Islandora from attempting to generate derivatives for
   * the PDF datastream; this is done because of a bug in SimpleTest with batch
   * processed executing out of order (see https://drupal.org/node/2265599; I'm
   * sure they'll get to it someday ...).
   *
   * @param string $cmodel
   *   The content model of the object to create.
   * @param bool $add_metadata
   *   Whether or not we should add the test MODS/DC to the form and re-index.
   * @param bool $add_pdf
   *   Whether or not we should try to append a PDF to the object after ingest.
   *   Note that if you add a PDF, the metadata is going to get added whether
   *   you like it or not.
   * @param bool $full_text
   *   Whether or not a FULL_TEXT datastream should be added to a potential PDF.
   * @param bool $validate
   *   Whether or not to validate datastreams after PDF ingest.
   *
   * @return IslandoraFedoraObject
   *   The generated object.
   */
  protected function createScholarObject($cmodel, $add_metadata = FALSE, $add_pdf = FALSE, $full_text = FALSE, $validate = TRUE) {
    // Basic properties of the object.
    $random_name = $this->randomName();
    $module_path = drupal_get_path('module', 'islandora_scholar');
    $properties = array(
      'pid' => "islandora:$random_name",
      'models' => $cmodel,
    );

    // Include metadata if we were asked to.
    $datastreams = array();
    if ($add_metadata) {
      $datastreams = array(
        array(
          'dsid' => 'MODS',
          'control_group' => 'X',
          'mimetype' => 'text/xml',
          'path' => "$module_path/tests/fixtures/mods.xml",
        ),
        array(
          'dsid' => 'DC',
          'control_group' => 'X',
          'mimetype' => 'text/xml',
          'path' => "$module_path/tests/fixtures/dc.xml",
        ),
      );
    }

    // Add in a PDF datastream if we were asked to.
    if ($add_pdf) {
      $datastreams[] = array(
        'dsid' => 'PDF',
        'control_group' => 'M',
        'mimetype' => 'application/pdf',
        'path' => "$module_path/tests/fixtures/test.pdf",
      );
    }

    // Ingest the actual object.
    $object = $this->ingestConstructedObject($properties, $datastreams);

    // For everything below here, right up until the point where we return the
    // object, we confirm whether or not the object was actually generated (i.e.
    // whether it doesn't return FALSE). If the object wasn't created, we don't
    // want to waste our time trying to make derivatives, index metadata, etc...
    if ($object) {
      // Run derivative creation for the PDF.
      if ($add_pdf) {
        module_load_include('inc', 'islandora_scholar', 'includes/upload.tab');
        module_load_include('inc', 'islandora_scholar', 'includes/derivatives');
        islandora_scholar_add_tn_derivative($object, TRUE);
        islandora_scholar_add_preview_derivative($object, TRUE);
        islandora_scholar_add_usage_and_version_elements_to_mods($object, 'author', 'PRE-PUBLICATION');
        if ($full_text) {
          islandora_scholar_add_fulltext_derivative($object);
        }
      }

      // Index the metadata in to fedoragsearch with a quick REST request.
      if ($add_metadata || $add_pdf) {
        drupal_http_request("http://{$this->configuration['admin_user']}:{$this->configuration['admin_pass']}@localhost:8080/fedoragsearch/rest?operation=updateIndex&action=fromPid&value={$object->id}");
      }

      // Validate if we were asked to, but only if a PDF was added.
      if ($add_pdf && $validate) {
        // Verify that the datastreams we wanted were created.
        $datastreams = array(
          'PDF',
          'TN',
          'PREVIEW',
        );
        if ($full_text) {
          $datastreams[] = 'FULL_TEXT';
        }
        $this->assertDatastreams($object, $datastreams);

        // Validate the created datastreams.
        $datastreams = array(
          array('TN', 'Image'),
          array('PDF', 'PDF'),
          array('PREVIEW', 'Image'),
        );
        if ($full_text) {
          $datastreams[] = array(
            'FULL_TEXT',
            'Text',
            array($this->stoppingByWoods, 1),
          );
        }
        $this->validateDatastreams($object, $datastreams);
      }
    }

    // Finally, return the object that we just worked with.
    return $object;
  }

  /**
   * Deletes a PDF through the interface.
   *
   * @param IslandoraFedoraObject $object
   *   The object to delete the PDF from.
   *
   * @return bool
   *   FALSE if things blow up.
   */
  protected function deletePDF($object) {
    // Is it even a citation object?
    if (in_array('islandora:citationCModel', $object->models) xor in_array('islandora:thesisCModel', $object->models)) {
      $this->fail("Cannot add PDF; object {$object->id} is not a scholar object.", 'Islandora');
      return FALSE;
    }
    // Is it already missing a PDF?
    if (!isset($object['PDF'])) {
      $this->fail("Cannot remove PDF; no PDF exists for object {$object->id}.", 'Islandora');
      return FALSE;
    }

    // Just click the 'delete' button. Check if it is gone.
    $this->drupalPost("islandora/object/{$object->id}/islandora_scholar_upload", array(), 'Delete Document/PDF');
    $datastreams = array(
      'TN',
      'PDF',
      'PREVIEW',
      'FULL_TEXT',
    );
    $this->assertNoDatastreams($object, $datastreams);
  }

  /**
   * Adds the MLA CSL style to the CSL style list and switches to it.
   *
   * @param bool $verify
   *   Triggers whether to verify successful addition.
   *
   * @return bool|void
   *   TRUE/FALSE depending on pass/fail. TRUE always if no verification was
   *   asked for.
   */
  protected function addMLACSLStyle($verify = TRUE) {
    // Add the MLA CSL style.
    $edit = array(
      'name' => 'MLA',
      'files[file]' => drupal_get_path('module', 'islandora_scholar') . "/tests/fixtures/mla.csl",
    );
    $this->drupalPost('admin/islandora/solution_pack_config/csl', $edit, 'Add');

    // Verify, if we were asked.
    if ($verify) {
      if (!$this->assertNoError() || !$this->assertNoWarning()) {
        $this->fail("Error encountered while attempting to upload the MLA CSL style.");
        return FALSE;
      }
    }

    // If things turn out okay, or if we weren't asked to verify, set the CSL
    // style variable.
    variable_set('csl_default_style', 'MLA');
    return TRUE;
  }

  /**
   * Contcatenates user creation and login for cleaner-looking permission tests.
   *
   * @return object
   *   The user that was created, which we need for a couple of tests.
   */
  protected function createUserAndLogin(array $permissions) {
    // Scholar embargo permissions are dependent on one or more of these.
    $standard_permission_set = array(
      ISLANDORA_VIEW_OBJECTS,
      ISLANDORA_ADD_DS,
      ISLANDORA_METADATA_EDIT,
    );

    // Mash together the permissions, create a user, and log in.
    $permissions = array_merge($standard_permission_set, $permissions);
    $user = $this->drupalCreateUser($permissions);
    $this->drupalLogin($user);
    return $user;
  }
}
