<?php

/**
 * @file
 * Admin form for the islandora_scholar.module
 */

/**
 * Created a system form element that defaults to a textfield.
 *
 * @param string $name
 *   The name of the form element/variable.
 * @param mixed $default
 *   The default value for the given form element/variable.
 * @param array $form_controls
 *   Form controls for the form element.
 *
 * @return array
 *   A system form element definition.
 */
function _islandora_scholar_add_variable($name, $default = '', $form_controls = array()) {
  $form_controls += array(
    '#type' => 'textfield',
    '#title' => $name,
  );
  $form_controls['#default_value'] = variable_get($name, $default);
  return array(
    $name => $form_controls,
  );
}

/**
 * Defines the admin settings form.
 *
 * @return array
 *   Drupal form definition.
 */
function islandora_scholar_admin_form() {
  $form = array(
    'romeo' => array(
      '#type' => 'fieldset',
      '#title' => t('RoMEO configuration'),
      '#description' => t('Various parameters used by RoMEO'),
      '#collapsible' => TRUE,
    ),
    'google_scholar_search' => array(
      '#type' => 'fieldset',
      '#title' => t('Google Scholar Search configuration'),
      '#description' => t('Various parameters used by Google Scholar Search'),
      '#collapsible' => TRUE,
    ),
    'library_catalog_search' => array(
      '#type' => 'fieldset',
      '#title' => t('Library Catalog Search configuration'),
      '#description' => t('Various parameters used by your Library Catalog'),
      '#collapsible' => TRUE,
    ),
  );

  $form['google_scholar_search'] += _islandora_scholar_add_variable('islandora_scholar_google_scholar_search_enabled', FALSE, array(
    '#type' => 'checkbox',
    '#title' => t('Render Google Scholar Search link'),
    '#description' => t('Enable Searching in Google Scholar.'),
    '#required' => TRUE,
  ));
  $form['google_scholar_search'] += _islandora_scholar_add_variable('islandora_scholar_google_scholar_primary_search_xpath', '//mods:identifier[@type="doi"]', array(
    '#type' => 'textfield',
    '#title' => t('Google Scholar Primary Search XPath'),
    '#description' => t('The primary xpath you want to use to search.'),
    '#states' => array(
      'visible' => array(
        ':input[name="islandora_scholar_google_scholar_search_enabled"]' => array('checked' => TRUE),
      ),
    ),
  ));
  $form['google_scholar_search'] += _islandora_scholar_add_variable('islandora_scholar_google_scholar_default_search_xpath', "//mods:mods[1]/mods:titleInfo/mods:title", array(
    '#type' => 'textfield',
    '#title' => t('Google Scholar Default Search XPath'),
    '#description' => t('The default xpath you want to use to search'),
    '#states' => array(
      'visible' => array(
        ':input[name="islandora_scholar_google_scholar_search_enabled"]' => array('checked' => TRUE),
      ),
    ),
  ));
  $form['library_catalog_search'] += _islandora_scholar_add_variable('islandora_scholar_library_catalog_search_enabled', FALSE, array(
    '#type' => 'checkbox',
    '#title' => t('Render Library Catalog Search link'),
    '#description' => t('Enable Searching in your library discovery layer.'),
  ));
  $form['library_catalog_search'] += _islandora_scholar_add_variable('islandora_scholar_library_catalog_search_URL', '', array(
    '#type' => 'textfield',
    '#title' => t('Library Catalog Location'),
    '#description' => t('The search URL for your library discovery layer. Example: https://yoursite.discoveryservice.serviceprovider.com'),
    '#states' => array(
      'visible' => array(
        ':input[name="islandora_scholar_library_catalog_search_enabled"]' => array('checked' => TRUE),
      ),
    ),
  ));
  $form['library_catalog_search'] += _islandora_scholar_add_variable('islandora_scholar_library_catalog_search_params', '/search?s.q=', array(
    '#type' => 'textfield',
    '#title' => t('Library Search Parameters'),
    '#description' => t('The search parameters for your library discovery layer. Default configuration for Summon discovery layer is shown.'),
    '#states' => array(
      'visible' => array(
        ':input[name="islandora_scholar_library_catalog_search_enabled"]' => array('checked' => TRUE),
      ),
    ),
  ));
  $form['library_catalog_search'] += _islandora_scholar_add_variable('islandora_scholar_library_catalog_primary_search_xpath', '//mods:identifier[@type="doi"]', array(
    '#type' => 'textfield',
    '#title' => t('Library Catalog Primary Search XPath'),
    '#description' => t('The primary xpath you want to use to search.'),
    '#states' => array(
      'visible' => array(
        ':input[name="islandora_scholar_library_catalog_search_enabled"]' => array('checked' => TRUE),
      ),
    ),
  ));
  $form['library_catalog_search'] += _islandora_scholar_add_variable('islandora_scholar_library_catalog_default_search_xpath', '//mods:title', array(
    '#type' => 'textfield',
    '#title' => t('Library Catalog Default Search XPath'),
    '#description' => t('The default xpath you want to use to search'),
    '#states' => array(
      'visible' => array(
        ':input[name="islandora_scholar_library_catalog_search_enabled"]' => array('checked' => TRUE),
      ),
    ),
  ));

  $form['romeo'] += _islandora_scholar_add_variable('islandora_scholar_romeo_enable', FALSE, array(
    '#type' => 'checkbox',
    '#title' => t('Enable RoMEO attempts.'),
    '#description' => t('Whether or not we should even try getting information from RoMEO.'),
  ));
  $form['romeo'] += _islandora_scholar_add_variable('islandora_scholar_romeo_enable', FALSE, array(
    '#type' => 'checkbox',
    '#title' => t('Enable RoMEO attempts.'),
    '#description' => t('Whether or not we should even try getting information from RoMEO.'),
  ));

  $form['romeo'] += _islandora_scholar_add_variable('islandora_scholar_romeo_key', '', array(
    '#title' => t('Sherpa/RoMEO API Key'),
    '#description' => t("Your institution's key to access RoMEO."),
  ));
  $form['romeo'] += _islandora_scholar_add_variable('islandora_scholar_issn_cache_time', 2, array(
    '#title' => t('ISSN cache time'),
    '#description' => t('The amount of time in days for which to maintain an ISSN value obtained by scanning an object.'),
  ));
  $form['romeo'] += _islandora_scholar_add_variable('islandora_scholar_romeo_cache_time', 2, array(
    '#title' => t('RoMEO cache time'),
    '#description' => t('The amount of time in days for which to maintain a copy of the markup for a given ISSN.'),
  ));

  $form['islandora_scholar_use_standard_metadata_display'] = array(
    '#title' => t('Use Standard Metadata Display'),
    '#description' => t('Check to use standard Islandora metadata display instead of COinS.'),
    '#type' => 'checkbox',
    '#default_value' => variable_get('islandora_scholar_use_standard_metadata_display', FALSE),
  );

  $form['islandora_scholar_users_choose_display_csl'] = array(
    '#title' => t('Let users choose display CSL'),
    '#description' => t('In object view let the users choose which CSL to use.'),
    '#type' => 'checkbox',
    '#default_value' => variable_get('islandora_scholar_users_choose_display_csl', FALSE),
  );

  $form['islandora_scholar_specify_document_versions'] = array(
    '#type' => 'checkbox',
    '#title' => t('Specify Document Versions'),
    '#description' => t('Use your own document versions when uploading a PDF'),
    '#default_value' => variable_get('islandora_scholar_specify_document_versions', FALSE),
  );

  $form['islandora_scholar_document_versions'] = array(
    '#type' => 'textarea',
    '#title' => t('Document Versions'),
    '#description' => t('Enter the document versions you would like to choose from. Enter one choice per line.'),
    '#states' => array(
      'visible' => array(
        ':input[name="islandora_scholar_specify_document_versions"]' => array('checked' => TRUE),
      ),
    ),
    '#default_value' => variable_get('islandora_scholar_document_versions', "Author's Original\nSubmitted Manuscript Under Review\nAccepted Manuscript\nProof\nVersion of Record\nCorrected Version of Record\nEnhanced Version of Record"),
  );

  $form['islandora_scholar_specify_use_permissions'] = array(
    '#type' => 'checkbox',
    '#title' => t('Specify Use Permissions'),
    '#description' => t('Use your own use permissions when uploading a PDF'),
    '#default_value' => variable_get('islandora_scholar_specify_use_permissions', FALSE),
  );

  $form['islandora_scholar_use_permissions'] = array(
    '#type' => 'textarea',
    '#title' => t('Use Permissions'),
    '#description' => t('Enter the use permissions you would like to choose from. Enter one choice per line.'),
    '#states' => array(
      'visible' => array(
        ':input[name="islandora_scholar_specify_use_permissions"]' => array('checked' => TRUE),
      ),
    ),
    '#default_value' => variable_get('islandora_scholar_use_permissions', "Publisher\nAuthor"),
  );

  module_load_include('inc', 'islandora', 'includes/solution_packs');
  $form += islandora_viewers_form('islandora_scholar_viewers', 'application/pdf');

  return system_settings_form($form);
}

/**
 * Defines the Islandora Scholar PDF derivative options form.
 *
 * @param array $form
 *   An array representing a Drupal form.
 * @param array $form_state
 *   An array representing the Drupal form state.
 *
 * @return array
 *   An array representing a Drupal form.
 */
function islandora_scholar_admin_pdf_form($form, &$form_state) {
  form_load_include($form_state, 'inc', 'islandora_scholar', 'includes/admin.form');
  // @TODO: Consolidate all this into derivatives library code so we don't
  // have similar code within PDF and Scholar.
  if (isset($form_state['values']['islandora_scholar_path_to_pdftotext'])) {
    $islandora_scholar_path_to_pdftotext = $form_state['values']['islandora_scholar_path_to_pdftotext'];
  }
  else {
    $islandora_scholar_path_to_pdftotext = variable_get('islandora_scholar_path_to_pdftotext', '/usr/bin/pdftotext');
  }
  exec($islandora_scholar_path_to_pdftotext, $output, $return_value);
  $confirmation_message = '<img src="' . url('misc/watchdog-ok.png') . '"/>'
    . t('pdftotext executable found at !url', array('!url' => "<strong>$islandora_scholar_path_to_pdftotext</strong>"));
  if ($return_value != 99) {
    $confirmation_message = '<img src="' . url('misc/watchdog-error.png') . '"/> '
      . t('Unable to find pdftotext executable at !url', array('!url' => "<strong>$islandora_scholar_path_to_pdftotext</strong>"));
  }

  $form = array();

  // AJAX wrapper for url checking.
  $form['islandora_scholar_url_fieldset'] = array(
    '#type' => 'fieldset',
    '#title' => t('TEXT'),
  );

  $form['islandora_scholar_url_fieldset']['islandora_scholar_create_fulltext'] = array(
    '#type' => 'checkbox',
    '#title' => t("Extract text streams from PDFs using pdftotext"),
    '#description' => t(
      "The extracted text stream is added as FULL_TEXT datastreams and are indexed into Solr. Uploading a text file takes priority over text stream extraction.
      </br><strong>Note:</strong> PDFs that contain visible text do not necessarily contain text streams (e.g. images scanned and saved as PDFs). Consider converting text-filled images with no text streams to TIFFs and using the !book with !ocr enabled.",
      array(
        '!book' => l(t('Book Solution Pack'), 'https://wiki.duraspace.org/display/ISLANDORA711/Book+Solution+Pack'),
        '!ocr' => l(t('OCR'), 'https://wiki.duraspace.org/display/ISLANDORA711/Islandora+OCR'),
      )
    ),
    '#default_value' => variable_get('islandora_scholar_create_fulltext', TRUE),
  );

  $form['islandora_scholar_url_fieldset']['wrapper'] = array(
    '#prefix' => '<div id="islandora-url">',
    '#suffix' => '</div>',
    '#type' => 'markup',
  );
  $form['islandora_scholar_url_fieldset']['wrapper'] = array(
    '#prefix' => '<div id="islandora-url">',
    '#suffix' => '</div>',
    '#type' => 'markup',
  );

  $form['islandora_scholar_url_fieldset']['wrapper']['islandora_scholar_path_to_pdftotext'] = array(
    '#type' => 'textfield',
    '#title' => t('Path to pdftotext executable'),
    '#default_value' => $islandora_scholar_path_to_pdftotext,
    '#description' => t('!confirmation_message',
      array(
        '!confirmation_message' => $confirmation_message)
    ),
    '#ajax' => array(
      'callback' => 'islandora_scholar_update_pdftotext_url_div',
      'wrapper' => 'islandora-url',
      'effect' => 'fade',
      'event' => 'blur',
      'progress' => array('type' => 'throbber'),
    ),
    '#states' => array(
      'visible' => array(
        ':input[name="islandora_scholar_create_fulltext"]' => array('checked' => TRUE),
      ),
    ),
  );

  $form['islandora_scholar_thumbnail_fieldset'] = array(
    '#type' => 'fieldset',
    '#title' => t('Thumbnail'),
    '#description' => t('Settings for creating PDF thumbnail derivatives'),
  );

  $form['islandora_scholar_thumbnail_fieldset']['islandora_scholar_thumbnail_width'] = array(
    '#type' => 'textfield',
    '#title' => t('Width'),
    '#description' => t('The width of the thumbnail in pixels.'),
    '#element_validate' => array('element_validate_number'),
    '#default_value' => variable_get('islandora_scholar_thumbnail_width', 200),
    '#size' => 5,
  );

  $form['islandora_scholar_thumbnail_fieldset']['islandora_scholar_thumbnail_height'] = array(
    '#type' => 'textfield',
    '#title' => t('Height'),
    '#description' => t('The height of the thumbnail in pixels.'),
    '#element_validate' => array('element_validate_number'),
    '#default_value' => variable_get('islandora_scholar_thumbnail_height', 200),
    '#size' => 5,
  );

  $form['islandora_scholar_preview_fieldset'] = array(
    '#type' => 'fieldset',
    '#title' => t('Preview image'),
    '#description' => t('Settings for creating PDF preview image derivatives'),
  );

  $form['islandora_scholar_preview_fieldset']['islandora_scholar_preview_width'] = array(
    '#type' => 'textfield',
    '#title' => t('Max width'),
    '#description' => t('The maximum width of the preview in pixels.'),
    '#element_validate' => array('element_validate_number'),
    '#default_value' => variable_get('islandora_scholar_preview_width', 500),
    '#size' => 5,
  );

  $form['islandora_scholar_preview_fieldset']['islandora_scholar_preview_height'] = array(
    '#type' => 'textfield',
    '#title' => t('Max height'),
    '#description' => t('The maximum height of the preview in pixels.'),
    '#element_validate' => array('element_validate_number'),
    '#default_value' => variable_get('islandora_scholar_preview_height', 700),
    '#size' => 5,
  );
  $form['islandora_scholar_preview_fieldset']['islandora_scholar_preview_density'] = array(
    '#type' => 'textfield',
    '#title' => t('Density used to generate preview'),
    '#description' => t('The imagemagick density value used to generate the preview. Generally, higher numbers are better but require more computation time.'),
    '#element_validate' => array('element_validate_number'),
    '#default_value' => variable_get('islandora_scholar_preview_density', 72),
    '#size' => 5,
  );
  return system_settings_form($form);
}

/**
 * Defines the admin xpath settings form.
 *
 * @return array
 *   Drupal form definition.
 */
function islandora_scholar_admin_xpath_form($form, &$form_state) {
  $form = array(
    'islandora_scholar_xpaths' => array(
      '#type' => 'fieldset',
      '#title' => t('Islandora Scholar Xpath configuration'),
      '#description' => t('Set custom XPaths for Islandora Scholar. Islandora-Collaboration-Group: <a href="https://github.com/Islandora-Collaboration-Group/icg_csv_import/blob/master/README.md#xpaths-and-keys" title="Description and breakdown of XPaths and Keys">Descriptions and breakdown of XPaths and Keys</a><br/><a href="https://github.com/Islandora/islandora_scholar/" title="Additional information at Github/Islandora Scholar">Additional information</a><br/>
        <p>Additional XPath Configurations<br/>
        <a href="admin/islandora/solution_pack_config/scholar#edit-google-scholar-search" title="Google Scholar Search XPaths Configuration Page">Google Scholar Search XPaths</a> &
        <a href="admin/islandora/solution_pack_config/scholar#edit-library-catalog-search" title="Library Catalog Search XPaths Configuration Page">Library Catalog Search XPaths</a><br/></p>
        <br/>'),
    ),
    'islandora_scholar_xpaths_demo' => array(
      '#type' => 'fieldset',
      '#title' => t('Object to Test values with'),
      '#description' => t('Check your values against an existing object via a PID'),
      '#collapsible' => TRUE,
      'access callback' => TRUE,
      '#states' => array(
        'visible' => array(
          ':input[name="islandora_scholar_xpaths_enabled"]' => array('checked' => TRUE),
        ),
      ),
    ),
    'islandora_scholar_xpaths_general' => array(
      '#type' => 'fieldset',
      '#title' => t('General Xpath configurations'),
      '#description' => check_plain('These XPaths are shared within most of the submodules.' . (!empty($form_state['values']['islandora_scholar_xpaths_demo']) ? "Testing again pid: <span style='color:red'>" . $form_state['values']['islandora_scholar_xpaths_demo'] . "</span>" : "")),
      '#collapsible' => TRUE,
      '#states' => array(
        'visible' => array(
          ':input[name="islandora_scholar_xpaths_enabled"]' => array('checked' => TRUE),
        ),
      ),
      '#prefix' => '<div id="islandora_scholar_xpaths_general_title">',
      '#suffix' => '</div>',
    ),
    'islandora_scholar_xpaths_google' => array(
      '#type' => 'fieldset',
      '#title' => t('Google Scholar Submodule Xpath configurations'),
      '#description' => check_plain('Not limited to Google Scholar, most of these are used by Coins and Citeproc as well.' . (!empty($form_state['values']['islandora_scholar_xpaths_demo']) ? "Testing again pid: <span style='color:red'>" . $form_state['values']['islandora_scholar_xpaths_demo'] . "</span>" : "")),
      '#collapsible' => TRUE,
      '#states' => array(
        'visible' => array(
          ':input[name="islandora_scholar_xpaths_enabled"]' => array('checked' => TRUE),
        ),
      ),
      '#prefix' => '<div id="islandora_scholar_xpaths_google_title">',
      '#suffix' => '</div>',
    ),
    'islandora_scholar_xpaths_coins' => array(
      '#type' => 'fieldset',
      '#title' => t('Coins Submodule Xpath configurations'),
      '#description' => check_plain('XPaths for the construction of COINS URLs.' . (!empty($form_state['values']['islandora_scholar_xpaths_demo']) ? "Testing again pid: <span style='color:red'>" . $form_state['values']['islandora_scholar_xpaths_demo'] . "</span>" : "")),
      '#collapsible' => TRUE,
      '#states' => array(
        'visible' => array(
          ':input[name="islandora_scholar_xpaths_enabled"]' => array('checked' => TRUE),
        ),
      ),
      '#prefix' => '<div id="islandora_scholar_xpaths_coins_title">',
      '#suffix' => '</div>',
    ),
    'islandora_scholar_xpaths_citeproc' => array(
      '#type' => 'fieldset',
      '#title' => t('Citeproc Submodule Xpath configurations'),
      '#description' => check_plain('MODS to Citeproc JSON Submodule Converter Xpath configurations' . (!empty($form_state['values']['islandora_scholar_xpaths_demo']) ? " Testing again pid: <span style='color:red'>" . $form_state['values']['islandora_scholar_xpaths_demo'] . "</span>" : "")),
      '#collapsible' => TRUE,
      '#states' => array(
        'visible' => array(
          ':input[name="islandora_scholar_xpaths_enabled"]' => array('checked' => TRUE),
        ),
      ),
      '#prefix' => '<div id="islandora_scholar_xpaths_citeproc_title">',
      '#suffix' => '</div>',
    ),
  );
  $form['islandora_scholar_xpaths_demo'] = array(
    '#type' => 'textfield',
    '#title' => t('Demo Object'),
    '#field_suffix' => ((!empty($form_state['values']['islandora_scholar_xpaths_demo'])) && (islandora_object_access(ISLANDORA_VIEW_OBJECTS, islandora_object_load($form_state['values']['islandora_scholar_xpaths_demo'])) == 1 ) ? t("<span style='color:green'>Successfully loaded PID!</span>") : t("No PID entered to verify MODS validity.")),
    '#description' => t('PID to check these values against.' . ((!empty($form_state['values']['islandora_scholar_xpaths_demo'])) && (islandora_object_access(ISLANDORA_VIEW_OBJECTS, islandora_object_load($form_state['values']['islandora_scholar_xpaths_demo'])) == 1 ) ? "<br/><img src='/islandora/object/" . $form_state['values']['islandora_scholar_xpaths_demo'] . "/datastream/TN/view'>" : "")),
    '#states' => array(
      'visible' => array(
        ':input[name="islandora_scholar_xpaths_enabled"]' => array('checked' => TRUE),
      ),
    ),
    '#autocomplete_path' => 'islandora_compound_object/autocomplete/child',
    '#ajax' => array(
      'callback' => 'islandora_scholar_xpaths_image_callback',
      'wrapper' => 'demo_image',
    ),
    '#prefix' => '<div id="demo_image">',
    '#suffix' => '</div>',
  );
  $form['islandora_scholar_xpaths'] += _islandora_scholar_add_variable('islandora_scholar_xpaths_enabled', FALSE, array(
      '#type' => 'checkbox',
      '#title' => t('Enable Custom XPaths'),
      '#description' => t('Set custom XPaths to the element to update and read. Unchecking will result in a reset of all form values.'),
  ));
  $form['islandora_scholar_xpaths']['reset'] = array(
    '#type' => 'submit',
    '#value' => t('Reset all Xpaths to defaults'),
    '#submit' => array('_islandora_scholar_xpaths_reset'),
    '#description' => t('Reset will remove these global variables and rely solely on the hard coded XPaths.'),
    '#states' => array(
      'visible' => array(
        ':input[name="islandora_scholar_xpaths_enabled"]' => array('checked' => TRUE),
      ),
    ),
  );
  $form['islandora_scholar_xpaths_general'] += _islandora_scholar_add_variable('islandora_scholar_xpaths_title', '//mods:mods[1]/mods:titleInfo/mods:title', array(
    '#type' => 'textfield',
    '#title' => t('Title'),
    '#description' => t('Default Value: //mods:mods[1]/mods:titleInfo/mods:title <br/>XPath to use for title.'),
    '#states' => array(
      'visible' => array(
        ':input[name="islandora_scholar_xpaths_enabled"]' => array('checked' => TRUE),
      ),
    ),
    '#ajax' => array(
      'callback' => 'islandora_scholar_xpaths_general_callback',
      'wrapper' => 'islandora_scholar_xpaths_general_title',
    ),
   '#field_suffix' => (!empty($form_state['values']['islandora_scholar_xpaths_demo']) ? _mods_id($form_state['values']['islandora_scholar_xpaths_title'], t($form_state['values']['islandora_scholar_xpaths_demo'])) : t("No PID entered to verify MODS validity.")),
  ));
  $form['islandora_scholar_xpaths_general'] += _islandora_scholar_add_variable('islandora_scholar_xpaths_title_sub_title', '//mods:subTitle', array(
    '#type' => 'textfield',
    '#title' => t('Subtitle'),
    '#description' => t('Default Value: //mods:subTitle <br/>XPath for Coins submodule to construct the first entry in an array of subtitle entries.'),
    '#states' => array(
      'visible' => array(
        ':input[name="islandora_scholar_xpaths_enabled"]' => array('checked' => TRUE),
      ),
    ),
    '#ajax' => array(
      'callback' => 'islandora_scholar_xpaths_general_callback',
      'wrapper' => 'islandora_scholar_xpaths_general_title',
    ),
    '#field_suffix' => (!empty($form_state['values']['islandora_scholar_xpaths_demo']) ? _mods_id($form_state['values']['islandora_scholar_xpaths_title_sub_title'], t($form_state['values']['islandora_scholar_xpaths_demo'])) : t("No PID entered to verify MODS validity.")),
  ));
  $form['islandora_scholar_xpaths_general'] += _islandora_scholar_add_variable('islandora_scholar_xpaths_abstract', '//mods:mods[1]/mods:abstract', array(
    '#type' => 'textfield',
    '#title' => t('Abstract'),
    '#description' => t('Default Value: //mods:mods[1]/mods:abstract <br/>XPath to use for the abstract.'),
    '#states' => array(
      'visible' => array(
        ':input[name="islandora_scholar_xpaths_enabled"]' => array('checked' => TRUE),
      ),
    ),
    '#ajax' => array(
      'callback' => 'islandora_scholar_xpaths_general_callback',
      'wrapper' => 'islandora_scholar_xpaths_general_title',
    ),
    '#field_suffix' => (!empty($form_state['values']['islandora_scholar_xpaths_demo']) ? _mods_id($form_state['values']['islandora_scholar_xpaths_abstract'], t($form_state['values']['islandora_scholar_xpaths_demo'])) : t("No PID entered to verify MODS validity.")),
  ));
  $form['islandora_scholar_xpaths_general'] += _islandora_scholar_add_variable('islandora_scholar_xpaths_family', 'mods:namePart[@type = "family"]', array(
    '#type' => 'textfield',
    '#title' => t('Family Name'),
    '#description' => t("Default Value: mods:namePart[@type = 'family'] <br/>XPath to use for the family in the bibutils submodule.<br/>XML of the mods:name to the processed format: <name><namePart type='family'><namePart type='given'></name>"),
    '#states' => array(
      'visible' => array(
        ':input[name="islandora_scholar_xpaths_enabled"]' => array('checked' => TRUE),
      ),
    ),
    '#ajax' => array(
      'callback' => 'islandora_scholar_xpaths_general_callback',
      'wrapper' => 'islandora_scholar_xpaths_general_title',
    ),
    '#field_suffix' => (!empty($form_state['values']['islandora_scholar_xpaths_demo']) ? _mods_id($form_state['values']['islandora_scholar_xpaths_family'], t($form_state['values']['islandora_scholar_xpaths_demo'])) : t("No PID entered to verify MODS validity.")),
  ));
  $form['islandora_scholar_xpaths_general'] += _islandora_scholar_add_variable('islandora_scholar_xpaths_given', 'mods:namePart[@type = "given"]', array(
    '#type' => 'textfield',
    '#title' => t('Given Name'),
    '#description' => t("Default Value: mods:namePart[@type = 'given'] <br/>XPath to use for the given."),
    '#states' => array(
      'visible' => array(
        ':input[name="islandora_scholar_xpaths_enabled"]' => array('checked' => TRUE),
      ),
    ),
    '#ajax' => array(
      'callback' => 'islandora_scholar_xpaths_general_callback',
      'wrapper' => 'islandora_scholar_xpaths_general_title',
    ),
    '#field_suffix' => (!empty($form_state['values']['islandora_scholar_xpaths_demo']) ? _mods_id($form_state['values']['islandora_scholar_xpaths_title_sub_title'], t($form_state['values']['islandora_scholar_xpaths_demo'])) : t("No PID entered to verify MODS validity.")),
  ));
  $form['islandora_scholar_xpaths_general'] += _islandora_scholar_add_variable('islandora_scholar_xpaths_authors_xpath', '//mods:mods[1]/mods:name/mods:role[mods:roleTerm = "author"]/../mods:namePart[@type="family"]', array(
    '#type' => 'textfield',
    '#title' => t('Authors'),
    '#description' => t('Default Value: //mods:mods[1]/mods:name/mods:role[mods:roleTerm = "author"]/../mods:namePart[@type="family"]<br/>XPath to use for authors in the bibliography submodule for display.'),
    '#states' => array(
      'visible' => array(
        ':input[name="islandora_scholar_xpaths_enabled"]' => array('checked' => TRUE),
      ),
    ),
    '#ajax' => array(
      'callback' => 'islandora_scholar_xpaths_general_callback',
      'wrapper' => 'islandora_scholar_xpaths_general_title',
    ),
    '#field_suffix' => (!empty($form_state['values']['islandora_scholar_xpaths_demo']) ? _mods_id($form_state['values']['islandora_scholar_xpaths_authors_xpath'], t($form_state['values']['islandora_scholar_xpaths_demo'])) : t("No PID entered to verify MODS validity.")),
  ));
  $form['islandora_scholar_xpaths_general'] += _islandora_scholar_add_variable('islandora_scholar_xpaths_created_date', '//mods:originInfo/mods:dateCreated', array(
    '#type' => 'textfield',
    '#title' => t('Created Date'),
    '#description' => t('Default Value: //mods:originInfo/mods:dateCreated <br/>XPath to use for the created_date.'),
    '#states' => array(
      'visible' => array(
        ':input[name="islandora_scholar_xpaths_enabled"]' => array('checked' => TRUE),
      ),
    ),
    '#ajax' => array(
      'callback' => 'islandora_scholar_xpaths_general_callback',
      'wrapper' => 'islandora_scholar_xpaths_general_title',
    ),
    '#field_suffix' => (!empty($form_state['values']['islandora_scholar_xpaths_demo']) ? _mods_id($form_state['values']['islandora_scholar_xpaths_title_sub_title'], t($form_state['values']['islandora_scholar_xpaths_demo'])) : t("No PID entered to verify MODS validity.")),
  ));
  $form['islandora_scholar_xpaths_google'] += _islandora_scholar_add_variable('islandora_scholar_xpaths_degree_grantor', '//mods:mods/mods:name[@type="corporate"][mods:role/mods:roleTerm = "Degree grantor"]/mods:namePart', array(
    '#type' => 'textfield',
    '#title' => t('Degree Grantor'),
    '#description' => t('Default Value: //mods:mods/mods:name[@type="corporate"][mods:role/mods:roleTerm = "Degree grantor"]/mods:namePart <br/>XPath to use for degree grantor.'),
    '#states' => array(
      'visible' => array(
        ':input[name="islandora_scholar_xpaths_enabled"]' => array('checked' => TRUE),
      ),
    ),
    '#ajax' => array(
      'callback' => 'islandora_scholar_xpaths_google_callback',
      'wrapper' => 'islandora_scholar_xpaths_google_title',
    ),
   '#field_suffix' => (!empty($form_state['values']['islandora_scholar_xpaths_demo']) ? _mods_id($form_state['values']['islandora_scholar_xpaths_degree_grantor'], t($form_state['values']['islandora_scholar_xpaths_demo'])) : t("No PID entered to verify MODS validity.")),
  ));
  $form['islandora_scholar_xpaths_google'] += _islandora_scholar_add_variable('islandora_scholar_xpaths_genre', '//mods:mods[1]/mods:genre', array(
    '#type' => 'textfield',
    '#title' => t('Genre'),
    '#description' => t('Default Value: //mods:mods[1]/mods:genre <br/>XPath to use for genre.'),
    '#states' => array(
      'visible' => array(
        ':input[name="islandora_scholar_xpaths_enabled"]' => array('checked' => TRUE),
      ),
    ),
    '#ajax' => array(
      'callback' => 'islandora_scholar_xpaths_google_callback',
      'wrapper' => 'islandora_scholar_xpaths_google_title',
    ),
   '#field_suffix' => (!empty($form_state['values']['islandora_scholar_xpaths_demo']) ? _mods_id($form_state['values']['islandora_scholar_xpaths_genre'], t($form_state['values']['islandora_scholar_xpaths_demo'])) : t("No PID entered to verify MODS validity.")),
  ));
  $form['islandora_scholar_xpaths_google'] += _islandora_scholar_add_variable('islandora_scholar_xpaths_host_title', '//mods:relatedItem[@type="host"]//mods:title', array(
    '#type' => 'textfield',
    '#title' => t('Host Title'),
    '#description' => t('Default Value: //mods:relatedItem[@type="host"]//mods:title <br/>XPath to use for the host_title.'),
    '#states' => array(
      'visible' => array(
        ':input[name="islandora_scholar_xpaths_enabled"]' => array('checked' => TRUE),
      ),
    ),
    '#ajax' => array(
      'callback' => 'islandora_scholar_xpaths_google_callback',
      'wrapper' => 'islandora_scholar_xpaths_google_title',
    ),
   '#field_suffix' => (!empty($form_state['values']['islandora_scholar_xpaths_demo']) ? _mods_id($form_state['values']['islandora_scholar_xpaths_host_title'], t($form_state['values']['islandora_scholar_xpaths_demo'])) : t("No PID entered to verify MODS validity.")),
  ));
  $form['islandora_scholar_xpaths_google'] += _islandora_scholar_add_variable('islandora_scholar_xpaths_issn', '//mods:identifier[@type="issn"]', array(
    '#type' => 'textfield',
    '#title' => t('ISSN'),
    '#description' => t('Default Value: //mods:identifier[@type="issn"] <br/>XPath to use for the issn.'),
    '#states' => array(
      'visible' => array(
        ':input[name="islandora_scholar_xpaths_enabled"]' => array('checked' => TRUE),
      ),
    ),
    '#ajax' => array(
      'callback' => 'islandora_scholar_xpaths_google_callback',
      'wrapper' => 'islandora_scholar_xpaths_google_title',
    ),
   '#field_suffix' => (!empty($form_state['values']['islandora_scholar_xpaths_demo']) ? _mods_id($form_state['values']['islandora_scholar_xpaths_issn'], t($form_state['values']['islandora_scholar_xpaths_demo'])) : t("No PID entered to verify MODS validity.")),
  ));
  $form['islandora_scholar_xpaths_google'] += _islandora_scholar_add_variable('islandora_scholar_xpaths_issue', '//mods:mods[1]/mods:part/mods:detail[@type="issue"]/mods:number', array(
    '#type' => 'textfield',
    '#title' => t('Issue'),
    '#description' => t('Default Value: //mods:mods[1]/mods:part/mods:detail[@type="issue"]/mods:number <br/>XPath to use for the issue title, name or number.'),
    '#states' => array(
      'visible' => array(
        ':input[name="islandora_scholar_xpaths_enabled"]' => array('checked' => TRUE),
      ),
    ),
    '#ajax' => array(
      'callback' => 'islandora_scholar_xpaths_google_callback',
      'wrapper' => 'islandora_scholar_xpaths_google_title',
    ),
   '#field_suffix' => (!empty($form_state['values']['islandora_scholar_xpaths_demo']) ? _mods_id($form_state['values']['islandora_scholar_xpaths_issue'], t($form_state['values']['islandora_scholar_xpaths_demo'])) : t("No PID entered to verify MODS validity.")),
  ));
  $form['islandora_scholar_xpaths_google'] += _islandora_scholar_add_variable('islandora_scholar_xpaths_online_date', '//mods:recordInfo/mods:recordCreationDate', array(
    '#type' => 'textfield',
    '#title' => t('Online Date'),
    '#description' => t('Default Value: //mods:recordInfo/mods:recordCreationDate <br/>XPath to use for the online_date.'),
    '#states' => array(
      'visible' => array(
        ':input[name="islandora_scholar_xpaths_enabled"]' => array('checked' => TRUE),
      ),
    ),
    '#ajax' => array(
      'callback' => 'islandora_scholar_xpaths_google_callback',
      'wrapper' => 'islandora_scholar_xpaths_google_title',
    ),
   '#field_suffix' => (!empty($form_state['values']['islandora_scholar_xpaths_demo']) ? _mods_id($form_state['values']['islandora_scholar_xpaths_online_date'], t($form_state['values']['islandora_scholar_xpaths_demo'])) : t("No PID entered to verify MODS validity.")),
  ));
  $form['islandora_scholar_xpaths_coins'] += _islandora_scholar_add_variable('islandora_scholar_xpaths_department', '//mods:identifier[@type="u2"]', array(
    '#type' => 'textfield',
    '#title' => t('Department'),
    '#description' => t('Default Value: //mods:identifier[@type="u2"] <br/>XPath to use for the department.'),
    '#states' => array(
      'visible' => array(
        ':input[name="islandora_scholar_xpaths_enabled"]' => array('checked' => TRUE),
      ),
    ),
    '#ajax' => array(
      'callback' => 'islandora_scholar_xpaths_coins_callback',
      'wrapper' => 'islandora_scholar_xpaths_coins_title',
    ),
   '#field_suffix' => (!empty($form_state['values']['islandora_scholar_xpaths_demo']) ? _mods_id($form_state['values']['islandora_scholar_xpaths_department'], t($form_state['values']['islandora_scholar_xpaths_demo'])) : t("No PID entered to verify MODS validity.")),
  ));
  $form['islandora_scholar_xpaths_coins'] += _islandora_scholar_add_variable('islandora_scholar_xpaths_related_date', '//mods:relatedItem[@type="host"]//mods:date', array(
    '#type' => 'textfield',
    '#title' => t('Related Date'),
    '#description' => t('Default Value: //mods:relatedItem[@type="host"]//mods:date <br/>XPath to use for the related_date.'),
    '#states' => array(
      'visible' => array(
        ':input[name="islandora_scholar_xpaths_enabled"]' => array('checked' => TRUE),
      ),
    ),
    '#ajax' => array(
      'callback' => 'islandora_scholar_xpaths_coins_callback',
      'wrapper' => 'islandora_scholar_xpaths_coins_title',
    ),
   '#field_suffix' => (!empty($form_state['values']['islandora_scholar_xpaths_demo']) ? _mods_id($form_state['values']['islandora_scholar_xpaths_related_date'], t($form_state['values']['islandora_scholar_xpaths_demo'])) : t("No PID entered to verify MODS validity.")),
  ));
  $form['islandora_scholar_xpaths_coins'] += _islandora_scholar_add_variable('islandora_scholar_xpaths_start_page', '//mods:extent[@unit="page"]/mods:start', array(
    '#type' => 'textfield',
    '#title' => t('Start Page'),
    '#description' => t('Default Value: //mods:extent[@unit="page"]/mods:start <br/>XPath to use for the start_page.'),
    '#states' => array(
      'visible' => array(
        ':input[name="islandora_scholar_xpaths_enabled"]' => array('checked' => TRUE),
      ),
    ),
    '#ajax' => array(
      'callback' => 'islandora_scholar_xpaths_coins_callback',
      'wrapper' => 'islandora_scholar_xpaths_coins_title',
    ),
   '#field_suffix' => (!empty($form_state['values']['islandora_scholar_xpaths_demo']) ? _mods_id($form_state['values']['islandora_scholar_xpaths_start_page'], t($form_state['values']['islandora_scholar_xpaths_demo'])) : t("No PID entered to verify MODS validity.")),
  ));
  $form['islandora_scholar_xpaths_coins'] += _islandora_scholar_add_variable('islandora_scholar_xpaths_embargo_date', '//mods:dateOther[@type="embargo"]', array(
    '#type' => 'textfield',
    '#title' => t('Embargo Date'),
    '#description' => t('Default Value: //mods:dateOther[@type="embargo"] <br/>XPath to use for the embargo_date.'),
    '#states' => array(
      'visible' => array(
        ':input[name="islandora_scholar_xpaths_enabled"]' => array('checked' => TRUE),
      ),
    ),
    '#ajax' => array(
      'callback' => 'islandora_scholar_xpaths_coins_callback',
      'wrapper' => 'islandora_scholar_xpaths_coins_title',
    ),
   '#field_suffix' => (!empty($form_state['values']['islandora_scholar_xpaths_demo']) ? _mods_id($form_state['values']['islandora_scholar_xpaths_embargo_date'], t($form_state['values']['islandora_scholar_xpaths_demo'])) : t("No PID entered to verify MODS validity.")),
  ));
  $form['islandora_scholar_xpaths_coins'] += _islandora_scholar_add_variable('islandora_scholar_xpaths_end_page', '//mods:extent[@unit="page"]/mods:end', array(
    '#type' => 'textfield',
    '#title' => t('End Page'),
    '#description' => t('Default Value: //mods:extent[@unit="page"]/mods:end <br/>XPath to use for the end_page.'),
    '#states' => array(
      'visible' => array(
        ':input[name="islandora_scholar_xpaths_enabled"]' => array('checked' => TRUE),
      ),
    ),
    '#ajax' => array(
      'callback' => 'islandora_scholar_xpaths_coins_callback',
      'wrapper' => 'islandora_scholar_xpaths_coins_title',
    ),
   '#field_suffix' => (!empty($form_state['values']['islandora_scholar_xpaths_demo']) ? _mods_id($form_state['values']['islandora_scholar_xpaths_end_page'], t($form_state['values']['islandora_scholar_xpaths_demo'])) : t("No PID entered to verify MODS validity.")),
  ));
  $form['islandora_scholar_xpaths_coins'] += _islandora_scholar_add_variable('islandora_scholar_xpaths_origin_date', '//mods:originInfo/mods:dateIssued', array(
    '#type' => 'textfield',
    '#title' => t('Origin Date'),
    '#description' => t('Default Value: //mods:originInfo/mods:dateIssued <br/>XPath to use for the origin_date.'),
    '#states' => array(
      'visible' => array(
        ':input[name="islandora_scholar_xpaths_enabled"]' => array('checked' => TRUE),
      ),
    ),
    '#ajax' => array(
      'callback' => 'islandora_scholar_xpaths_coins_callback',
      'wrapper' => 'islandora_scholar_xpaths_coins_title',
    ),
   '#field_suffix' => (!empty($form_state['values']['islandora_scholar_xpaths_demo']) ? _mods_id($form_state['values']['islandora_scholar_xpaths_origin_date'], t($form_state['values']['islandora_scholar_xpaths_demo'])) : t("No PID entered to verify MODS validity.")),
  ));
  $form['islandora_scholar_xpaths_coins'] += _islandora_scholar_add_variable('islandora_scholar_xpaths_part_date', '//mods:part/mods:date', array(
    '#type' => 'textfield',
    '#title' => t('Part Date'),
    '#description' => t('Default Value: //mods:part/mods:date <br/>XPath to use for the part_date.'),
    '#states' => array(
      'visible' => array(
        ':input[name="islandora_scholar_xpaths_enabled"]' => array('checked' => TRUE),
      ),
    ),
    '#ajax' => array(
      'callback' => 'islandora_scholar_xpaths_coins_callback',
      'wrapper' => 'islandora_scholar_xpaths_coins_title',
    ),
   '#field_suffix' => (!empty($form_state['values']['islandora_scholar_xpaths_demo']) ? _mods_id($form_state['values']['islandora_scholar_xpaths_part_date'], t($form_state['values']['islandora_scholar_xpaths_demo'])) : t("No PID entered to verify MODS validity.")),
  ));
  $form['islandora_scholar_xpaths_coins'] += _islandora_scholar_add_variable('islandora_scholar_xpaths_volume', '//mods:mods[1]/mods:part/mods:detail[@type="volume"]/mods:number', array(
    '#type' => 'textfield',
    '#title' => t('Volume'),
    '#description' => t('Default Value: //mods:mods[1]/mods:part/mods:detail[@type="volume"]/mods:number <br/>XPath to use for volume number / title.'),
    '#states' => array(
      'visible' => array(
        ':input[name="islandora_scholar_xpaths_enabled"]' => array('checked' => TRUE),
      ),
    ),
    '#ajax' => array(
      'callback' => 'islandora_scholar_xpaths_coins_callback',
      'wrapper' => 'islandora_scholar_xpaths_coins_title',
    ),
   '#field_suffix' => (!empty($form_state['values']['islandora_scholar_xpaths_demo']) ? _mods_id($form_state['values']['islandora_scholar_xpaths_volume'], t($form_state['values']['islandora_scholar_xpaths_demo'])) : t("No PID entered to verify MODS validity.")),
  ));
  $form['islandora_scholar_xpaths_citeproc'] += _islandora_scholar_add_variable('islandora_scholar_xpaths_title_subtitle', '../mods:subTitle', array(
    '#type' => 'textfield',
    '#title' => t('Subtitle used for Citeproc JSON'),
    '#description' => t('Default Value: ../mods:subTitle <br/>XPath to use for subtitle. Default is a relative path from the title (declared above). If there is a value it will display as `title: subtitle`'),
    '#states' => array(
      'visible' => array(
        ':input[name="islandora_scholar_xpaths_enabled"]' => array('checked' => TRUE),
      ),
    ),
    '#ajax' => array(
      'callback' => 'islandora_scholar_xpaths_citeproc_callback',
      'wrapper' => 'islandora_scholar_xpaths_citeproc_title',
    ),
   '#field_suffix' => (!empty($form_state['values']['islandora_scholar_xpaths_demo']) ? _mods_id($form_state['values']['islandora_scholar_xpaths_title_subtitle'], t($form_state['values']['islandora_scholar_xpaths_demo'])) : t("No PID entered to verify MODS validity.")),
  ));
  $form['islandora_scholar_xpaths_citeproc'] += _islandora_scholar_add_variable('islandora_scholar_xpaths_title_nonsort', '../mods:nonSort', array(
    '#type' => 'textfield',
    '#title' => t('Nonsort used for Citeproc JSON'),
    '#description' => t("Default Value: ../mods:nonSort <br/>XPath to use for title's nonsort values. The defaults are relative path from the title (declared above). If there is a value it will display as `non_sort title`"),
    '#states' => array(
      'visible' => array(
        ':input[name="islandora_scholar_xpaths_enabled"]' => array('checked' => TRUE),
      ),
    ),
    '#ajax' => array(
      'callback' => 'islandora_scholar_xpaths_citeproc_callback',
      'wrapper' => 'islandora_scholar_xpaths_citeproc_title',
    ),
   '#field_suffix' => (!empty($form_state['values']['islandora_scholar_xpaths_demo']) ? _mods_id($form_state['values']['islandora_scholar_xpaths_title_nonsort'], t($form_state['values']['islandora_scholar_xpaths_demo'])) : t("No PID entered to verify MODS validity.")),
  ));
  $form['islandora_scholar_xpaths_citeproc'] += _islandora_scholar_add_variable('islandora_scholar_xpaths_call-number', '//mods:mods[1]/mods:classification', array(
    '#type' => 'textfield',
    '#title' => t('Call number'),
    '#description' => t('Default Value: //mods:mods[1]/mods:classification <br/>XPath to use for the call number.'),
    '#states' => array(
      'visible' => array(
        ':input[name="islandora_scholar_xpaths_enabled"]' => array('checked' => TRUE),
      ),
    ),
    '#ajax' => array(
      'callback' => 'islandora_scholar_xpaths_citeproc_callback',
      'wrapper' => 'islandora_scholar_xpaths_citeproc_title',
    ),
   '#field_suffix' => (!empty($form_state['values']['islandora_scholar_xpaths_demo']) ? _mods_id($form_state['values']['islandora_scholar_xpaths_call-number'], t($form_state['values']['islandora_scholar_xpaths_demo'])) : t("No PID entered to verify MODS validity.")),
  ));
  $form['islandora_scholar_xpaths_citeproc'] += _islandora_scholar_add_variable('islandora_scholar_xpaths_collection-title', '//mods:mods[1]/mods:relatedItem[@type="series"]/mods:titleInfo[not(@type)]/mods:title', array(
    '#type' => 'textfield',
    '#title' => t('Collection title'),
    '#description' => t('Default Value: //mods:mods[1]/mods:relatedItem[@type="series"]/mods:titleInfo[not(@type)]/mods:title <br/>XPath to use for collection title.'),
    '#states' => array(
      'visible' => array(
        ':input[name="islandora_scholar_xpaths_enabled"]' => array('checked' => TRUE),
      ),
    ),
    '#ajax' => array(
      'callback' => 'islandora_scholar_xpaths_citeproc_callback',
      'wrapper' => 'islandora_scholar_xpaths_citeproc_title',
    ),
   '#field_suffix' => (!empty($form_state['values']['islandora_scholar_xpaths_demo']) ? _mods_id($form_state['values']['islandora_scholar_xpaths_collection-title'], t($form_state['values']['islandora_scholar_xpaths_demo'])) : t("No PID entered to verify MODS validity.")),
  ));
  $form['islandora_scholar_xpaths_citeproc'] += _islandora_scholar_add_variable('islandora_scholar_xpaths_container-title', '//mods:mods[1]/mods:relatedItem[@type="host"]/mods:titleInfo[not(@type)]/mods:title', array(
    '#type' => 'textfield',
    '#title' => t('Container title'),
    '#description' => t('Default Value: //mods:mods[1]/mods:relatedItem[@type="host"]/mods:titleInfo[not(@type)]/mods:title <br/>XPath to use for the container search.'),
    '#states' => array(
      'visible' => array(
        ':input[name="islandora_scholar_xpaths_enabled"]' => array('checked' => TRUE),
      ),
    ),
    '#ajax' => array(
      'callback' => 'islandora_scholar_xpaths_citeproc_callback',
      'wrapper' => 'islandora_scholar_xpaths_citeproc_title',
    ),
   '#field_suffix' => (!empty($form_state['values']['islandora_scholar_xpaths_demo']) ? _mods_id($form_state['values']['islandora_scholar_xpaths_container-title'], t($form_state['values']['islandora_scholar_xpaths_demo'])) : t("No PID entered to verify MODS validity.")),
  ));
  $form['islandora_scholar_xpaths_citeproc'] += _islandora_scholar_add_variable('islandora_scholar_xpaths_DOI', '//mods:mods[1]/mods:identifier[@type="doi"]', array(
    '#type' => 'textfield',
    '#title' => t('DOI'),
    '#description' => t('Default Value: //mods:mods[1]/mods:identifier[@type="doi"] <br/>XPath to use for DOI.'),
    '#states' => array(
      'visible' => array(
        ':input[name="islandora_scholar_xpaths_enabled"]' => array('checked' => TRUE),
      ),
    ),
    '#ajax' => array(
      'callback' => 'islandora_scholar_xpaths_citeproc_callback',
      'wrapper' => 'islandora_scholar_xpaths_citeproc_title',
    ),
   '#field_suffix' => (!empty($form_state['values']['islandora_scholar_xpaths_demo']) ? _mods_id($form_state['values']['islandora_scholar_xpaths_DOI'], t($form_state['values']['islandora_scholar_xpaths_demo'])) : t("No PID entered to verify MODS validity.")),
  ));
  $form['islandora_scholar_xpaths_citeproc'] += _islandora_scholar_add_variable('islandora_scholar_xpaths_edition', '//mods:mods[1]/mods:originInfo/mods:edition', array(
    '#type' => 'textfield',
    '#title' => t('Edition'),
    '#description' => t('Default Value: //mods:mods[1]/mods:originInfo/mods:edition <br/>The XPath to use edition.'),
    '#states' => array(
      'visible' => array(
        ':input[name="islandora_scholar_xpaths_enabled"]' => array('checked' => TRUE),
      ),
    ),
    '#ajax' => array(
      'callback' => 'islandora_scholar_xpaths_citeproc_callback',
      'wrapper' => 'islandora_scholar_xpaths_citeproc_title',
    ),
   '#field_suffix' => (!empty($form_state['values']['islandora_scholar_xpaths_demo']) ? _mods_id($form_state['values']['islandora_scholar_xpaths_edition'], t($form_state['values']['islandora_scholar_xpaths_demo'])) : t("No PID entered to verify MODS validity.")),
  ));
  $form['islandora_scholar_xpaths_citeproc'] += _islandora_scholar_add_variable('islandora_scholar_xpaths_event', '//mods:mods[1][mods:genre[@authority="local"]="conferencePaper"]/mods:relatedItem/mods:titleInfo/mods:title', array(
    '#type' => 'textfield',
    '#title' => t('Event Title'),
    '#description' => t('Default Value: //mods:mods[1][mods:genre[@authority="local"]="conferencePaper"]/mods:relatedItem/mods:titleInfo/mods:title <br/>XPath to use for event type.'),
    '#states' => array(
      'visible' => array(
        ':input[name="islandora_scholar_xpaths_enabled"]' => array('checked' => TRUE),
      ),
    ),
    '#ajax' => array(
      'callback' => 'islandora_scholar_xpaths_citeproc_callback',
      'wrapper' => 'islandora_scholar_xpaths_citeproc_title',
    ),
   '#field_suffix' => (!empty($form_state['values']['islandora_scholar_xpaths_demo']) ? _mods_id($form_state['values']['islandora_scholar_xpaths_event'], t($form_state['values']['islandora_scholar_xpaths_demo'])) : t("No PID entered to verify MODS validity.")),
  ));
  $form['islandora_scholar_xpaths_citeproc'] += _islandora_scholar_add_variable('islandora_scholar_xpaths_event_place_property', '//mods:mods[1][mods:genre[@authority="marcgt"][text()="conference publication"]]/mods:originInfo/mods:place/mods:placeTerm', array(
    '#type' => 'textfield',
    '#title' => t('Event Place'),
    '#description' => t('Default Value: //mods:mods[1][mods:genre[@authority="marcgt"][text()="conference publication"]]/mods:originInfo/mods:place/mods:placeTerm <br/>XPath to use for event place property. Gets the event-place property for the Citation.'),
    '#states' => array(
      'visible' => array(
        ':input[name="islandora_scholar_xpaths_enabled"]' => array('checked' => TRUE),
      ),
    ),
    '#ajax' => array(
      'callback' => 'islandora_scholar_xpaths_citeproc_callback',
      'wrapper' => 'islandora_scholar_xpaths_citeproc_title',
    ),
   '#field_suffix' => (!empty($form_state['values']['islandora_scholar_xpaths_demo']) ? _mods_id($form_state['values']['islandora_scholar_xpaths_event_place_property'], t($form_state['values']['islandora_scholar_xpaths_demo'])) : t("No PID entered to verify MODS validity.")),
  ));
  $form['islandora_scholar_xpaths_citeproc'] += _islandora_scholar_add_variable('islandora_scholar_xpaths_event_place', '//mods:mods[1][mods:genre[@authority="local"][text()="conferencePaper"]]/mods:originInfo/mods:place/mods:placeTerm', array(
    '#type' => 'textfield',
    '#title' => t('Event'),
    '#description' => t('Default Value: //mods:mods[1][mods:genre[@authority="local"][text()="conferencePaper"]]/mods:originInfo/mods:place/mods:placeTerm <br/>XPath to use for event place. Gets the event-place property for the Citation. If the value of `Event Place` is empty Citeproc will default to this.'),
    '#states' => array(
      'visible' => array(
        ':input[name="islandora_scholar_xpaths_enabled"]' => array('checked' => TRUE),
      ),
    ),
    '#ajax' => array(
      'callback' => 'islandora_scholar_xpaths_citeproc_callback',
      'wrapper' => 'islandora_scholar_xpaths_citeproc_title',
    ),
   '#field_suffix' => (!empty($form_state['values']['islandora_scholar_xpaths_demo']) ? _mods_id($form_state['values']['islandora_scholar_xpaths_event_place'], t($form_state['values']['islandora_scholar_xpaths_demo'])) : t("No PID entered to verify MODS validity.")),
  ));
  $form['islandora_scholar_xpaths_citeproc'] += _islandora_scholar_add_variable('islandora_scholar_xpaths_ISBN', '//mods:mods[1]/mods:identifier[@type="isbn"]', array(
    '#type' => 'textfield',
    '#title' => t('ISBN'),
    '#description' => t('Default Value: //mods:mods[1]/mods:identifier[@type="isbn"] <br/>XPath to use for ISBN number.'),
    '#states' => array(
      'visible' => array(
        ':input[name="islandora_scholar_xpaths_enabled"]' => array('checked' => TRUE),
      ),
    ),
    '#ajax' => array(
      'callback' => 'islandora_scholar_xpaths_citeproc_callback',
      'wrapper' => 'islandora_scholar_xpaths_citeproc_title',
    ),
   '#field_suffix' => (!empty($form_state['values']['islandora_scholar_xpaths_demo']) ? _mods_id($form_state['values']['islandora_scholar_xpaths_ISBN'], t($form_state['values']['islandora_scholar_xpaths_demo'])) : t("No PID entered to verify MODS validity.")),
  ));
  $form['islandora_scholar_xpaths_citeproc'] += _islandora_scholar_add_variable('islandora_scholar_xpaths_note', '//mods:mods[1]/mods:note', array(
    '#type' => 'textfield',
    '#title' => t('Note'),
    '#description' => t('Default Value: //mods:mods[1]/mods:note <br/>XPath to use for notes.'),
    '#states' => array(
      'visible' => array(
        ':input[name="islandora_scholar_xpaths_enabled"]' => array('checked' => TRUE),
      ),
    ),
    '#ajax' => array(
      'callback' => 'islandora_scholar_xpaths_citeproc_callback',
      'wrapper' => 'islandora_scholar_xpaths_citeproc_title',
    ),
   '#field_suffix' => (!empty($form_state['values']['islandora_scholar_xpaths_demo']) ? _mods_id($form_state['values']['islandora_scholar_xpaths_note'], t($form_state['values']['islandora_scholar_xpaths_demo'])) : t("No PID entered to verify MODS validity.")),
  ));
  $form['islandora_scholar_xpaths_citeproc'] += _islandora_scholar_add_variable('islandora_scholar_xpaths_number', '//mods:mods[1]/mods:relatedItem[@type="series"]/mods:titleInfo/mods:partNumber', array(
    '#type' => 'textfield',
    '#title' => t('Number'),
    '#description' => t('Default Value: //mods:mods[1]/mods:relatedItem[@type="series"]/mods:titleInfo/mods:partNumber <br/>XPath to use for series part number.'),
    '#states' => array(
      'visible' => array(
        ':input[name="islandora_scholar_xpaths_enabled"]' => array('checked' => TRUE),
      ),
    ),
    '#ajax' => array(
      'callback' => 'islandora_scholar_xpaths_citeproc_callback',
      'wrapper' => 'islandora_scholar_xpaths_citeproc_title',
    ),
   '#field_suffix' => (!empty($form_state['values']['islandora_scholar_xpaths_demo']) ? _mods_id($form_state['values']['islandora_scholar_xpaths_number'], t($form_state['values']['islandora_scholar_xpaths_demo'])) : t("No PID entered to verify MODS validity.")),
  ));
  $form['islandora_scholar_xpaths_citeproc'] += _islandora_scholar_add_variable('islandora_scholar_xpaths_publisher', '//mods:mods[1]/mods:originInfo/mods:publisher', array(
    '#type' => 'textfield',
    '#title' => t('Publisher'),
    '#description' => t('Default Value: //mods:mods[1]/mods:originInfo/mods:publisher <br/>XPath to use for Publisher.'),
    '#states' => array(
      'visible' => array(
        ':input[name="islandora_scholar_xpaths_enabled"]' => array('checked' => TRUE),
      ),
    ),
    '#ajax' => array(
      'callback' => 'islandora_scholar_xpaths_citeproc_callback',
      'wrapper' => 'islandora_scholar_xpaths_citeproc_title',
    ),
   '#field_suffix' => (!empty($form_state['values']['islandora_scholar_xpaths_demo']) ? _mods_id($form_state['values']['islandora_scholar_xpaths_publisher'], t($form_state['values']['islandora_scholar_xpaths_demo'])) : t("No PID entered to verify MODS validity.")),
  ));
  $form['islandora_scholar_xpaths_citeproc'] += _islandora_scholar_add_variable('islandora_scholar_xpaths_publisher-place', '//mods:mods[1]/mods:originInfo/mods:place/mods:placeTerm', array(
    '#type' => 'textfield',
    '#title' => t('Publisher place'),
    '#description' => t('Default Value: //mods:mods[1]/mods:originInfo/mods:place/mods:placeTerm <br/>XPath to use for publisher-place.'),
    '#states' => array(
      'visible' => array(
        ':input[name="islandora_scholar_xpaths_enabled"]' => array('checked' => TRUE),
      ),
    ),
    '#ajax' => array(
      'callback' => 'islandora_scholar_xpaths_citeproc_callback',
      'wrapper' => 'islandora_scholar_xpaths_citeproc_title',
    ),
   '#field_suffix' => (!empty($form_state['values']['islandora_scholar_xpaths_demo']) ? _mods_id($form_state['values']['islandora_scholar_xpaths_publisher-place'], t($form_state['values']['islandora_scholar_xpaths_demo'])) : t("No PID entered to verify MODS validity.")),
  ));
  $form['islandora_scholar_xpaths_citeproc'] += _islandora_scholar_add_variable('islandora_scholar_xpaths_URL', '//mods:mods[1]/mods:location/mods:url', array(
    '#type' => 'textfield',
    '#title' => t('URL'),
    '#description' => t('Default Value: //mods:mods[1]/mods:location/mods:url <br/>XPath to use for URL.'),
    '#states' => array(
      'visible' => array(
        ':input[name="islandora_scholar_xpaths_enabled"]' => array('checked' => TRUE),
      ),
    ),
    '#ajax' => array(
      'callback' => 'islandora_scholar_xpaths_citeproc_callback',
      'wrapper' => 'islandora_scholar_xpaths_citeproc_title',
    ),
   '#field_suffix' => (!empty($form_state['values']['islandora_scholar_xpaths_demo']) ? _mods_id($form_state['values']['islandora_scholar_xpaths_URL'], t($form_state['values']['islandora_scholar_xpaths_demo'])) : t("No PID entered to verify MODS validity.")),
  ));
  $form['islandora_scholar_xpaths_citeproc'] += _islandora_scholar_add_variable('islandora_scholar_xpaths_number-pmid', '//mods:mods[1]/mods:identifier[@type="accession"]', array(
    '#type' => 'textfield',
    '#title' => t('PubMed ID'),
    '#description' => t('Default Value: //mods:mods[1]/mods:identifier[@type="accession"] <br/>XPath to use for the PubMed ID.'),
    '#states' => array(
      'visible' => array(
        ':input[name="islandora_scholar_xpaths_enabled"]' => array('checked' => TRUE),
      ),
    ),
    '#ajax' => array(
      'callback' => 'islandora_scholar_xpaths_citeproc_callback',
      'wrapper' => 'islandora_scholar_xpaths_citeproc_title',
    ),
   '#field_suffix' => (!empty($form_state['values']['islandora_scholar_xpaths_demo']) ? _mods_id($form_state['values']['islandora_scholar_xpaths_number-pmid'], t($form_state['values']['islandora_scholar_xpaths_demo'])) : t("No PID entered to verify MODS validity.")),
  ));
  $form['islandora_scholar_xpaths_citeproc'] += _islandora_scholar_add_variable('islandora_scholar_xpaths_number-pmcid', '//mods:mods[1]/mods:identifier[@type="pmc"]', array(
    '#type' => 'textfield',
    '#title' => t('PMCID'),
    '#description' => t('Default Value: //mods:mods[1]/mods:identifier[@type="pmc"] <br/>XPath to use for the PubMed Central reference number (PMCID).'),
    '#states' => array(
      'visible' => array(
        ':input[name="islandora_scholar_xpaths_enabled"]' => array('checked' => TRUE),
      ),
    ),
    '#ajax' => array(
      'callback' => 'islandora_scholar_xpaths_citeproc_callback',
      'wrapper' => 'islandora_scholar_xpaths_citeproc_title',
    ),
   '#field_suffix' => (!empty($form_state['values']['islandora_scholar_xpaths_demo']) ? _mods_id($form_state['values']['islandora_scholar_xpaths_number-pmcid'], t($form_state['values']['islandora_scholar_xpaths_demo'])) : t("No PID entered to verify MODS validity.")),
  ));
  $form['islandora_scholar_xpaths_citeproc'] += _islandora_scholar_add_variable('islandora_scholar_xpaths_number-nihmsid', '//mods:mods[1]/mods:identifier[@type="mid"]', array(
    '#type' => 'textfield',
    '#title' => t('NIH Manuscript Submission Reference Number'),
    '#description' => t('Default Value: //mods:mods[1]/mods:identifier[@type="mid"] <br/>XPath to use for NIH Manuscript Submission Reference Number (NIHMSID).'),
    '#states' => array(
      'visible' => array(
        ':input[name="islandora_scholar_xpaths_enabled"]' => array('checked' => TRUE),
      ),
    ),
    '#ajax' => array(
      'callback' => 'islandora_scholar_xpaths_citeproc_callback',
      'wrapper' => 'islandora_scholar_xpaths_citeproc_title',
    ),
   '#field_suffix' => (!empty($form_state['values']['islandora_scholar_xpaths_demo']) ? _mods_id($form_state['values']['islandora_scholar_xpaths_number-nihmsid'], t($form_state['values']['islandora_scholar_xpaths_demo'])) : t("No PID entered to verify MODS validity.")),
  ));
  $form['islandora_scholar_xpaths_citeproc'] += _islandora_scholar_add_variable('islandora_scholar_xpaths_event_property', '//mods:mods[1][mods:genre[@authority="marcgt"][text()="conference publication"]]/mods:relatedItem/mods:titleInfo/mods:title', array(
    '#type' => 'textfield',
    '#title' => t('Property'),
    '#description' => t('Default Value: //mods:mods[1][mods:genre[@authority="marcgt"][text()="conference publication"]]/mods:relatedItem/mods:titleInfo/mods:title <br/>XPath to use for property.'),
    '#states' => array(
      'visible' => array(
        ':input[name="islandora_scholar_xpaths_enabled"]' => array('checked' => TRUE),
      ),
    ),
    '#ajax' => array(
      'callback' => 'islandora_scholar_xpaths_citeproc_callback',
      'wrapper' => 'islandora_scholar_xpaths_citeproc_title',
    ),
   '#field_suffix' => (!empty($form_state['values']['islandora_scholar_xpaths_demo']) ? _mods_id($form_state['values']['islandora_scholar_xpaths_event_property'], t($form_state['values']['islandora_scholar_xpaths_demo'])) : t("No PID entered to verify MODS validity.")),
  ));
  $form['islandora_scholar_xpaths_citeproc'] += _islandora_scholar_add_variable('islandora_scholar_xpaths_pages', '//mods:mods[1]/mods:part/mods:extent[@unit="pages"]', array(
    '#type' => 'textfield',
    '#title' => t('Pages'),
    '#description' => t('Default Value: //mods:mods[1]/mods:part/mods:extent[@unit="pages"] <br/>XPath to use for pages.'),
    '#states' => array(
      'visible' => array(
        ':input[name="islandora_scholar_xpaths_enabled"]' => array('checked' => TRUE),
      ),
    ),
    '#ajax' => array(
      'callback' => 'islandora_scholar_xpaths_citeproc_callback',
      'wrapper' => 'islandora_scholar_xpaths_citeproc_title',
    ),
   '#field_suffix' => (!empty($form_state['values']['islandora_scholar_xpaths_demo']) ? _mods_id($form_state['values']['islandora_scholar_xpaths_pages'], t($form_state['values']['islandora_scholar_xpaths_demo'])) : t("No PID entered to verify MODS validity.")),
  ));
  $form['islandora_scholar_xpaths_citeproc'] += _islandora_scholar_add_variable('islandora_scholar_xpaths_types', '//mods:mods[1]/mods:genre[@authority="endnote"]', array(
    '#type' => 'textfield',
    '#title' => t('Types'),
    '#description' => t('Default Value: //mods:mods[1]/mods:genre[@authority="endnote"] <br/>XPath to use for types.'),
    '#states' => array(
      'visible' => array(
        ':input[name="islandora_scholar_xpaths_enabled"]' => array('checked' => TRUE),
      ),
    ),
    '#ajax' => array(
      'callback' => 'islandora_scholar_xpaths_citeproc_callback',
      'wrapper' => 'islandora_scholar_xpaths_citeproc_title',
    ),
   '#field_suffix' => (!empty($form_state['values']['islandora_scholar_xpaths_demo']) ? _mods_id($form_state['values']['islandora_scholar_xpaths_types'], t($form_state['values']['islandora_scholar_xpaths_demo'])) : t("No PID entered to verify MODS validity.")),
  ));
  $form['islandora_scholar_xpaths_citeproc'] += _islandora_scholar_add_variable('islandora_scholar_xpaths_type_marcgt', '//mods:mods[1]/mods:genre[@authority="marcgt"]', array(
    '#type' => 'textfield',
    '#title' => t('MARC genre type'),
    '#description' => t('Default Value: //mods:mods[1]/mods:genre[@authority="marcgt"] <br/>XPath to use for type_marcgt.'),
    '#states' => array(
      'visible' => array(
        ':input[name="islandora_scholar_xpaths_enabled"]' => array('checked' => TRUE),
      ),
    ),
    '#ajax' => array(
      'callback' => 'islandora_scholar_xpaths_citeproc_callback',
      'wrapper' => 'islandora_scholar_xpaths_citeproc_title',
    ),
   '#field_suffix' => (!empty($form_state['values']['islandora_scholar_xpaths_demo']) ? _mods_id($form_state['values']['islandora_scholar_xpaths_type_marcgt'], t($form_state['values']['islandora_scholar_xpaths_demo'])) : t("No PID entered to verify MODS validity.")),
  ));
  $form['islandora_scholar_xpaths_citeproc'] += _islandora_scholar_add_variable('islandora_scholar_xpaths_type_marcgt_host_titles', '../mods:relatedItem[@type="host"]/mods:titleInfo/mods:title', array(
    '#type' => 'textfield',
    '#title' => t('→ MARC genre type (relative child): chapter titles in a book'),
    '#description' => t("Default Value: ../mods:relatedItem[@type='host']/mods:titleInfo/mods:title <br/>Relative child XPath to use for type_marcgt titles (Book uses this for title or defaults back to the book title)."),
    '#states' => array(
      'visible' => array(
        ':input[name="islandora_scholar_xpaths_enabled"]' => array('checked' => TRUE),
      ),
    ),
    '#ajax' => array(
      'callback' => 'islandora_scholar_xpaths_citeproc_callback',
      'wrapper' => 'islandora_scholar_xpaths_citeproc_title',
    ),
   '#field_suffix' => (!empty($form_state['values']['islandora_scholar_xpaths_demo']) ? _mods_id($form_state['values']['islandora_scholar_xpaths_type_marcgt_host_titles'], t($form_state['values']['islandora_scholar_xpaths_demo'])) : t("No PID entered to verify MODS validity.")),
  ));
  $form['islandora_scholar_xpaths_citeproc'] += _islandora_scholar_add_variable('islandora_scholar_xpaths_type_marcgt_related', '//mods:mods[1]/mods:relatedItem/mods:genre[@authority="marcgt"]', array(
    '#type' => 'textfield',
    '#title' => t('MARC genre type related'),
    '#description' => t('Default Value: //mods:mods[1]/mods:relatedItem/mods:genre[@authority="marcgt"] <br/>XPath to use for type_marcgt_related.'),
    '#states' => array(
      'visible' => array(
        ':input[name="islandora_scholar_xpaths_enabled"]' => array('checked' => TRUE),
      ),
    ),
    '#ajax' => array(
      'callback' => 'islandora_scholar_xpaths_citeproc_callback',
      'wrapper' => 'islandora_scholar_xpaths_citeproc_title',
    ),
   '#field_suffix' => (!empty($form_state['values']['islandora_scholar_xpaths_demo']) ? _mods_id($form_state['values']['islandora_scholar_xpaths_type_marcgt_related'], t($form_state['values']['islandora_scholar_xpaths_demo'])) : t("No PID entered to verify MODS validity.")),
  ));
  $form['islandora_scholar_xpaths_citeproc'] += _islandora_scholar_add_variable('islandora_scholar_xpaths_types_local_auth', '//mods:mods[1]/mods:genre[not(@authority="marcgt" or @authority="endnote")]', array(
    '#type' => 'textfield',
    '#title' => t('types local auth'),
    '#description' => t('Default Value: //mods:mods[1]/mods:genre[not(@authority="marcgt" or @authority="endnote")] <br/>XPath to use for types_local_auth.'),
    '#states' => array(
      'visible' => array(
        ':input[name="islandora_scholar_xpaths_enabled"]' => array('checked' => TRUE),
      ),
    ),
    '#ajax' => array(
      'callback' => 'islandora_scholar_xpaths_citeproc_callback',
      'wrapper' => 'islandora_scholar_xpaths_citeproc_title',
    ),
   '#field_suffix' => (!empty($form_state['values']['islandora_scholar_xpaths_demo']) ? _mods_id($form_state['values']['islandora_scholar_xpaths_types_local_auth'], t($form_state['values']['islandora_scholar_xpaths_demo'])) : t("No PID entered to verify MODS validity.")),
  ));
  $form['islandora_scholar_xpaths_citeproc'] += _islandora_scholar_add_variable('islandora_scholar_xpaths_season', '//mods:originInfo/mods:dateOther[@type="season"]', array(
    '#type' => 'textfield',
    '#title' => t('Season'),
    '#description' => t('Default Value: //mods:originInfo/mods:dateOther[@type="season"] <br/>XPath to use for season.'),
    '#states' => array(
      'visible' => array(
        ':input[name="islandora_scholar_xpaths_enabled"]' => array('checked' => TRUE),
      ),
    ),
    '#ajax' => array(
      'callback' => 'islandora_scholar_xpaths_citeproc_callback',
      'wrapper' => 'islandora_scholar_xpaths_citeproc_title',
    ),
   '#field_suffix' => (!empty($form_state['values']['islandora_scholar_xpaths_demo']) ? _mods_id($form_state['values']['islandora_scholar_xpaths_season'], t($form_state['values']['islandora_scholar_xpaths_demo'])) : t("No PID entered to verify MODS validity.")),
  ));
  $form['islandora_scholar_xpaths_citeproc'] += _islandora_scholar_add_variable('islandora_scholar_xpaths_date_issued', '//mods:originInfo/mods:dateIssued | //mods:mods[1]/mods:relatedItem[@type="host"]/mods:part/mods:date', array(
    '#type' => 'textfield',
    '#title' => t('Date Issued'),
    '#description' => t('Default Value: //mods:originInfo/mods:dateIssued | //mods:mods[1]/mods:relatedItem[@type="host"]/mods:part/mods:date <br/>XPath to use for date_issued.'),
    '#states' => array(
      'visible' => array(
        ':input[name="islandora_scholar_xpaths_enabled"]' => array('checked' => TRUE),
      ),
    ),
    '#ajax' => array(
      'callback' => 'islandora_scholar_xpaths_citeproc_callback',
      'wrapper' => 'islandora_scholar_xpaths_citeproc_title',
    ),
   '#field_suffix' => (!empty($form_state['values']['islandora_scholar_xpaths_demo']) ? _mods_id($form_state['values']['islandora_scholar_xpaths_date_issued'], t($form_state['values']['islandora_scholar_xpaths_demo'])) : t("No PID entered to verify MODS validity.")),
  ));
  $form['islandora_scholar_xpaths_citeproc'] += _islandora_scholar_add_variable('islandora_scholar_xpaths_title_results', '//mods:mods[1]/mods:titleInfo/mods:title', array(
    '#type' => 'textfield',
    '#title' => t('Title Results'),
    '#description' => t('Default Value: //mods:mods[1]/mods:titleInfo/mods:title <br/>XPath to use for title_results.'),
    '#states' => array(
      'visible' => array(
        ':input[name="islandora_scholar_xpaths_enabled"]' => array('checked' => TRUE),
      ),
    ),
    '#ajax' => array(
      'callback' => 'islandora_scholar_xpaths_citeproc_callback',
      'wrapper' => 'islandora_scholar_xpaths_citeproc_title',
    ),
   '#field_suffix' => (!empty($form_state['values']['islandora_scholar_xpaths_demo']) ? _mods_id($form_state['values']['islandora_scholar_xpaths_title_results'], t($form_state['values']['islandora_scholar_xpaths_demo'])) : t("No PID entered to verify MODS validity.")),
  ));
  $form['islandora_scholar_xpaths_citeproc'] += _islandora_scholar_add_variable('islandora_scholar_xpaths_dateCaptured', '//mods:originInfo/mods:dateCaptured', array(
    '#type' => 'textfield',
    '#title' => t('Date Captured'),
    '#description' => t('Default Value: //mods:originInfo/mods:dateCaptured <br/>XPath to use for the date_captured.'),
    '#states' => array(
      'visible' => array(
        ':input[name="islandora_scholar_xpaths_enabled"]' => array('checked' => TRUE),
      ),
    ),
    '#ajax' => array(
      'callback' => 'islandora_scholar_xpaths_citeproc_callback',
      'wrapper' => 'islandora_scholar_xpaths_citeproc_title',
    ),
   '#field_suffix' => (!empty($form_state['values']['islandora_scholar_xpaths_demo']) ? _mods_id($form_state['values']['islandora_scholar_xpaths_dateCaptured'], t($form_state['values']['islandora_scholar_xpaths_demo'])) : t("No PID entered to verify MODS validity.")),
  ));
  $form['islandora_scholar_xpaths_citeproc'] += _islandora_scholar_add_variable('islandora_scholar_xpaths_local_author', '//mods:mods[1]/mods:name[normalize-space(mods:namePart)]', array(
    '#type' => 'textfield',
    '#title' => t('Names'),
    '#description' => t('Default Value: //mods:mods[1]/mods:name[normalize-space(mods:namePart)] <br/>XPath to use for the names.'),
    '#states' => array(
      'visible' => array(
        ':input[name="islandora_scholar_xpaths_enabled"]' => array('checked' => TRUE),
      ),
    ),
    '#ajax' => array(
      'callback' => 'islandora_scholar_xpaths_citeproc_callback',
      'wrapper' => 'islandora_scholar_xpaths_citeproc_title',
    ),
   '#field_suffix' => (!empty($form_state['values']['islandora_scholar_xpaths_demo']) ? _mods_id($form_state['values']['islandora_scholar_xpaths_local_author'], t($form_state['values']['islandora_scholar_xpaths_demo'])) : t("No PID entered to verify MODS validity.")),
  ));
  $form['islandora_scholar_xpaths_citeproc'] += _islandora_scholar_add_variable('islandora_scholar_xpaths_local_container-author', '//mods:mods[1]/mods:relatedItem[@type="host"]/mods:name', array(
    '#type' => 'textfield',
    '#title' => t('Container Author'),
    '#description' => t('Default Value: //mods:mods[1]/mods:relatedItem[@type="host"]/mods:name <br/>XPath to use for the container-author.'),
    '#states' => array(
      'visible' => array(
        ':input[name="islandora_scholar_xpaths_enabled"]' => array('checked' => TRUE),
      ),
    ),
    '#ajax' => array(
      'callback' => 'islandora_scholar_xpaths_citeproc_callback',
      'wrapper' => 'islandora_scholar_xpaths_citeproc_title',
    ),
   '#field_suffix' => (!empty($form_state['values']['islandora_scholar_xpaths_demo']) ? _mods_id($form_state['values']['islandora_scholar_xpaths_local_container-author'], t($form_state['values']['islandora_scholar_xpaths_demo'])) : t("No PID entered to verify MODS validity.")),
  ));
  $form['islandora_scholar_xpaths_citeproc'] += _islandora_scholar_add_variable('islandora_scholar_xpaths_local_collection-editor', '//mods:mods[1]/mods:relatedItem[@type="series"]/mods:name', array(
    '#type' => 'textfield',
    '#title' => t('Collection Editor'),
    '#description' => t('Default Value: //mods:mods[1]/mods:relatedItem[@type="series"]/mods:name <br/>XPath to use for the collection-editor.'),
    '#states' => array(
      'visible' => array(
        ':input[name="islandora_scholar_xpaths_enabled"]' => array('checked' => TRUE),
      ),
    ),
    '#ajax' => array(
      'callback' => 'islandora_scholar_xpaths_citeproc_callback',
      'wrapper' => 'islandora_scholar_xpaths_citeproc_title',
    ),
   '#field_suffix' => (!empty($form_state['values']['islandora_scholar_xpaths_demo']) ? _mods_id($form_state['values']['islandora_scholar_xpaths_local_collection-editor'], t($form_state['values']['islandora_scholar_xpaths_demo'])) : t("No PID entered to verify MODS validity.")),
  ));
  $form['islandora_scholar_xpaths_citeproc'] += _islandora_scholar_add_variable('islandora_scholar_xpaths_role_term', 'mods:role/mods:roleTerm', array(
    '#type' => 'textfield',
    '#title' => t('Role Term'),
    '#description' => t('Default Value: mods:role/mods:roleTerm <br/>XPath to use for the role_term.'),
    '#states' => array(
      'visible' => array(
        ':input[name="islandora_scholar_xpaths_enabled"]' => array('checked' => TRUE),
      ),
    ),
    '#ajax' => array(
      'callback' => 'islandora_scholar_xpaths_citeproc_callback',
      'wrapper' => 'islandora_scholar_xpaths_citeproc_title',
    ),
   '#field_suffix' => (!empty($form_state['values']['islandora_scholar_xpaths_demo']) ? _mods_id($form_state['values']['islandora_scholar_xpaths_role_term'], t($form_state['values']['islandora_scholar_xpaths_demo'])) : t("No PID entered to verify MODS validity.")),
  ));
  module_load_include('inc', 'islandora', 'includes/solution_packs');
  return system_settings_form($form);
}

/**
 * Admin form validate function.
 */
function islandora_scholar_admin_pdf_form_validate($form, &$form_state) {
  if ($form_state['values']['islandora_scholar_create_fulltext']) {
    $islandora_scholar_path_to_pdftotext = $form_state['values']['islandora_scholar_path_to_pdftotext'];
    exec($islandora_scholar_path_to_pdftotext, $output, $return_value);
    if ($return_value != 99) {
      form_set_error('islandora_scholar_path_to_pdftotext', t('Cannot extract text from PDF without a valid path to pdftotext.'));
    }
  }
}

/**
 * Function updates PDF to text URL div.
 */
function islandora_scholar_update_pdftotext_url_div($form, $form_state) {
  $form_state['rebuild'] = TRUE;
  return $form['islandora_scholar_url_fieldset']['wrapper'];
}

/**
 * Function removes and resets the custom XPaths.
 */
function _islandora_scholar_xpaths_reset() {
  $islandora_scholar_xpaths = islandora_scholar_xpaths_list();
  $message = t('All custom Islandora Scholar XPaths have been reset.');
  drupal_set_message(check_plain($message));
  array_walk($islandora_scholar_xpaths, 'variable_del');
}

/**
 * Function specifies the array of intputs.
 */
function islandora_scholar_xpaths_list() {
  $islandora_scholar_xpaths = array(
    'islandora_scholar_xpaths_abstract',
    'islandora_scholar_xpaths_authors_xpath',
    'islandora_scholar_xpaths_call-number',
    'islandora_scholar_xpaths_collection-title',
    'islandora_scholar_xpaths_container-title',
    'islandora_scholar_xpaths_created_date',
    'islandora_scholar_xpaths_dateCaptured',
    'islandora_scholar_xpaths_date_issued',
    'islandora_scholar_xpaths_degree_grantor',
    'islandora_scholar_xpaths_department',
    'islandora_scholar_xpaths_DOI',
    'islandora_scholar_xpaths_edition',
    'islandora_scholar_xpaths_embargo_date',
    'islandora_scholar_xpaths_enabled',
    'islandora_scholar_xpaths_end_page',
    'islandora_scholar_xpaths_event',
    'islandora_scholar_xpaths_event_place',
    'islandora_scholar_xpaths_event_place_property',
    'islandora_scholar_xpaths_event_property',
    'islandora_scholar_xpaths_family',
    'islandora_scholar_xpaths_genre',
    'islandora_scholar_xpaths_given',
    'islandora_scholar_xpaths_host_title',
    'islandora_scholar_xpaths_ISBN',
    'islandora_scholar_xpaths_issn',
    'islandora_scholar_xpaths_issue',
    'islandora_scholar_xpaths_local_author',
    'islandora_scholar_xpaths_local_collection-editor',
    'islandora_scholar_xpaths_local_containimg src="https://digital.lib.utk.edu/collections/islandora/object/acwiley%3A280/datastream/TN/view"></a>',
    'islandora_scholar_xpaths_number',
    'islandora_scholar_xpaths_number-nihmsid',
    'islandora_scholar_xpaths_number-pmcid',
    'islandora_scholar_xpaths_number-pmid',
    'islandora_scholar_xpaths_online_date',
    'islandora_scholar_xpaths_origin_date',
    'islandora_scholar_xpaths_pages',
    'islandora_scholar_xpaths_part_date',
    'islandora_scholar_xpaths_publisher',
    'islandora_scholar_xpaths_publisher-place',
    'islandora_scholar_xpaths_related_date',
    'islandora_scholar_xpaths_role_term',
    'islandora_scholar_xpaths_season',
    'islandora_scholar_xpaths_start_page',
    'islandora_scholar_xpaths_title',
    'islandora_scholar_xpaths_title_nonsort',
    'islandora_scholar_xpaths_title_results',
    'islandora_scholar_xpaths_title_subtitle',
    'islandora_scholar_xpaths_title_sub_title',
    'islandora_scholar_xpaths_types',
    'islandora_scholar_xpaths_types_local_auth',
    'islandora_scholar_xpaths_type_marcgt',
    'islandora_scholar_xpaths_type_marcgt_host_titles',
    'islandora_scholar_xpaths_type_marcgt_related',
    'islandora_scholar_xpaths_URL',
    'islandora_scholar_xpaths_volume',
  );
  return $islandora_scholar_xpaths;
}

/**
 * Ajax callback function for fields.
 *
 * Used by 'demo image', input.
 *
 * @param array $form
 *   An associative array containing the form definition.
 * @param array $form_state
 *   An associative array containing the form state.
 *
 * @return array
 *   The part of the form array that needs asynchronous updating.
 */
function islandora_scholar_xpaths_image_callback($form, &$form_state) {
  $form_state['rebuild'] = TRUE;
  return $form['islandora_scholar_xpaths_demo'];
}

/**
 * Ajax callback function for general fields.
 *
 * Used by 'islandora_scholar_xpaths', input.
 *
 * @param array $form
 *   An associative array containing the form definition.
 * @param array $form_state
 *   An associative array containing the form state.
 *
 * @return array
 *   The part of the form array that needs asynchronous updating.
 */
function islandora_scholar_xpaths_general_callback($form, &$form_state) {
  $form_state['rebuild'] = TRUE;
  return $form['islandora_scholar_xpaths_general'];
}
/**
 * Ajax callback function for google scholar fields.
 *
 * Used by 'islandora_scholar_xpaths', input.
 *
 * @param array $form
 *   An associative array containing the form definition.
 * @param array $form_state
 *   An associative array containing the form state.
 *
 * @return array
 *   The part of the form array that needs asynchronous updating.
 */

function islandora_scholar_xpaths_google_callback($form, &$form_state) {
  $form_state['rebuild'] = TRUE;
  return $form['islandora_scholar_xpaths_google'];
}
/**
 * Ajax callback function for coins fields.
 *
 * Used by 'islandora_scholar_xpaths', input.
 *
 * @param array $form
 *   An associative array containing the form definition.
 * @param array $form_state
 *   An associative array containing the form state.
 *
 * @return array
 *   The part of the form array that needs asynchronous updating.
 */

function islandora_scholar_xpaths_coins_callback($form, &$form_state) {
  $form_state['rebuild'] = TRUE;
  return $form['islandora_scholar_xpaths_coins'];
}
/**
 * Ajax callback function for citeproc fields.
 *
 * Used by 'islandora_scholar_xpaths', input.
 *
 * @param array $form
 *   An associative array containing the form definition.
 * @param array $form_state
 *   An associative array containing the form state.
 *
 * @return array
 *   The part of the form array that needs asynchronous updating.
 */

function islandora_scholar_xpaths_citeproc_callback($form, &$form_state) {
  $form_state['rebuild'] = TRUE;
  return $form['islandora_scholar_xpaths_citeproc'];
}

/**
 * AJAX callback function for 'on blurr' validation.
 *
 * Passes values from the field, loads the object's corisponding string,
 * determines is valid and populates the dialog accordingly.
 *
 * @param array $mods_value
 *   A string specifying the MODS value to test.
 * @param array $pid
 *   A string specifying the form_id.
 *
 * @return array
 *   An array of AJAX commands.
 */
function _mods_id ($mods_value = NULL, $pid) {
  $object = islandora_object_load(t($pid));
  $xmlstr = $object['MODS']->content;
  $mods_doc = new DOMDocument();
  $mods_doc->loadXML($object['MODS']->content);
  $mods_xpath = new DOMXPath($mods_doc);
  $mods_xpath->registerNamespace('m', 'http://www.loc.gov/mods/v3');
  $lookup_results = $mods_xpath->query($mods_value);
  $build_string = '';

  // Test inputs for validity and to avoid a 500 error.
  if (strlen($mods_value) == 0) {
    $build_string = 'For pid "' . $pid . '" <span style="color:red">Path cannot be blank</span>';
  } elseif ($lookup_results == NULL) {
    $build_string = 'For pid "' . $pid . '" <span style="color:red">ERROR</span>';
  } elseif (strlen($lookup_results->item(0)->textContent) == 0) {
    $build_string = 'For pid "' . $pid . '" <span style="color:orange">Nothing Found</span>';
  } else {
    $build_string = 'For pid "' . $pid . '": <span style="color:blue">' . $lookup_results->item(0)->textContent . '</span>';
  }

  return t($build_string);
}
