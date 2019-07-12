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
    'google_scholar_metatags' => array(
      '#type' => 'fieldset',
      '#title' => t('Google Scholar metatag configuration'),
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

  $form['google_scholar_metatags'] += _islandora_scholar_add_variable('islandora_scholar_google_scholar_metatags_enabled', TRUE, array(
    '#type' => 'checkbox',
    '#title' => t('Enable Google Scholar metatags on Citation/Thesis objects'),
  ));
  $form['google_scholar_search'] += _islandora_scholar_add_variable('islandora_scholar_google_scholar_search_enabled', FALSE, array(
    '#type' => 'checkbox',
    '#title' => t('Render Google Scholar Search link'),
    '#description' => t('Enable Searching in Google Scholar.'),
  ));
  $form['google_scholar_search'] += _islandora_scholar_add_variable('islandora_scholar_google_scholar_primary_search_xpath', '//mods:mods[1]/mods:identifier[@type="doi"]', array(
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
  $confirmation_message = '<img src="' . url('misc/watchdog-ok.png') . '"/>'
    . t('pdftotext executable found at !url', array('!url' => "<strong>$islandora_scholar_path_to_pdftotext</strong>"));
  if (_islandora_scholar_check_path($islandora_scholar_path_to_pdftotext) != 99) {
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

  $form['islandora_scholar_thumbnail_fieldset']['islandora_scholar_thumbnail_colorspace'] = array(
    '#type' => 'select',
    '#title' => t('Colorspace'),
    '#options' => array(
      'sRGB' => t('sRGB'),
      'RGB' => t('RGB'),
      'CMYK' => t('CMYK'),
      'Gray' => t('Grayscale'),
    ),
    '#description' => t('The colorspace for the derived thumbnail.'),
    '#default_value' => variable_get('islandora_scholar_thumbnail_colorspace', 'sRGB'),
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

  $form['islandora_scholar_preview_fieldset']['islandora_scholar_preview_colorspace'] = array(
    '#type' => 'select',
    '#title' => t('Colorspace'),
    '#options' => array(
      'sRGB' => t('sRGB'),
      'RGB' => t('RGB'),
      'CMYK' => t('CMYK'),
      'Gray' => t('Grayscale'),
    ),
    '#description' => t('The colorspace for the derived preview.'),
    '#default_value' => variable_get('islandora_scholar_preview_colorspace', 'sRGB'),
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
  form_load_include($form_state, 'inc', 'islandora_scholar', 'includes/admin.form');
  if (!empty($form_state['values']['islandora_scholar_xpaths_demo'])) {
    $islandora_scholar_xpath_pid = $form_state['values']['islandora_scholar_xpaths_demo'];
    exec($islandora_scholar_xpath_pid, $output, $return_value);
    $islandora_scholar_xpaths_general_description = t('These XPaths are shared within most of the submodules. To see the MODS value after editing an entry, tab to the next field. <span style="color:blue">Testing against pid:</span> <span style="color:green">!pid</span> <img src="', array(
        '!pid' => "$islandora_scholar_xpath_pid",
      )) . url('misc/watchdog-ok.png') . '"/><br/><br/>';
    $islandora_scholar_xpaths_google_description = t('Not limited to Google Scholar, most of these are used by Coins and Citeproc as well. To see the MODS value after editing an entry, tab to the next field. <span style="color:blue">Testing against pid:</span> <span style="color:green">!pid</span> <img src="', array(
        '!pid' => "$islandora_scholar_xpath_pid",
      )) . url('misc/watchdog-ok.png') . '"/><br/><br/>';
    $islandora_scholar_xpaths_coins_description = t('XPaths for the construction of COINS URLs. To see the MODS value after editing an entry, tab to the next field. <span style="color:blue">Testing against pid:</span> <span style="color:green">!pid</span> <img src="', array(
        '!pid' => "$islandora_scholar_xpath_pid",
      )) . url('misc/watchdog-ok.png') . '"/><br/><br/>';
    $islandora_scholar_xpaths_citeproc_description = t('MODS to Citeproc JSON Submodule Converter Xpath configurations <span style="color:blue">Testing against pid:</span> <span style="color:green">!pid</span> <img src="', array(
        '!pid' => "$islandora_scholar_xpath_pid",
      )) . url('misc/watchdog-ok.png') . '"/><br/><br/>';
  }
  else {
    $islandora_scholar_xpaths_general_description = t('These XPaths are shared within most of the submodules. To see the MODS value after editing an entry, tab to the next field.');
    $islandora_scholar_xpaths_google_description = t('These XPaths are shared within most of the submodules. To see the MODS value after editing an entry, tab to the next field.');
    $islandora_scholar_xpaths_coins_description = t('These XPaths are shared within most of the submodules. To see the MODS value after editing an entry, tab to the next field.');
    $islandora_scholar_xpaths_citeproc_description = t('These XPaths are shared within most of the submodules. To see the MODS value after editing an entry, tab to the next field.');
  }
  $form = array(
    'islandora_scholar_xpaths' => array(
      '#type' => 'fieldset',
      '#title' => t('Islandora Scholar Xpath configuration'),
      '#description' => t("Set custom XPaths for Islandora Scholar. <br/>"),
      '#prefix' => '<div id="islandora_scholar_xpaths_title">',
      '#suffix' => '</div>',
      '#tree' => FALSE,
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
      '#prefix' => '<div id="islandora_scholar_xpaths_demo">',
      '#suffix' => '</div>',
      '#tree' => FALSE,
    ),
    'islandora_scholar_xpaths_general' => array(
      '#type' => 'fieldset',
      '#title' => t('General Xpath configurations'),
      '#description' => t('!description_message',
        array(
          '!description_message' => $islandora_scholar_xpaths_general_description,
        )),
      '#collapsible' => TRUE,
      '#states' => array(
        'visible' => array(
          ':input[name="islandora_scholar_xpaths_enabled"]' => array('checked' => TRUE),
        ),
      ),
      '#prefix' => '<div id="islandora_scholar_xpaths_general_title">',
      '#suffix' => '</div>',
      '#tree' => FALSE,
    ),
    'islandora_scholar_xpaths_google' => array(
      '#type' => 'fieldset',
      '#title' => t('Google Scholar Submodule Xpath configurations'),
      '#description' => t('!description_message',
        array(
          '!description_message' => $islandora_scholar_xpaths_google_description,
        )),
      '#collapsible' => TRUE,
      '#states' => array(
        'visible' => array(
          ':input[name="islandora_scholar_xpaths_enabled"]' => array('checked' => TRUE),
        ),
      ),
      '#prefix' => '<div id="islandora_scholar_xpaths_google_title">',
      '#suffix' => '</div>',
      '#tree' => FALSE,
    ),
    'islandora_scholar_xpaths_coins' => array(
      '#type' => 'fieldset',
      '#title' => t('Coins Submodule Xpath configurations'),
      '#description' => t('!description_message',
        array(
          '!description_message' => $islandora_scholar_xpaths_coins_description,
        )),
      '#collapsible' => TRUE,
      '#states' => array(
        'visible' => array(
          ':input[name="islandora_scholar_xpaths_enabled"]' => array('checked' => TRUE),
        ),
      ),
      '#prefix' => '<div id="islandora_scholar_xpaths_coins_title">',
      '#suffix' => '</div>',
      '#tree' => FALSE,
    ),
    'islandora_scholar_xpaths_citeproc' => array(
      '#type' => 'fieldset',
      '#title' => t('Citeproc Submodule Xpath configurations'),
      '#description' => t('!description_message',
        array(
          '!description_message' => $islandora_scholar_xpaths_citeproc_description,
        )),
      '#collapsible' => TRUE,
      '#states' => array(
        'visible' => array(
          ':input[name="islandora_scholar_xpaths_enabled"]' => array('checked' => TRUE),
        ),
      ),
      '#prefix' => '<div id="islandora_scholar_xpaths_citeproc_title">',
      '#suffix' => '</div>',
      '#tree' => FALSE,
    ),
  );
  $form['islandora_scholar_xpaths_demo'] = array(
    '#type' => 'textfield',
    '#title' => t('Demo Object'),
    '#field_suffix' => (!empty($form_state['values']['islandora_scholar_xpaths_demo']) && (islandora_object_access(ISLANDORA_VIEW_OBJECTS, islandora_object_load($form_state['values']['islandora_scholar_xpaths_demo'])) == 1)) ? islandora_scholar_xpaths_demo_prefix_callback($form_state['values']['islandora_scholar_xpaths_demo']) : t("Modify to refresh value (PID for demo object is required)."),
    '#description' => t('PID to check these values against. @option', array('@option' => ((!empty($form_state['values']['islandora_scholar_xpaths_demo'])) && (islandora_object_access(ISLANDORA_VIEW_OBJECTS, islandora_object_load($form_state['values']['islandora_scholar_xpaths_demo'])) == 1) ? $form_state['values']['islandora_scholar_xpaths_demo'] : ''))),
    '#states' => array(
      'visible' => array(
        ':input[name="islandora_scholar_xpaths_enabled"]' => array('checked' => TRUE),
      ),
    ),
    '#autocomplete_path' => 'islandora_compound_object/autocomplete/child',
    '#ajax' => array(
      'callback' => 'islandora_scholar_xpaths_image_callback',
      'wrapper' => 'islandora_scholar_xpaths_demo_image',
    ),
    '#prefix' => '<div id="islandora_scholar_xpaths_demo_image">',
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
  $form['islandora_scholar_xpaths_general']['reset'] = array(
    '#type' => 'button',
    '#value' => t('Get Xpaths Values'),
    '#attributes' => array(
      "onclick" => 'foreach (islandora_scholar_xpaths_list() as $key => $value) { document.querySelectorAll(".form-text").focus(); }'),
    '#states' => array(
      'visible' => array(
        ':input[name="islandora_scholar_xpaths_enabled"]' => array('checked' => TRUE),
      ),
    ),
  );
  $form['islandora_scholar_xpaths_general'] += _islandora_scholar_add_variable('islandora_scholar_xpaths_title', '//mods:mods[1]/mods:titleInfo/mods:title', array(
    '#type' => 'textfield',
    '#title' => t('Title'),
    '#description' => t('Default Value: //mods:mods[1]/mods:titleInfo/mods:title'),
    '#states' => array(
      'visible' => array(
        ':input[name="islandora_scholar_xpaths_enabled"]' => array('checked' => TRUE),
      ),
    ),
    '#ajax' => array(
      'callback' => 'islandora_scholar_xpaths_general_callback',
      'wrapper' => 'islandora_scholar_xpaths_general_title',
    ),
   '#field_suffix' => (!empty($form_state['values']['islandora_scholar_xpaths_demo']) ? _mods_id('islandora_scholar_xpaths_title', $form_state['values']['islandora_scholar_xpaths_title'], check_plain($form_state['values']['islandora_scholar_xpaths_demo'])) : t("Modify to refresh value (PID for demo object is required).")),
  ));
  $form['islandora_scholar_xpaths_general'] += _islandora_scholar_add_variable('islandora_scholar_xpaths_title_sub_title', '//mods:mods[1]//mods:subTitle', array(
    '#type' => 'textfield',
    '#title' => t('Subtitle'),
    '#description' => t('Default Value: //mods:mods[1]//mods:subTitle'),
    '#states' => array(
      'visible' => array(
        ':input[name="islandora_scholar_xpaths_enabled"]' => array('checked' => TRUE),
      ),
    ),
    '#ajax' => array(
      'callback' => 'islandora_scholar_xpaths_general_callback',
      'wrapper' => 'islandora_scholar_xpaths_general_title',
    ),
    '#field_suffix' => (!empty($form_state['values']['islandora_scholar_xpaths_demo']) ? _mods_id('islandora_scholar_xpaths_title_sub_title', $form_state['values']['islandora_scholar_xpaths_title_sub_title'], check_plain($form_state['values']['islandora_scholar_xpaths_demo'])) : t("Modify to refresh value (PID for demo object is required).")),
  ));
  $form['islandora_scholar_xpaths_general'] += _islandora_scholar_add_variable('islandora_scholar_xpaths_abstract', '//mods:mods[1]/mods:abstract', array(
    '#type' => 'textfield',
    '#title' => t('Abstract'),
    '#description' => t('Default Value: //mods:mods[1]/mods:abstract'),
    '#states' => array(
      'visible' => array(
        ':input[name="islandora_scholar_xpaths_enabled"]' => array('checked' => TRUE),
      ),
    ),
    '#ajax' => array(
      'callback' => 'islandora_scholar_xpaths_general_callback',
      'wrapper' => 'islandora_scholar_xpaths_general_title',
    ),
    '#field_suffix' => (!empty($form_state['values']['islandora_scholar_xpaths_demo']) ? _mods_id('islandora_scholar_xpaths_abstract', $form_state['values']['islandora_scholar_xpaths_abstract'], check_plain($form_state['values']['islandora_scholar_xpaths_demo'])) : t("Modify to refresh value (PID for demo object is required).")),
  ));
  $form['islandora_scholar_xpaths_general'] += _islandora_scholar_add_variable('islandora_scholar_xpaths_family', '//mods:mods[1]/mods:name[mods:role/mods:roleTerm = "author"]/mods:namePart[@type = "family"]', array(
    '#type' => 'textfield',
    '#title' => t('Family Name'),
    '#description' => t('Default Value: //mods:mods[1]/mods:name[mods:role/mods:roleTerm = "author"]/mods:namePart[@type = "family"]'),
    '#states' => array(
      'visible' => array(
        ':input[name="islandora_scholar_xpaths_enabled"]' => array('checked' => TRUE),
      ),
    ),
    '#ajax' => array(
      'callback' => 'islandora_scholar_xpaths_general_callback',
      'wrapper' => 'islandora_scholar_xpaths_general_title',
    ),
    '#field_suffix' => (!empty($form_state['values']['islandora_scholar_xpaths_demo']) ? _mods_id('islandora_scholar_xpaths_family', $form_state['values']['islandora_scholar_xpaths_family'], check_plain($form_state['values']['islandora_scholar_xpaths_demo'])) : t("Modify to refresh value (PID for demo object is required).")),
  ));
  $form['islandora_scholar_xpaths_general'] += _islandora_scholar_add_variable('islandora_scholar_xpaths_given', '//mods:mods[1]/mods:name[mods:role/mods:roleTerm = "author"]/mods:namePart[@type = "given"]', array(
    '#type' => 'textfield',
    '#title' => t('Given Name'),
    '#description' => t("Default Value: //mods:mods[1]/mods:name[mods:role/mods:roleTerm = \"author\"]//mods:namePart[@type = 'given']"),
    '#states' => array(
      'visible' => array(
        ':input[name="islandora_scholar_xpaths_enabled"]' => array('checked' => TRUE),
      ),
    ),
    '#ajax' => array(
      'callback' => 'islandora_scholar_xpaths_general_callback',
      'wrapper' => 'islandora_scholar_xpaths_general_title',
    ),
    '#field_suffix' => (!empty($form_state['values']['islandora_scholar_xpaths_demo']) ? _mods_id('islandora_scholar_xpaths_given', $form_state['values']['islandora_scholar_xpaths_given'], check_plain($form_state['values']['islandora_scholar_xpaths_demo'])) : t("Modify to refresh value (PID for demo object is required).")),
  ));
  $form['islandora_scholar_xpaths_general'] += _islandora_scholar_add_variable('islandora_scholar_xpaths_authors_xpath', '//mods:mods[1]/mods:name/mods:role[mods:roleTerm = "author"]/../mods:namePart[@type="family"]', array(
    '#type' => 'textfield',
    '#title' => t('Authors'),
    '#description' => t('Default Value: //mods:mods[1]/mods:name/mods:role[mods:roleTerm = "author"]/../mods:namePart[@type="family"]'),
    '#states' => array(
      'visible' => array(
        ':input[name="islandora_scholar_xpaths_enabled"]' => array('checked' => TRUE),
      ),
    ),
    '#ajax' => array(
      'callback' => 'islandora_scholar_xpaths_general_callback',
      'wrapper' => 'islandora_scholar_xpaths_general_title',
    ),
    '#field_suffix' => (!empty($form_state['values']['islandora_scholar_xpaths_demo']) ? _mods_id('islandora_scholar_xpaths_authors_xpath', $form_state['values']['islandora_scholar_xpaths_authors_xpath'], check_plain($form_state['values']['islandora_scholar_xpaths_demo'])) : t("Modify to refresh value (PID for demo object is required).")),
  ));
  $form['islandora_scholar_xpaths_general'] += _islandora_scholar_add_variable('islandora_scholar_xpaths_created_date', '//mods:mods[1]/mods:originInfo/mods:dateCreated', array(
    '#type' => 'textfield',
    '#title' => t('Created Date'),
    '#description' => t('Default Value: //mods:mods[1]/mods:originInfo/mods:dateCreated'),
    '#states' => array(
      'visible' => array(
        ':input[name="islandora_scholar_xpaths_enabled"]' => array('checked' => TRUE),
      ),
    ),
    '#ajax' => array(
      'callback' => 'islandora_scholar_xpaths_general_callback',
      'wrapper' => 'islandora_scholar_xpaths_general_title',
    ),
    '#field_suffix' => (!empty($form_state['values']['islandora_scholar_xpaths_demo']) ? _mods_id('islandora_scholar_xpaths_created_date', $form_state['values']['islandora_scholar_xpaths_created_date'], check_plain($form_state['values']['islandora_scholar_xpaths_demo'])) : t("Modify to refresh value (PID for demo object is required).")),
  ));
  $form['islandora_scholar_xpaths_google'] += _islandora_scholar_add_variable('islandora_scholar_xpaths_degree_grantor', '//mods:mods[1]/mods:name[@type="corporate"][mods:role/mods:roleTerm = "Degree grantor"]/mods:namePart', array(
    '#type' => 'textfield',
    '#title' => t('Degree Grantor'),
    '#description' => t('Default Value: //mods:mods[1]/mods:name[@type="corporate"][mods:role/mods:roleTerm = "Degree grantor"]/mods:namePart'),
    '#states' => array(
      'visible' => array(
        ':input[name="islandora_scholar_xpaths_enabled"]' => array('checked' => TRUE),
      ),
    ),
    '#ajax' => array(
      'callback' => 'islandora_scholar_xpaths_google_callback',
      'wrapper' => 'islandora_scholar_xpaths_google_title',
    ),
   '#field_suffix' => (!empty($form_state['values']['islandora_scholar_xpaths_demo']) ? _mods_id('islandora_scholar_xpaths_degree_grantor', $form_state['values']['islandora_scholar_xpaths_degree_grantor'], check_plain($form_state['values']['islandora_scholar_xpaths_demo'])) : t("Modify to refresh value (PID for demo object is required).")),
  ));
  $form['islandora_scholar_xpaths_google'] += _islandora_scholar_add_variable('islandora_scholar_xpaths_genre', '//mods:mods[1]/mods:genre', array(
    '#type' => 'textfield',
    '#title' => t('Genre'),
    '#description' => t('Default Value: //mods:mods[1]/mods:genre'),
    '#states' => array(
      'visible' => array(
        ':input[name="islandora_scholar_xpaths_enabled"]' => array('checked' => TRUE),
      ),
    ),
    '#ajax' => array(
      'callback' => 'islandora_scholar_xpaths_google_callback',
      'wrapper' => 'islandora_scholar_xpaths_google_title',
    ),
   '#field_suffix' => (!empty($form_state['values']['islandora_scholar_xpaths_demo']) ? _mods_id('islandora_scholar_xpaths_genre', $form_state['values']['islandora_scholar_xpaths_genre'], check_plain($form_state['values']['islandora_scholar_xpaths_demo'])) : t("Modify to refresh value (PID for demo object is required).")),
  ));
  $form['islandora_scholar_xpaths_google'] += _islandora_scholar_add_variable('islandora_scholar_xpaths_host_title', '//mods:mods[1]/mods:relatedItem[@type="host"]//mods:title', array(
    '#type' => 'textfield',
    '#title' => t('Host Title'),
    '#description' => t('Default Value: //mods:mods[1]/mods:relatedItem[@type="host"]//mods:title'),
    '#states' => array(
      'visible' => array(
        ':input[name="islandora_scholar_xpaths_enabled"]' => array('checked' => TRUE),
      ),
    ),
    '#ajax' => array(
      'callback' => 'islandora_scholar_xpaths_google_callback',
      'wrapper' => 'islandora_scholar_xpaths_google_title',
    ),
   '#field_suffix' => (!empty($form_state['values']['islandora_scholar_xpaths_demo']) ? _mods_id('islandora_scholar_xpaths_host_title', $form_state['values']['islandora_scholar_xpaths_host_title'], check_plain($form_state['values']['islandora_scholar_xpaths_demo'])) : t("Modify to refresh value (PID for demo object is required).")),
  ));
  $form['islandora_scholar_xpaths_google'] += _islandora_scholar_add_variable('islandora_scholar_xpaths_issn', '//mods:mods[1]/mods:identifier[@type="issn"]', array(
    '#type' => 'textfield',
    '#title' => t('ISSN'),
    '#description' => t('Default Value: //mods:mods[1]/mods:identifier[@type="issn"]'),
    '#states' => array(
      'visible' => array(
        ':input[name="islandora_scholar_xpaths_enabled"]' => array('checked' => TRUE),
      ),
    ),
    '#ajax' => array(
      'callback' => 'islandora_scholar_xpaths_google_callback',
      'wrapper' => 'islandora_scholar_xpaths_google_title',
    ),
   '#field_suffix' => (!empty($form_state['values']['islandora_scholar_xpaths_demo']) ? _mods_id('islandora_scholar_xpaths_issn', $form_state['values']['islandora_scholar_xpaths_issn'], check_plain($form_state['values']['islandora_scholar_xpaths_demo'])) : t("Modify to refresh value (PID for demo object is required).")),
  ));
  $form['islandora_scholar_xpaths_google'] += _islandora_scholar_add_variable('islandora_scholar_xpaths_issue', '//mods:mods[1]/mods:part/mods:detail[@type="issue"]/mods:number', array(
    '#type' => 'textfield',
    '#title' => t('Issue'),
    '#description' => t('Default Value: //mods:mods[1]/mods:part/mods:detail[@type="issue"]/mods:number'),
    '#states' => array(
      'visible' => array(
        ':input[name="islandora_scholar_xpaths_enabled"]' => array('checked' => TRUE),
      ),
    ),
    '#ajax' => array(
      'callback' => 'islandora_scholar_xpaths_google_callback',
      'wrapper' => 'islandora_scholar_xpaths_google_title',
    ),
   '#field_suffix' => (!empty($form_state['values']['islandora_scholar_xpaths_demo']) ? _mods_id('islandora_scholar_xpaths_issue', $form_state['values']['islandora_scholar_xpaths_issue'], check_plain($form_state['values']['islandora_scholar_xpaths_demo'])) : t("Modify to refresh value (PID for demo object is required).")),
  ));
  $form['islandora_scholar_xpaths_google'] += _islandora_scholar_add_variable('islandora_scholar_xpaths_online_date', '//mods:mods[1]/mods:recordInfo/mods:recordCreationDate', array(
    '#type' => 'textfield',
    '#title' => t('Online Date'),
    '#description' => t('Default Value: //mods:mods[1]/mods:recordInfo/mods:recordCreationDate'),
    '#states' => array(
      'visible' => array(
        ':input[name="islandora_scholar_xpaths_enabled"]' => array('checked' => TRUE),
      ),
    ),
    '#ajax' => array(
      'callback' => 'islandora_scholar_xpaths_google_callback',
      'wrapper' => 'islandora_scholar_xpaths_google_title',
    ),
   '#field_suffix' => (!empty($form_state['values']['islandora_scholar_xpaths_demo']) ? _mods_id('islandora_scholar_xpaths_online_date', $form_state['values']['islandora_scholar_xpaths_online_date'], check_plain($form_state['values']['islandora_scholar_xpaths_demo'])) : t("Modify to refresh value (PID for demo object is required).")),
  ));
  $form['islandora_scholar_xpaths_coins'] += _islandora_scholar_add_variable('islandora_scholar_xpaths_department', '//mods:mods[1]/mods:identifier[@type="u2"]', array(
    '#type' => 'textfield',
    '#title' => t('Department'),
    '#description' => t('Default Value: //mods:mods[1]/mods:identifier[@type="u2"]'),
    '#states' => array(
      'visible' => array(
        ':input[name="islandora_scholar_xpaths_enabled"]' => array('checked' => TRUE),
      ),
    ),
    '#ajax' => array(
      'callback' => 'islandora_scholar_xpaths_coins_callback',
      'wrapper' => 'islandora_scholar_xpaths_coins_title',
    ),
   '#field_suffix' => (!empty($form_state['values']['islandora_scholar_xpaths_demo']) ? _mods_id('islandora_scholar_xpaths_department', $form_state['values']['islandora_scholar_xpaths_department'], check_plain($form_state['values']['islandora_scholar_xpaths_demo'])) : t("Modify to refresh value (PID for demo object is required).")),
  ));
  $form['islandora_scholar_xpaths_coins'] += _islandora_scholar_add_variable('islandora_scholar_xpaths_related_date', '//mods:mods[1]/mods:relatedItem[@type="host"]//mods:date', array(
    '#type' => 'textfield',
    '#title' => t('Related Date'),
    '#description' => t('Default Value: //mods:mods[1]/mods:relatedItem[@type="host"]//mods:date'),
    '#states' => array(
      'visible' => array(
        ':input[name="islandora_scholar_xpaths_enabled"]' => array('checked' => TRUE),
      ),
    ),
    '#ajax' => array(
      'callback' => 'islandora_scholar_xpaths_coins_callback',
      'wrapper' => 'islandora_scholar_xpaths_coins_title',
    ),
   '#field_suffix' => (!empty($form_state['values']['islandora_scholar_xpaths_demo']) ? _mods_id('islandora_scholar_xpaths_related_date', $form_state['values']['islandora_scholar_xpaths_related_date'], check_plain($form_state['values']['islandora_scholar_xpaths_demo'])) : t("Modify to refresh value (PID for demo object is required).")),
  ));
  $form['islandora_scholar_xpaths_coins'] += _islandora_scholar_add_variable('islandora_scholar_xpaths_start_page', '//mods:mods[1]//mods:extent[@unit="page"]/mods:start', array(
    '#type' => 'textfield',
    '#title' => t('Start Page'),
    '#description' => t('Default Value: //mods:mods[1]//mods:extent[@unit="page"]/mods:start'),
    '#states' => array(
      'visible' => array(
        ':input[name="islandora_scholar_xpaths_enabled"]' => array('checked' => TRUE),
      ),
    ),
    '#ajax' => array(
      'callback' => 'islandora_scholar_xpaths_coins_callback',
      'wrapper' => 'islandora_scholar_xpaths_coins_title',
    ),
   '#field_suffix' => (!empty($form_state['values']['islandora_scholar_xpaths_demo']) ? _mods_id('islandora_scholar_xpaths_start_page', $form_state['values']['islandora_scholar_xpaths_start_page'], check_plain($form_state['values']['islandora_scholar_xpaths_demo'])) : t("Modify to refresh value (PID for demo object is required).")),
  ));
  $form['islandora_scholar_xpaths_coins'] += _islandora_scholar_add_variable('islandora_scholar_xpaths_embargo_date', '//mods:mods[1]//mods:dateOther[@type="embargo"]', array(
    '#type' => 'textfield',
    '#title' => t('Embargo Date'),
    '#description' => t('Default Value: //mods:mods[1]/mods:dateOther[@type="embargo"]'),
    '#states' => array(
      'visible' => array(
        ':input[name="islandora_scholar_xpaths_enabled"]' => array('checked' => TRUE),
      ),
    ),
    '#ajax' => array(
      'callback' => 'islandora_scholar_xpaths_coins_callback',
      'wrapper' => 'islandora_scholar_xpaths_coins_title',
    ),
   '#field_suffix' => (!empty($form_state['values']['islandora_scholar_xpaths_demo']) ? _mods_id('islandora_scholar_xpaths_embargo_date', $form_state['values']['islandora_scholar_xpaths_embargo_date'], check_plain($form_state['values']['islandora_scholar_xpaths_demo'])) : t("Modify to refresh value (PID for demo object is required).")),
  ));
  $form['islandora_scholar_xpaths_coins'] += _islandora_scholar_add_variable('islandora_scholar_xpaths_end_page', '//mods:mods[1]//mods:extent[@unit="page"]/mods:end', array(
    '#type' => 'textfield',
    '#title' => t('End Page'),
    '#description' => t('Default Value: //mods:mods[1]//mods:extent[@unit="page"]/mods:end'),
    '#states' => array(
      'visible' => array(
        ':input[name="islandora_scholar_xpaths_enabled"]' => array('checked' => TRUE),
      ),
    ),
    '#ajax' => array(
      'callback' => 'islandora_scholar_xpaths_coins_callback',
      'wrapper' => 'islandora_scholar_xpaths_coins_title',
    ),
   '#field_suffix' => (!empty($form_state['values']['islandora_scholar_xpaths_demo']) ? _mods_id('islandora_scholar_xpaths_end_page', $form_state['values']['islandora_scholar_xpaths_end_page'], check_plain($form_state['values']['islandora_scholar_xpaths_demo'])) : t("Modify to refresh value (PID for demo object is required).")),
  ));
  $form['islandora_scholar_xpaths_coins'] += _islandora_scholar_add_variable('islandora_scholar_xpaths_origin_date', '//mods:mods[1]/mods:originInfo/mods:dateIssued', array(
    '#type' => 'textfield',
    '#title' => t('Origin Date'),
    '#description' => t('Default Value: //mods:originInfo/mods:dateIssued'),
    '#states' => array(
      'visible' => array(
        ':input[name="islandora_scholar_xpaths_enabled"]' => array('checked' => TRUE),
      ),
    ),
    '#ajax' => array(
      'callback' => 'islandora_scholar_xpaths_coins_callback',
      'wrapper' => 'islandora_scholar_xpaths_coins_title',
    ),
   '#field_suffix' => (!empty($form_state['values']['islandora_scholar_xpaths_demo']) ? _mods_id('islandora_scholar_xpaths_origin_date', $form_state['values']['islandora_scholar_xpaths_origin_date'], check_plain($form_state['values']['islandora_scholar_xpaths_demo'])) : t("Modify to refresh value (PID for demo object is required).")),
  ));
  $form['islandora_scholar_xpaths_coins'] += _islandora_scholar_add_variable('islandora_scholar_xpaths_part_date', '//mods:mods[1]/mods:part/mods:date', array(
    '#type' => 'textfield',
    '#title' => t('Part Date'),
    '#description' => t('Default Value: //mods:part/mods:date'),
    '#states' => array(
      'visible' => array(
        ':input[name="islandora_scholar_xpaths_enabled"]' => array('checked' => TRUE),
      ),
    ),
    '#ajax' => array(
      'callback' => 'islandora_scholar_xpaths_coins_callback',
      'wrapper' => 'islandora_scholar_xpaths_coins_title',
    ),
   '#field_suffix' => (!empty($form_state['values']['islandora_scholar_xpaths_demo']) ? _mods_id('islandora_scholar_xpaths_part_date', $form_state['values']['islandora_scholar_xpaths_part_date'], check_plain($form_state['values']['islandora_scholar_xpaths_demo'])) : t("Modify to refresh value (PID for demo object is required).")),
  ));
  $form['islandora_scholar_xpaths_coins'] += _islandora_scholar_add_variable('islandora_scholar_xpaths_volume', '//mods:mods[1]/mods:part/mods:detail[@type="volume"]/mods:number', array(
    '#type' => 'textfield',
    '#title' => t('Volume'),
    '#description' => t('Default Value: //mods:mods[1]/mods:part/mods:detail[@type="volume"]/mods:number'),
    '#states' => array(
      'visible' => array(
        ':input[name="islandora_scholar_xpaths_enabled"]' => array('checked' => TRUE),
      ),
    ),
    '#ajax' => array(
      'callback' => 'islandora_scholar_xpaths_coins_callback',
      'wrapper' => 'islandora_scholar_xpaths_coins_title',
    ),
   '#field_suffix' => (!empty($form_state['values']['islandora_scholar_xpaths_demo']) ? _mods_id('islandora_scholar_xpaths_volume', $form_state['values']['islandora_scholar_xpaths_volume'], check_plain($form_state['values']['islandora_scholar_xpaths_demo'])) : t("Modify to refresh value (PID for demo object is required).")),
  ));
  $form['islandora_scholar_xpaths_citeproc'] += _islandora_scholar_add_variable('islandora_scholar_xpaths_title_subtitle', '../mods:subTitle', array(
    '#type' => 'textfield',
    '#title' => t('Subtitle used for Citeproc JSON'),
    '#description' => t('Default Value: ../mods:subTitle'),
    '#states' => array(
      'visible' => array(
        ':input[name="islandora_scholar_xpaths_enabled"]' => array('checked' => TRUE),
      ),
    ),
    '#ajax' => array(
      'callback' => 'islandora_scholar_xpaths_citeproc_callback',
      'wrapper' => 'islandora_scholar_xpaths_citeproc_title',
    ),
   '#field_suffix' => (!empty($form_state['values']['islandora_scholar_xpaths_demo']) ? _mods_id('islandora_scholar_xpaths_title_subtitle', $form_state['values']['islandora_scholar_xpaths_title_subtitle'], check_plain($form_state['values']['islandora_scholar_xpaths_demo'])) : t("Modify to refresh value (PID for demo object is required).")),
  ));
  $form['islandora_scholar_xpaths_citeproc'] += _islandora_scholar_add_variable('islandora_scholar_xpaths_title_nonsort', '../mods:nonSort', array(
    '#type' => 'textfield',
    '#title' => t('Nonsort used for Citeproc JSON'),
    '#description' => t("Default Value: ../mods:nonSort"),
    '#states' => array(
      'visible' => array(
        ':input[name="islandora_scholar_xpaths_enabled"]' => array('checked' => TRUE),
      ),
    ),
    '#ajax' => array(
      'callback' => 'islandora_scholar_xpaths_citeproc_callback',
      'wrapper' => 'islandora_scholar_xpaths_citeproc_title',
    ),
   '#field_suffix' => (!empty($form_state['values']['islandora_scholar_xpaths_demo']) ? _mods_id('islandora_scholar_xpaths_title_nonsort', $form_state['values']['islandora_scholar_xpaths_title_nonsort'], check_plain($form_state['values']['islandora_scholar_xpaths_demo'])) : t("Modify to refresh value (PID for demo object is required).")),
  ));
  $form['islandora_scholar_xpaths_citeproc'] += _islandora_scholar_add_variable('islandora_scholar_xpaths_call-number', '//mods:mods[1]/mods:classification', array(
    '#type' => 'textfield',
    '#title' => t('Call number'),
    '#description' => t('Default Value: //mods:mods[1]/mods:classification'),
    '#states' => array(
      'visible' => array(
        ':input[name="islandora_scholar_xpaths_enabled"]' => array('checked' => TRUE),
      ),
    ),
    '#ajax' => array(
      'callback' => 'islandora_scholar_xpaths_citeproc_callback',
      'wrapper' => 'islandora_scholar_xpaths_citeproc_title',
    ),
   '#field_suffix' => (!empty($form_state['values']['islandora_scholar_xpaths_demo']) ? _mods_id('islandora_scholar_xpaths_call-number', $form_state['values']['islandora_scholar_xpaths_call-number'], check_plain($form_state['values']['islandora_scholar_xpaths_demo'])) : t("Modify to refresh value (PID for demo object is required).")),
  ));
  $form['islandora_scholar_xpaths_citeproc'] += _islandora_scholar_add_variable('islandora_scholar_xpaths_collection-title', '//mods:mods[1]/mods:relatedItem[@type="series"]/mods:titleInfo[not(@type)]/mods:title', array(
    '#type' => 'textfield',
    '#title' => t('Series Title'),
    '#description' => t('Default Value: //mods:mods[1]/mods:relatedItem[@type="series"]/mods:titleInfo[not(@type)]/mods:title'),
    '#states' => array(
      'visible' => array(
        ':input[name="islandora_scholar_xpaths_enabled"]' => array('checked' => TRUE),
      ),
    ),
    '#ajax' => array(
      'callback' => 'islandora_scholar_xpaths_citeproc_callback',
      'wrapper' => 'islandora_scholar_xpaths_citeproc_title',
    ),
   '#field_suffix' => (!empty($form_state['values']['islandora_scholar_xpaths_demo']) ? _mods_id('islandora_scholar_xpaths_collection-title', $form_state['values']['islandora_scholar_xpaths_collection-title'], check_plain($form_state['values']['islandora_scholar_xpaths_demo'])) : t("Modify to refresh value (PID for demo object is required).")),
  ));
  $form['islandora_scholar_xpaths_citeproc'] += _islandora_scholar_add_variable('islandora_scholar_xpaths_container-title', '//mods:mods[1]/mods:relatedItem[@type="host"]/mods:titleInfo[not(@type)]/mods:title', array(
    '#type' => 'textfield',
    '#title' => t('Container title'),
    '#description' => t('Default Value: //mods:mods[1]/mods:relatedItem[@type="host"]/mods:titleInfo[not(@type)]/mods:title'),
    '#states' => array(
      'visible' => array(
        ':input[name="islandora_scholar_xpaths_enabled"]' => array('checked' => TRUE),
      ),
    ),
    '#ajax' => array(
      'callback' => 'islandora_scholar_xpaths_citeproc_callback',
      'wrapper' => 'islandora_scholar_xpaths_citeproc_title',
    ),
   '#field_suffix' => (!empty($form_state['values']['islandora_scholar_xpaths_demo']) ? _mods_id('islandora_scholar_xpaths_container-title', $form_state['values']['islandora_scholar_xpaths_container-title'], check_plain($form_state['values']['islandora_scholar_xpaths_demo'])) : t("Modify to refresh value (PID for demo object is required).")),
  ));
  $form['islandora_scholar_xpaths_citeproc'] += _islandora_scholar_add_variable('islandora_scholar_xpaths_DOI', '//mods:mods[1]/mods:identifier[@type="doi"]', array(
    '#type' => 'textfield',
    '#title' => t('DOI'),
    '#description' => t('Default Value: //mods:mods[1]/mods:identifier[@type="doi"]'),
    '#states' => array(
      'visible' => array(
        ':input[name="islandora_scholar_xpaths_enabled"]' => array('checked' => TRUE),
      ),
    ),
    '#ajax' => array(
      'callback' => 'islandora_scholar_xpaths_citeproc_callback',
      'wrapper' => 'islandora_scholar_xpaths_citeproc_title',
    ),
   '#field_suffix' => (!empty($form_state['values']['islandora_scholar_xpaths_demo']) ? _mods_id('islandora_scholar_xpaths_DOI', $form_state['values']['islandora_scholar_xpaths_DOI'], check_plain($form_state['values']['islandora_scholar_xpaths_demo'])) : t("Modify to refresh value (PID for demo object is required).")),
  ));
  $form['islandora_scholar_xpaths_citeproc'] += _islandora_scholar_add_variable('islandora_scholar_xpaths_edition', '//mods:mods[1]/mods:originInfo/mods:edition', array(
    '#type' => 'textfield',
    '#title' => t('Edition'),
    '#description' => t('Default Value: //mods:mods[1]/mods:originInfo/mods:edition'),
    '#states' => array(
      'visible' => array(
        ':input[name="islandora_scholar_xpaths_enabled"]' => array('checked' => TRUE),
      ),
    ),
    '#ajax' => array(
      'callback' => 'islandora_scholar_xpaths_citeproc_callback',
      'wrapper' => 'islandora_scholar_xpaths_citeproc_title',
    ),
   '#field_suffix' => (!empty($form_state['values']['islandora_scholar_xpaths_demo']) ? _mods_id('islandora_scholar_xpaths_edition', $form_state['values']['islandora_scholar_xpaths_edition'], check_plain($form_state['values']['islandora_scholar_xpaths_demo'])) : t("Modify to refresh value (PID for demo object is required).")),
  ));
  $form['islandora_scholar_xpaths_citeproc'] += _islandora_scholar_add_variable('islandora_scholar_xpaths_event', '//mods:mods[1][mods:genre[@authority="local"]="conferencePaper"]/mods:relatedItem/mods:titleInfo/mods:title', array(
    '#type' => 'textfield',
    '#title' => t('Event Title'),
    '#description' => t('Default Value: //mods:mods[1][mods:genre[@authority="local"]="conferencePaper"]/mods:relatedItem/mods:titleInfo/mods:title'),
    '#states' => array(
      'visible' => array(
        ':input[name="islandora_scholar_xpaths_enabled"]' => array('checked' => TRUE),
      ),
    ),
    '#ajax' => array(
      'callback' => 'islandora_scholar_xpaths_citeproc_callback',
      'wrapper' => 'islandora_scholar_xpaths_citeproc_title',
    ),
   '#field_suffix' => (!empty($form_state['values']['islandora_scholar_xpaths_demo']) ? _mods_id('islandora_scholar_xpaths_event', $form_state['values']['islandora_scholar_xpaths_event'], check_plain($form_state['values']['islandora_scholar_xpaths_demo'])) : t("Modify to refresh value (PID for demo object is required).")),
  ));
  $form['islandora_scholar_xpaths_citeproc'] += _islandora_scholar_add_variable('islandora_scholar_xpaths_event_place_property', '//mods:mods[1][mods:genre[@authority="marcgt"][text()="conference publication"]]/mods:originInfo/mods:place/mods:placeTerm', array(
    '#type' => 'textfield',
    '#title' => t('Event Place'),
    '#description' => t('Default Value: //mods:mods[1][mods:genre[@authority="marcgt"][text()="conference publication"]]/mods:originInfo/mods:place/mods:placeTerm'),
    '#states' => array(
      'visible' => array(
        ':input[name="islandora_scholar_xpaths_enabled"]' => array('checked' => TRUE),
      ),
    ),
    '#ajax' => array(
      'callback' => 'islandora_scholar_xpaths_citeproc_callback',
      'wrapper' => 'islandora_scholar_xpaths_citeproc_title',
    ),
   '#field_suffix' => (!empty($form_state['values']['islandora_scholar_xpaths_demo']) ? _mods_id('islandora_scholar_xpaths_event_place_property', $form_state['values']['islandora_scholar_xpaths_event_place_property'], check_plain($form_state['values']['islandora_scholar_xpaths_demo'])) : t("Modify to refresh value (PID for demo object is required).")),
  ));
  $form['islandora_scholar_xpaths_citeproc'] += _islandora_scholar_add_variable('islandora_scholar_xpaths_event_place', '//mods:mods[1][mods:genre[@authority="local"][text()="conferencePaper"]]/mods:originInfo/mods:place/mods:placeTerm', array(
    '#type' => 'textfield',
    '#title' => t('Event'),
    '#description' => t('Default Value: //mods:mods[1][mods:genre[@authority="local"][text()="conferencePaper"]]/mods:originInfo/mods:place/mods:placeTerm'),
    '#states' => array(
      'visible' => array(
        ':input[name="islandora_scholar_xpaths_enabled"]' => array('checked' => TRUE),
      ),
    ),
    '#ajax' => array(
      'callback' => 'islandora_scholar_xpaths_citeproc_callback',
      'wrapper' => 'islandora_scholar_xpaths_citeproc_title',
    ),
   '#field_suffix' => (!empty($form_state['values']['islandora_scholar_xpaths_demo']) ? _mods_id('islandora_scholar_xpaths_event_place', $form_state['values']['islandora_scholar_xpaths_event_place'], check_plain($form_state['values']['islandora_scholar_xpaths_demo'])) : t("Modify to refresh value (PID for demo object is required).")),
  ));
  $form['islandora_scholar_xpaths_citeproc'] += _islandora_scholar_add_variable('islandora_scholar_xpaths_ISBN', '//mods:mods[1]/mods:identifier[@type="isbn"]', array(
    '#type' => 'textfield',
    '#title' => t('ISBN'),
    '#description' => t('Default Value: //mods:mods[1]/mods:identifier[@type="isbn"]'),
    '#states' => array(
      'visible' => array(
        ':input[name="islandora_scholar_xpaths_enabled"]' => array('checked' => TRUE),
      ),
    ),
    '#ajax' => array(
      'callback' => 'islandora_scholar_xpaths_citeproc_callback',
      'wrapper' => 'islandora_scholar_xpaths_citeproc_title',
    ),
   '#field_suffix' => (!empty($form_state['values']['islandora_scholar_xpaths_demo']) ? _mods_id('islandora_scholar_xpaths_ISBN', $form_state['values']['islandora_scholar_xpaths_ISBN'], check_plain($form_state['values']['islandora_scholar_xpaths_demo'])) : t("Modify to refresh value (PID for demo object is required).")),
  ));
  $form['islandora_scholar_xpaths_citeproc'] += _islandora_scholar_add_variable('islandora_scholar_xpaths_note', '//mods:mods[1]/mods:note', array(
    '#type' => 'textfield',
    '#title' => t('Note'),
    '#description' => t('Default Value: //mods:mods[1]/mods:note'),
    '#states' => array(
      'visible' => array(
        ':input[name="islandora_scholar_xpaths_enabled"]' => array('checked' => TRUE),
      ),
    ),
    '#ajax' => array(
      'callback' => 'islandora_scholar_xpaths_citeproc_callback',
      'wrapper' => 'islandora_scholar_xpaths_citeproc_title',
    ),
   '#field_suffix' => (!empty($form_state['values']['islandora_scholar_xpaths_demo']) ? _mods_id('islandora_scholar_xpaths_note', $form_state['values']['islandora_scholar_xpaths_note'], check_plain($form_state['values']['islandora_scholar_xpaths_demo'])) : t("Modify to refresh value (PID for demo object is required).")),
  ));
  $form['islandora_scholar_xpaths_citeproc'] += _islandora_scholar_add_variable('islandora_scholar_xpaths_number', '//mods:mods[1]/mods:relatedItem[@type="series"]/mods:titleInfo/mods:partNumber', array(
    '#type' => 'textfield',
    '#title' => t('Number'),
    '#description' => t('Default Value: //mods:mods[1]/mods:relatedItem[@type="series"]/mods:titleInfo/mods:partNumber'),
    '#states' => array(
      'visible' => array(
        ':input[name="islandora_scholar_xpaths_enabled"]' => array('checked' => TRUE),
      ),
    ),
    '#ajax' => array(
      'callback' => 'islandora_scholar_xpaths_citeproc_callback',
      'wrapper' => 'islandora_scholar_xpaths_citeproc_title',
    ),
   '#field_suffix' => (!empty($form_state['values']['islandora_scholar_xpaths_demo']) ? _mods_id('islandora_scholar_xpaths_number', $form_state['values']['islandora_scholar_xpaths_number'], check_plain($form_state['values']['islandora_scholar_xpaths_demo'])) : t("Modify to refresh value (PID for demo object is required).")),
  ));
  $form['islandora_scholar_xpaths_citeproc'] += _islandora_scholar_add_variable('islandora_scholar_xpaths_publisher', '//mods:mods[1]/mods:originInfo/mods:publisher', array(
    '#type' => 'textfield',
    '#title' => t('Publisher'),
    '#description' => t('Default Value: //mods:mods[1]/mods:originInfo/mods:publisher'),
    '#states' => array(
      'visible' => array(
        ':input[name="islandora_scholar_xpaths_enabled"]' => array('checked' => TRUE),
      ),
    ),
    '#ajax' => array(
      'callback' => 'islandora_scholar_xpaths_citeproc_callback',
      'wrapper' => 'islandora_scholar_xpaths_citeproc_title',
    ),
   '#field_suffix' => (!empty($form_state['values']['islandora_scholar_xpaths_demo']) ? _mods_id('islandora_scholar_xpaths_publisher', $form_state['values']['islandora_scholar_xpaths_publisher'], check_plain($form_state['values']['islandora_scholar_xpaths_demo'])) : t("Modify to refresh value (PID for demo object is required).")),
  ));
  $form['islandora_scholar_xpaths_citeproc'] += _islandora_scholar_add_variable('islandora_scholar_xpaths_publisher-place', '//mods:mods[1]/mods:originInfo/mods:place/mods:placeTerm', array(
    '#type' => 'textfield',
    '#title' => t('Publisher place'),
    '#description' => t('Default Value: //mods:mods[1]/mods:originInfo/mods:place/mods:placeTerm'),
    '#states' => array(
      'visible' => array(
        ':input[name="islandora_scholar_xpaths_enabled"]' => array('checked' => TRUE),
      ),
    ),
    '#ajax' => array(
      'callback' => 'islandora_scholar_xpaths_citeproc_callback',
      'wrapper' => 'islandora_scholar_xpaths_citeproc_title',
    ),
   '#field_suffix' => (!empty($form_state['values']['islandora_scholar_xpaths_demo']) ? _mods_id('islandora_scholar_xpaths_publisher-place', $form_state['values']['islandora_scholar_xpaths_publisher-place'], check_plain($form_state['values']['islandora_scholar_xpaths_demo'])) : t("Modify to refresh value (PID for demo object is required).")),
  ));
  $form['islandora_scholar_xpaths_citeproc'] += _islandora_scholar_add_variable('islandora_scholar_xpaths_URL', '//mods:mods[1]/mods:location/mods:url', array(
    '#type' => 'textfield',
    '#title' => t('URL'),
    '#description' => t('Default Value: //mods:mods[1]/mods:location/mods:url'),
    '#states' => array(
      'visible' => array(
        ':input[name="islandora_scholar_xpaths_enabled"]' => array('checked' => TRUE),
      ),
    ),
    '#ajax' => array(
      'callback' => 'islandora_scholar_xpaths_citeproc_callback',
      'wrapper' => 'islandora_scholar_xpaths_citeproc_title',
    ),
   '#field_suffix' => (!empty($form_state['values']['islandora_scholar_xpaths_demo']) ? _mods_id('islandora_scholar_xpaths_URL', $form_state['values']['islandora_scholar_xpaths_URL'], check_plain($form_state['values']['islandora_scholar_xpaths_demo'])) : t("Modify to refresh value (PID for demo object is required).")),
  ));
  $form['islandora_scholar_xpaths_citeproc'] += _islandora_scholar_add_variable('islandora_scholar_xpaths_number-pmid', '//mods:mods[1]/mods:identifier[@type="accession"]', array(
    '#type' => 'textfield',
    '#title' => t('Accession Number'),
    '#description' => t('Default Value: //mods:mods[1]/mods:identifier[@type="accession"]'),
    '#states' => array(
      'visible' => array(
        ':input[name="islandora_scholar_xpaths_enabled"]' => array('checked' => TRUE),
      ),
    ),
    '#ajax' => array(
      'callback' => 'islandora_scholar_xpaths_citeproc_callback',
      'wrapper' => 'islandora_scholar_xpaths_citeproc_title',
    ),
   '#field_suffix' => (!empty($form_state['values']['islandora_scholar_xpaths_demo']) ? _mods_id('islandora_scholar_xpaths_number-pmid', $form_state['values']['islandora_scholar_xpaths_number-pmid'], check_plain($form_state['values']['islandora_scholar_xpaths_demo'])) : t("Modify to refresh value (PID for demo object is required).")),
  ));
  $form['islandora_scholar_xpaths_citeproc'] += _islandora_scholar_add_variable('islandora_scholar_xpaths_number-pmcid', '//mods:mods[1]/mods:identifier[@type="pmc"]', array(
    '#type' => 'textfield',
    '#title' => t('PMCID'),
    '#description' => t('Default Value: //mods:mods[1]/mods:identifier[@type="pmc"]'),
    '#states' => array(
      'visible' => array(
        ':input[name="islandora_scholar_xpaths_enabled"]' => array('checked' => TRUE),
      ),
    ),
    '#ajax' => array(
      'callback' => 'islandora_scholar_xpaths_citeproc_callback',
      'wrapper' => 'islandora_scholar_xpaths_citeproc_title',
    ),
   '#field_suffix' => (!empty($form_state['values']['islandora_scholar_xpaths_demo']) ? _mods_id('islandora_scholar_xpaths_number-pmcid', $form_state['values']['islandora_scholar_xpaths_number-pmcid'], check_plain($form_state['values']['islandora_scholar_xpaths_demo'])) : t("Modify to refresh value (PID for demo object is required).")),
  ));
  $form['islandora_scholar_xpaths_citeproc'] += _islandora_scholar_add_variable('islandora_scholar_xpaths_number-nihmsid', '//mods:mods[1]/mods:identifier[@type="mid"]', array(
    '#type' => 'textfield',
    '#title' => t('NIH Manuscript Submission Reference Number'),
    '#description' => t('Default Value: //mods:mods[1]/mods:identifier[@type="mid"]'),
    '#states' => array(
      'visible' => array(
        ':input[name="islandora_scholar_xpaths_enabled"]' => array('checked' => TRUE),
      ),
    ),
    '#ajax' => array(
      'callback' => 'islandora_scholar_xpaths_citeproc_callback',
      'wrapper' => 'islandora_scholar_xpaths_citeproc_title',
    ),
   '#field_suffix' => (!empty($form_state['values']['islandora_scholar_xpaths_demo']) ? _mods_id('islandora_scholar_xpaths_number-nihmsid', $form_state['values']['islandora_scholar_xpaths_number-nihmsid'], check_plain($form_state['values']['islandora_scholar_xpaths_demo'])) : t("Modify to refresh value (PID for demo object is required).")),
  ));
  $form['islandora_scholar_xpaths_citeproc'] += _islandora_scholar_add_variable('islandora_scholar_xpaths_event_property', '//mods:mods[1][mods:genre[@authority="marcgt"][text()="conference publication"]]/mods:relatedItem/mods:titleInfo/mods:title', array(
    '#type' => 'textfield',
    '#title' => t('Property'),
    '#description' => t('Default Value: //mods:mods[1][mods:genre[@authority="marcgt"][text()="conference publication"]]/mods:relatedItem/mods:titleInfo/mods:title'),
    '#states' => array(
      'visible' => array(
        ':input[name="islandora_scholar_xpaths_enabled"]' => array('checked' => TRUE),
      ),
    ),
    '#ajax' => array(
      'callback' => 'islandora_scholar_xpaths_citeproc_callback',
      'wrapper' => 'islandora_scholar_xpaths_citeproc_title',
    ),
   '#field_suffix' => (!empty($form_state['values']['islandora_scholar_xpaths_demo']) ? _mods_id('islandora_scholar_xpaths_event_property', $form_state['values']['islandora_scholar_xpaths_event_property'], check_plain($form_state['values']['islandora_scholar_xpaths_demo'])) : t("Modify to refresh value (PID for demo object is required).")),
  ));
  $form['islandora_scholar_xpaths_citeproc'] += _islandora_scholar_add_variable('islandora_scholar_xpaths_pages', '//mods:mods[1]/mods:part/mods:extent[@unit="pages"]', array(
    '#type' => 'textfield',
    '#title' => t('Pages'),
    '#description' => t('Default Value: //mods:mods[1]/mods:part/mods:extent[@unit="pages"]'),
    '#states' => array(
      'visible' => array(
        ':input[name="islandora_scholar_xpaths_enabled"]' => array('checked' => TRUE),
      ),
    ),
    '#ajax' => array(
      'callback' => 'islandora_scholar_xpaths_citeproc_callback',
      'wrapper' => 'islandora_scholar_xpaths_citeproc_title',
    ),
   '#field_suffix' => (!empty($form_state['values']['islandora_scholar_xpaths_demo']) ? _mods_id('islandora_scholar_xpaths_pages', $form_state['values']['islandora_scholar_xpaths_pages'], check_plain($form_state['values']['islandora_scholar_xpaths_demo'])) : t("Modify to refresh value (PID for demo object is required).")),
  ));
  $form['islandora_scholar_xpaths_citeproc'] += _islandora_scholar_add_variable('islandora_scholar_xpaths_types', '//mods:mods[1]/mods:genre[@authority="endnote"]', array(
    '#type' => 'textfield',
    '#title' => t('Endnote authority genre types'),
    '#description' => t('Default Value: //mods:mods[1]/mods:genre[@authority="endnote"]'),
    '#states' => array(
      'visible' => array(
        ':input[name="islandora_scholar_xpaths_enabled"]' => array('checked' => TRUE),
      ),
    ),
    '#ajax' => array(
      'callback' => 'islandora_scholar_xpaths_citeproc_callback',
      'wrapper' => 'islandora_scholar_xpaths_citeproc_title',
    ),
   '#field_suffix' => (!empty($form_state['values']['islandora_scholar_xpaths_demo']) ? _mods_id('islandora_scholar_xpaths_types', $form_state['values']['islandora_scholar_xpaths_types'], check_plain($form_state['values']['islandora_scholar_xpaths_demo'])) : t("Modify to refresh value (PID for demo object is required).")),
  ));
  $form['islandora_scholar_xpaths_citeproc'] += _islandora_scholar_add_variable('islandora_scholar_xpaths_type_marcgt', '//mods:mods[1]/mods:genre[@authority="marcgt"]', array(
    '#type' => 'textfield',
    '#title' => t('MARC authority genre types'),
    '#description' => t('Default Value: //mods:mods[1]/mods:genre[@authority="marcgt"]'),
    '#states' => array(
      'visible' => array(
        ':input[name="islandora_scholar_xpaths_enabled"]' => array('checked' => TRUE),
      ),
    ),
    '#ajax' => array(
      'callback' => 'islandora_scholar_xpaths_citeproc_callback',
      'wrapper' => 'islandora_scholar_xpaths_citeproc_title',
    ),
   '#field_suffix' => (!empty($form_state['values']['islandora_scholar_xpaths_demo']) ? _mods_id('islandora_scholar_xpaths_type_marcgt', $form_state['values']['islandora_scholar_xpaths_type_marcgt'], check_plain($form_state['values']['islandora_scholar_xpaths_demo'])) : t("Modify to refresh value (PID for demo object is required).")),
  ));
  $form['islandora_scholar_xpaths_citeproc'] += _islandora_scholar_add_variable('islandora_scholar_xpaths_type_marcgt_host_titles', '//mods:mods[1]/mods:relatedItem[@type="host"]/mods:titleInfo/mods:title', array(
    '#type' => 'textfield',
    '#title' => t('→ MARC genre type (relative child): chapter titles in a book'),
    '#description' => t("Default Value: //mods:mods[1]/mods:relatedItem[@type='host']/mods:titleInfo/mods:title"),
    '#states' => array(
      'visible' => array(
        ':input[name="islandora_scholar_xpaths_enabled"]' => array('checked' => TRUE),
      ),
    ),
    '#ajax' => array(
      'callback' => 'islandora_scholar_xpaths_citeproc_callback',
      'wrapper' => 'islandora_scholar_xpaths_citeproc_title',
    ),
   '#field_suffix' => (!empty($form_state['values']['islandora_scholar_xpaths_demo']) ? _mods_id('islandora_scholar_xpaths_type_marcgt_host_titles', $form_state['values']['islandora_scholar_xpaths_type_marcgt_host_titles'], check_plain($form_state['values']['islandora_scholar_xpaths_demo'])) : t("Modify to refresh value (PID for demo object is required).")),
  ));
  $form['islandora_scholar_xpaths_citeproc'] += _islandora_scholar_add_variable('islandora_scholar_xpaths_type_marcgt_related', '//mods:mods[1]/mods:relatedItem/mods:genre[@authority="marcgt"]', array(
    '#type' => 'textfield',
    '#title' => t('MARC genre type related'),
    '#description' => t('Default Value: //mods:mods[1]/mods:relatedItem/mods:genre[@authority="marcgt"]'),
    '#states' => array(
      'visible' => array(
        ':input[name="islandora_scholar_xpaths_enabled"]' => array('checked' => TRUE),
      ),
    ),
    '#ajax' => array(
      'callback' => 'islandora_scholar_xpaths_citeproc_callback',
      'wrapper' => 'islandora_scholar_xpaths_citeproc_title',
    ),
   '#field_suffix' => (!empty($form_state['values']['islandora_scholar_xpaths_demo']) ? _mods_id('islandora_scholar_xpaths_type_marcgt_related', $form_state['values']['islandora_scholar_xpaths_type_marcgt_related'], check_plain($form_state['values']['islandora_scholar_xpaths_demo'])) : t("Modify to refresh value (PID for demo object is required).")),
  ));
  $form['islandora_scholar_xpaths_citeproc'] += _islandora_scholar_add_variable('islandora_scholar_xpaths_types_local_auth', '//mods:mods[1]/mods:genre[not(@authority="marcgt" or @authority="endnote")]', array(
    '#type' => 'textfield',
    '#title' => t('types local auth'),
    '#description' => t('Default Value: //mods:mods[1]/mods:genre[not(@authority="marcgt" or @authority="endnote")]'),
    '#states' => array(
      'visible' => array(
        ':input[name="islandora_scholar_xpaths_enabled"]' => array('checked' => TRUE),
      ),
    ),
    '#ajax' => array(
      'callback' => 'islandora_scholar_xpaths_citeproc_callback',
      'wrapper' => 'islandora_scholar_xpaths_citeproc_title',
    ),
   '#field_suffix' => (!empty($form_state['values']['islandora_scholar_xpaths_demo']) ? _mods_id('islandora_scholar_xpaths_types_local_auth', $form_state['values']['islandora_scholar_xpaths_types_local_auth'], check_plain($form_state['values']['islandora_scholar_xpaths_demo'])) : t("Modify to refresh value (PID for demo object is required).")),
  ));
  $form['islandora_scholar_xpaths_citeproc'] += _islandora_scholar_add_variable('islandora_scholar_xpaths_season', '//mods:mods[1]/mods:originInfo/mods:dateOther[@type="season"]', array(
    '#type' => 'textfield',
    '#title' => t('Season'),
    '#description' => t('Default Value: //mods:mods[1]/mods:originInfo/mods:dateOther[@type="season"]'),
    '#states' => array(
      'visible' => array(
        ':input[name="islandora_scholar_xpaths_enabled"]' => array('checked' => TRUE),
      ),
    ),
    '#ajax' => array(
      'callback' => 'islandora_scholar_xpaths_citeproc_callback',
      'wrapper' => 'islandora_scholar_xpaths_citeproc_title',
    ),
   '#field_suffix' => (!empty($form_state['values']['islandora_scholar_xpaths_demo']) ? _mods_id('islandora_scholar_xpaths_season', $form_state['values']['islandora_scholar_xpaths_season'], check_plain($form_state['values']['islandora_scholar_xpaths_demo'])) : t("Modify to refresh value (PID for demo object is required).")),
  ));
  $form['islandora_scholar_xpaths_citeproc'] += _islandora_scholar_add_variable('islandora_scholar_xpaths_date_issued', '//mods:mods[1]/mods:originInfo/mods:dateIssued | //mods:mods[1]/mods:relatedItem[@type="host"]/mods:part/mods:date', array(
    '#type' => 'textfield',
    '#title' => t('Date Issued'),
    '#description' => t('Default Value: //mods:mods[1]/mods:originInfo/mods:dateIssued | //mods:mods[1]/mods:relatedItem[@type="host"]/mods:part/mods:date'),
    '#states' => array(
      'visible' => array(
        ':input[name="islandora_scholar_xpaths_enabled"]' => array('checked' => TRUE),
      ),
    ),
    '#ajax' => array(
      'callback' => 'islandora_scholar_xpaths_citeproc_callback',
      'wrapper' => 'islandora_scholar_xpaths_citeproc_title',
    ),
   '#field_suffix' => (!empty($form_state['values']['islandora_scholar_xpaths_demo']) ? _mods_id('islandora_scholar_xpaths_date_issued', $form_state['values']['islandora_scholar_xpaths_date_issued'], check_plain($form_state['values']['islandora_scholar_xpaths_demo'])) : t("Modify to refresh value (PID for demo object is required).")),
  ));
  $form['islandora_scholar_xpaths_citeproc'] += _islandora_scholar_add_variable('islandora_scholar_xpaths_dateCaptured', '//mods:mods[1]/mods:originInfo/mods:dateCaptured', array(
    '#type' => 'textfield',
    '#title' => t('Date Captured'),
    '#description' => t('Default Value: //mods:mods[1]/mods:originInfo/mods:dateCaptured'),
    '#states' => array(
      'visible' => array(
        ':input[name="islandora_scholar_xpaths_enabled"]' => array('checked' => TRUE),
      ),
    ),
    '#ajax' => array(
      'callback' => 'islandora_scholar_xpaths_citeproc_callback',
      'wrapper' => 'islandora_scholar_xpaths_citeproc_title',
    ),
   '#field_suffix' => (!empty($form_state['values']['islandora_scholar_xpaths_demo']) ? _mods_id('islandora_scholar_xpaths_dateCaptured', $form_state['values']['islandora_scholar_xpaths_dateCaptured'], check_plain($form_state['values']['islandora_scholar_xpaths_demo'])) : t("Modify to refresh value (PID for demo object is required).")),
  ));
  $form['islandora_scholar_xpaths_citeproc'] += _islandora_scholar_add_variable('islandora_scholar_xpaths_local_author', '//mods:mods[1]/mods:name[normalize-space(mods:namePart)]', array(
    '#type' => 'textfield',
    '#title' => t('Names'),
    '#description' => t('Default Value: //mods:mods[1]/mods:name[normalize-space(mods:namePart)]'),
    '#states' => array(
      'visible' => array(
        ':input[name="islandora_scholar_xpaths_enabled"]' => array('checked' => TRUE),
      ),
    ),
    '#ajax' => array(
      'callback' => 'islandora_scholar_xpaths_citeproc_callback',
      'wrapper' => 'islandora_scholar_xpaths_citeproc_title',
    ),
   '#field_suffix' => (!empty($form_state['values']['islandora_scholar_xpaths_demo']) ? _mods_id('islandora_scholar_xpaths_local_author', $form_state['values']['islandora_scholar_xpaths_local_author'], check_plain($form_state['values']['islandora_scholar_xpaths_demo'])) : t("Modify to refresh value (PID for demo object is required).")),
  ));
  $form['islandora_scholar_xpaths_citeproc'] += _islandora_scholar_add_variable('islandora_scholar_xpaths_local_container-author', '//mods:mods[1]/mods:relatedItem[@type="host"]/mods:name', array(
    '#type' => 'textfield',
    '#title' => t('Container Author'),
    '#description' => t('Default Value: //mods:mods[1]/mods:relatedItem[@type="host"]/mods:name'),
    '#states' => array(
      'visible' => array(
        ':input[name="islandora_scholar_xpaths_enabled"]' => array('checked' => TRUE),
      ),
    ),
    '#ajax' => array(
      'callback' => 'islandora_scholar_xpaths_citeproc_callback',
      'wrapper' => 'islandora_scholar_xpaths_citeproc_title',
    ),
   '#field_suffix' => (!empty($form_state['values']['islandora_scholar_xpaths_demo']) ? _mods_id('islandora_scholar_xpaths_local_container-author', $form_state['values']['islandora_scholar_xpaths_local_container-author'], check_plain($form_state['values']['islandora_scholar_xpaths_demo'])) : t("Modify to refresh value (PID for demo object is required).")),
  ));
  $form['islandora_scholar_xpaths_citeproc'] += _islandora_scholar_add_variable('islandora_scholar_xpaths_local_collection-editor', '//mods:mods[1]/mods:relatedItem[@type="series"]/mods:name', array(
    '#type' => 'textfield',
    '#title' => t('Series Editor'),
    '#description' => t('Default Value: //mods:mods[1]/mods:relatedItem[@type="series"]/mods:name'),
    '#states' => array(
      'visible' => array(
        ':input[name="islandora_scholar_xpaths_enabled"]' => array('checked' => TRUE),
      ),
    ),
    '#ajax' => array(
      'callback' => 'islandora_scholar_xpaths_citeproc_callback',
      'wrapper' => 'islandora_scholar_xpaths_citeproc_title',
    ),
   '#field_suffix' => (!empty($form_state['values']['islandora_scholar_xpaths_demo']) ? _mods_id('islandora_scholar_xpaths_local_collection-editor', $form_state['values']['islandora_scholar_xpaths_local_collection-editor'], check_plain($form_state['values']['islandora_scholar_xpaths_demo'])) : t("Modify to refresh value (PID for demo object is required).")),
  ));
  $form['islandora_scholar_xpaths_citeproc'] += _islandora_scholar_add_variable('islandora_scholar_xpaths_role_term', '//mods:mods[1]//mods:role/mods:roleTerm', array(
    '#type' => 'textfield',
    '#title' => t('Role Term'),
    '#description' => t('Default Value: //mods:mods[1]//mods:role/mods:roleTerm'),
    '#states' => array(
      'visible' => array(
        ':input[name="islandora_scholar_xpaths_enabled"]' => array('checked' => TRUE),
      ),
    ),
    '#ajax' => array(
      'callback' => 'islandora_scholar_xpaths_citeproc_callback',
      'wrapper' => 'islandora_scholar_xpaths_citeproc_title',
    ),
   '#field_suffix' => (!empty($form_state['values']['islandora_scholar_xpaths_demo']) ? _mods_id('islandora_scholar_xpaths_role_term', $form_state['values']['islandora_scholar_xpaths_role_term'], check_plain($form_state['values']['islandora_scholar_xpaths_demo'])) : t("Modify to refresh value (PID for demo object is required).")),
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
    if (_islandora_scholar_check_path($islandora_scholar_path_to_pdftotext) != 99) {
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
    'islandora_scholar_xpaths_local_container-author',
    'islandora_scholar_xpaths_note',
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
 * Function concatenates strings and a veriable to return a preview image.
 */
function islandora_scholar_xpaths_demo_prefix_callback($image) {
  if (isset($image)) {
    $image_tag = "<span style='color:green'>Successfully loaded PID!</span><br/><br/><img src='/islandora/object/" . $image . "/datastream/TN/view'>";
    return $image_tag;
  }
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
 *   A string specifying the object's pid.
 *
 * @return array
 *   An array of AJAX commands.
 */
function _mods_id($field = NULL, $mods_value = NULL, $pid = NULL) {
  // The subtitle and nonsort are relative to a parent but the preview needs to
  // prepend the parent manually.
  if ($field == 'islandora_scholar_xpaths_title_subtitle' || $field == 'islandora_scholar_xpaths_title_nonsort') {
    $mods_value = '//mods:mods[1]/mods:titleInfo/mods:title/' . $mods_value;
  }

  if (islandora_object_access(ISLANDORA_VIEW_OBJECTS, islandora_object_load($pid)) == 1) {
    try{
      $object = islandora_object_load($pid);
      $xmlstr = $object['MODS']->content;
      $mods_doc = new DOMDocument();
      $mods_doc->loadXML($object['MODS']->content);
      $mods_xpath = new DOMXPath($mods_doc);
      $mods_xpath->registerNamespace('m', 'http://www.loc.gov/mods/v3');
      $lookup_results = $mods_xpath->query($mods_value);
      $build_string = '';

      // Test inputs for validity and to avoid a 500 error.
      if (isset($lookup_results->item(0)->textContent)) {
        $mods_value_string = $lookup_results->item(0)->textContent;
      }
      else {
        $mods_value_string = '';
      }
      // Build the field suffix.
      if (strlen($mods_value) == 0) {
        $build_string = t('For pid !pid: <span style="color:red">Path cannot be blank</span>', array(
          '!pid' => $pid,
        ));
      }
      elseif ($lookup_results == NULL) {
        $build_string = t('For pid !pid: <span style="color:red">ERROR</span>', array(
          '!pid' => $pid,
        ));
      }
      elseif (strlen($mods_value_string) == 0) {
        $build_string = t('For pid !pid: <span style="color:orange">Nothing Found</span>', array(
          '!pid' => $pid,
        ));
      }
      else {
        $build_string = t('For pid !pid: <span style="color:blue">!mods_value_string</span>', array(
          '!pid' => $pid,
          '!mods_value_string' => $mods_value_string,
        ));
      }
      return $build_string;
    }
    catch (Exception $e) {
      watchdog('islandora_scholar', 'Got exception: @message', array(
        '@message' => $e->getMessage(),
      ));
    }
  }
}

/**
 * Verifies file path of a binary by executing it.
 *
 * @param string $check_path
 *   The user-submitted path to an executable binary.
 *
 * @return int
 *   Returns a integer to determine if the directory is executable.
 */
function _islandora_scholar_check_path($check_path) {
  try{
    $status = array(
      'output' => '',
      'errors' => array(),
    );
    if (!is_file($check_path)) {
      $status['errors'][] = t('The specified application file path %file does not exist.', array('%file' => $check_path));
    }
    elseif (!is_executable($check_path)) {
      $status['errors'][] = t('The specified application file path %file is not executable.', array('%file' => $check_path));
    }
    if ($status['errors'] && ($open_basedir = ini_get('open_basedir'))) {
      form_set_error('check_path', t('The PHP <a href="@php-url">open_basedir</a> security restriction is set to %open-basedir, which may prevent to locate the application.', array(
        '%open-basedir' => $open_basedir,
        '@php-url' => 'http://php.net/manual/en/ini.core.php#ini.open-basedir',
      )));
    }
    if (!$status['errors']) {
      exec(escapeshellcmd($check_path), $error, $output);
      if ($output !== '') {
        $status['output'] = $output;
      }
    }
    return $status['output'];
  }
  catch (Exception $e) {
    watchdog('islandora_scholar', 'Got exception: @message', array(
      '@message' => $e->getMessage(),
    ));
  }
}
