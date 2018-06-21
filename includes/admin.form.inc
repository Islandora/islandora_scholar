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
    'islandora_scholar_xpaths' => array(
      '#type' => 'fieldset',
      '#title' => t('Islandora Scholar Xpath configuration'),
      '#description' => t('Various xpaths. <a href="https://github.com/Islandora/islandora_scholar/">Additional information</a><br/><br/>'),
      '#collapsible' => TRUE,
    ),
  );
  $form['islandora_scholar_xpaths']['reset'] = array(
    '#type' => 'submit',
    '#value' => t('Reset all Xpaths to defaults'),
    '#submit' => array('_islandora_scholar_xpaths_reset'),
  );

  $form['islandora_scholar_xpaths'] += _islandora_scholar_add_variable('islandora_scholar_default_search_xpath_title', '//mods:mods[1]/mods:titleInfo/mods:title', array(
    '#type' => 'textfield',
    '#title' => t('Title'),
    '#description' => t('Default Value: //mods:mods[1]/mods:titleInfo/mods:title <br/>The XPath to use to use for title.'),
    '#states' => array(
      'visible' => array(
        ':input[name="islandora_scholar_islandora_scholar_xpaths_enabled"]' => array('checked' => TRUE),
      ),
    ),
  ));
  $form['islandora_scholar_xpaths'] += _islandora_scholar_add_variable('islandora_scholar_default_search_xpath_abstract', '//mods:mods[1]/mods:abstract', array(
    '#type' => 'textfield',
    '#title' => t('Abstract'),
    '#description' => t('Default Value: //mods:mods[1]/mods:abstract <br/>The XPath to use for the abstract.'),
    '#states' => array(
      'visible' => array(
        ':input[name="islandora_scholar_islandora_scholar_xpaths_enabled"]' => array('checked' => TRUE),
      ),
    ),
  ));
  $form['islandora_scholar_xpaths'] += _islandora_scholar_add_variable('islandora_scholar_default_search_xpath_call-number', '//mods:mods[1]/mods:classification', array(
    '#type' => 'textfield',
    '#title' => t('Call number'),
    '#description' => t('Default Value: //mods:mods[1]/mods:classification <br/>The XPath to use for the call number. '),
    '#states' => array(
      'visible' => array(
        ':input[name="islandora_scholar_islandora_scholar_xpaths_enabled"]' => array('checked' => TRUE),
      ),
    ),
  ));
  $form['islandora_scholar_xpaths'] += _islandora_scholar_add_variable('islandora_scholar_default_search_xpath_collection-title', '//mods:mods[1]/mods:relatedItem[@type="series"]/mods:titleInfo[not(@type)]/mods:title', array(
    '#type' => 'textfield',
    '#title' => t('Collection title'),
    '#description' => t('Default Value: //mods:mods[1]/mods:relatedItem[@type="series"]/mods:titleInfo[not(@type)]/mods:title <br/>The XPath to use for collection title. '),
    '#states' => array(
      'visible' => array(
        ':input[name="islandora_scholar_islandora_scholar_xpaths_enabled"]' => array('checked' => TRUE),
      ),
    ),
  ));
  $form['islandora_scholar_xpaths'] += _islandora_scholar_add_variable('islandora_scholar_default_search_xpath_container-title', '//mods:mods[1]/mods:relatedItem[@type="host"]/mods:titleInfo[not(@type)]/mods:title', array(
    '#type' => 'textfield',
    '#title' => t('Container title'),
    '#description' => t('Default Value: //mods:mods[1]/mods:relatedItem[@type="host"]/mods:titleInfo[not(@type)]/mods:title <br/>The XPath to use for the container search. '),
    '#states' => array(
      'visible' => array(
        ':input[name="islandora_scholar_islandora_scholar_xpaths_enabled"]' => array('checked' => TRUE),
      ),
    ),
  ));
  $form['islandora_scholar_xpaths'] += _islandora_scholar_add_variable('islandora_scholar_default_search_xpath_DOI', '//mods:mods[1]/mods:identifier[@type="doi"]', array(
    '#type' => 'textfield',
    '#title' => t('DOI'),
    '#description' => t('Default Value: //mods:mods[1]/mods:identifier[@type="doi"] <br/>The XPath to use for DOI. '),
    '#states' => array(
      'visible' => array(
        ':input[name="islandora_scholar_islandora_scholar_xpaths_enabled"]' => array('checked' => TRUE),
      ),
    ),
  ));
  $form['islandora_scholar_xpaths'] += _islandora_scholar_add_variable('islandora_scholar_default_search_xpath_edition', '//mods:mods[1]/mods:originInfo/mods:edition', array(
    '#type' => 'textfield',
    '#title' => t('Edition'),
    '#description' => t('Default Value: //mods:mods[1]/mods:originInfo/mods:edition <br/>The XPath to use edition. '),
    '#states' => array(
      'visible' => array(
        ':input[name="islandora_scholar_islandora_scholar_xpaths_enabled"]' => array('checked' => TRUE),
      ),
    ),
  ));
  $form['islandora_scholar_xpaths'] += _islandora_scholar_add_variable('islandora_scholar_default_search_xpath_event', '//mods:mods[1][mods:genre[@authority="local"]="conferencePaper"]/mods:relatedItem/mods:titleInfo/mods:title', array(
    '#type' => 'textfield',
    '#title' => t('Event'),
    '#description' => t('Default Value: //mods:mods[1][mods:genre[@authority="local"]="conferencePaper"]/mods:relatedItem/mods:titleInfo/mods:title <br/>The XPath to use for event type. '),
    '#states' => array(
      'visible' => array(
        ':input[name="islandora_scholar_islandora_scholar_xpaths_enabled"]' => array('checked' => TRUE),
      ),
    ),
  ));
  $form['islandora_scholar_xpaths'] += _islandora_scholar_add_variable('islandora_scholar_default_search_xpath_genre', '//mods:mods[1]/mods:genre', array(
    '#type' => 'textfield',
    '#title' => t('Genre'),
    '#description' => t('Default Value: //mods:mods[1]/mods:genre <br/>The XPath to use for genre. '),
    '#states' => array(
      'visible' => array(
        ':input[name="islandora_scholar_islandora_scholar_xpaths_enabled"]' => array('checked' => TRUE),
      ),
    ),
  ));
  $form['islandora_scholar_xpaths'] += _islandora_scholar_add_variable('islandora_scholar_default_search_xpath_ISBN', '//mods:mods[1]/mods:identifier[@type="isbn"]', array(
    '#type' => 'textfield',
    '#title' => t('ISBN'),
    '#description' => t('Default Value: //mods:mods[1]/mods:identifier[@type="isbn"] <br/>The XPath to use for ISBN number. '),
    '#states' => array(
      'visible' => array(
        ':input[name="islandora_scholar_islandora_scholar_xpaths_enabled"]' => array('checked' => TRUE),
      ),
    ),
  ));
  $form['islandora_scholar_xpaths'] += _islandora_scholar_add_variable('islandora_scholar_default_search_xpath_volume', '//mods:mods[1]/mods:part/mods:detail[@type="volume"]/mods:number', array(
    '#type' => 'textfield',
    '#title' => t('Volume'),
    '#description' => t('Default Value: //mods:mods[1]/mods:part/mods:detail[@type="volume"]/mods:number <br/>The XPath to use for volume number / title. '),
    '#states' => array(
      'visible' => array(
        ':input[name="islandora_scholar_islandora_scholar_xpaths_enabled"]' => array('checked' => TRUE),
      ),
    ),
  ));
  $form['islandora_scholar_xpaths'] += _islandora_scholar_add_variable('islandora_scholar_default_search_xpath_issue', '//mods:mods[1]/mods:part/mods:detail[@type="issue"]/mods:number', array(
    '#type' => 'textfield',
    '#title' => t('Issue'),
    '#description' => t('Default Value: //mods:mods[1]/mods:part/mods:detail[@type="issue"]/mods:number <br/>The XPath to use for the issue title, name or number. '),
    '#states' => array(
      'visible' => array(
        ':input[name="islandora_scholar_islandora_scholar_xpaths_enabled"]' => array('checked' => TRUE),
      ),
    ),
  ));
  $form['islandora_scholar_xpaths'] += _islandora_scholar_add_variable('islandora_scholar_default_search_xpath_note', '//mods:mods[1]/mods:note', array(
    '#type' => 'textfield',
    '#title' => t('Note'),
    '#description' => t('Default Value: //mods:mods[1]/mods:note <br/>The XPath to use for notes. '),
    '#states' => array(
      'visible' => array(
        ':input[name="islandora_scholar_islandora_scholar_xpaths_enabled"]' => array('checked' => TRUE),
      ),
    ),
  ));
  $form['islandora_scholar_xpaths'] += _islandora_scholar_add_variable('islandora_scholar_default_search_xpath_number', '//mods:mods[1]/mods:relatedItem[@type="series"]/mods:titleInfo/mods:partNumber', array(
    '#type' => 'textfield',
    '#title' => t('Number'),
    '#description' => t('Default Value: //mods:mods[1]/mods:relatedItem[@type="series"]/mods:titleInfo/mods:partNumber <br/>The XPath to use for series part number. '),
    '#states' => array(
      'visible' => array(
        ':input[name="islandora_scholar_islandora_scholar_xpaths_enabled"]' => array('checked' => TRUE),
      ),
    ),
  ));
  $form['islandora_scholar_xpaths'] += _islandora_scholar_add_variable('islandora_scholar_default_search_xpath_publisher', '//mods:mods[1]/mods:originInfo/mods:publisher', array(
    '#type' => 'textfield',
    '#title' => t('Publisher'),
    '#description' => t('Default Value: //mods:mods[1]/mods:originInfo/mods:publisher <br/>The XPath to use for Publisher. '),
    '#states' => array(
      'visible' => array(
        ':input[name="islandora_scholar_islandora_scholar_xpaths_enabled"]' => array('checked' => TRUE),
      ),
    ),
  ));
  $form['islandora_scholar_xpaths'] += _islandora_scholar_add_variable('islandora_scholar_default_search_xpath_publisher-place', '//mods:mods[1]/mods:originInfo/mods:place/mods:placeTerm', array(
    '#type' => 'textfield',
    '#title' => t('Publisher place'),
    '#description' => t('Default Value: //mods:mods[1]/mods:originInfo/mods:place/mods:placeTerm <br/>The XPath to use for publisher-place. '),
    '#states' => array(
      'visible' => array(
        ':input[name="islandora_scholar_islandora_scholar_xpaths_enabled"]' => array('checked' => TRUE),
      ),
    ),
  ));
  $form['islandora_scholar_xpaths'] += _islandora_scholar_add_variable('islandora_scholar_default_search_xpath_URL', '//mods:mods[1]/mods:location/mods:url', array(
    '#type' => 'textfield',
    '#title' => t('URL'),
    '#description' => t('Default Value: //mods:mods[1]/mods:location/mods:url <br/>The XPath to use for URL. '),
    '#states' => array(
      'visible' => array(
        ':input[name="islandora_scholar_islandora_scholar_xpaths_enabled"]' => array('checked' => TRUE),
      ),
    ),
  ));
  $form['islandora_scholar_xpaths'] += _islandora_scholar_add_variable('islandora_scholar_default_search_xpath_number-pmid', '//mods:mods[1]/mods:identifier[@type="accession"]', array(
    '#type' => 'textfield',
    '#title' => t('PubMed ID'),
    '#description' => t('Default Value: //mods:mods[1]/mods:identifier[@type="accession"] <br/>The XPath to use for the PubMed ID. '),
    '#states' => array(
      'visible' => array(
        ':input[name="islandora_scholar_islandora_scholar_xpaths_enabled"]' => array('checked' => TRUE),
      ),
    ),
  ));
  $form['islandora_scholar_xpaths'] += _islandora_scholar_add_variable('islandora_scholar_default_search_xpath_number-pmcid', '//mods:mods[1]/mods:identifier[@type="pmc"]', array(
    '#type' => 'textfield',
    '#title' => t('PMCID'),
    '#description' => t('Default Value: //mods:mods[1]/mods:identifier[@type="pmc"] <br/>The XPath to use for the PubMed Central reference number (PMCID). '),
    '#states' => array(
      'visible' => array(
        ':input[name="islandora_scholar_islandora_scholar_xpaths_enabled"]' => array('checked' => TRUE),
      ),
    ),
  ));
  $form['islandora_scholar_xpaths'] += _islandora_scholar_add_variable('islandora_scholar_default_search_xpath_number-nihmsid', '//mods:mods[1]/mods:identifier[@type="mid"]', array(
    '#type' => 'textfield',
    '#title' => t('NIH Manuscript Submission Reference Number'),
    '#description' => t('Default Value: //mods:mods[1]/mods:identifier[@type="mid"] <br/>The XPath to use for NIH Manuscript Submission Reference Number (NIHMSID). '),
    '#states' => array(
      'visible' => array(
        ':input[name="islandora_scholar_islandora_scholar_xpaths_enabled"]' => array('checked' => TRUE),
      ),
    ),
  ));
  $form['islandora_scholar_xpaths'] += _islandora_scholar_add_variable('islandora_scholar_default_search_xpath_authors_xpath', '//mods:mods[1]/mods:name/mods:role[mods:roleTerm = "author"]/../mods:namePart[@type="family"]', array(
    '#type' => 'textfield',
    '#title' => t('Authors'),
    '#description' => t('Default Value: //mods:mods[1]/mods:name/mods:role[mods:roleTerm = "author"]/../mods:namePart[@type="family"] <br/>The XPath to use for authors. '),
    '#states' => array(
      'visible' => array(
        ':input[name="islandora_scholar_islandora_scholar_xpaths_enabled"]' => array('checked' => TRUE),
      ),
    ),
  ));
  $form['islandora_scholar_xpaths'] += _islandora_scholar_add_variable('islandora_scholar_default_search_xpath_property', '//mods:mods[1][mods:genre[@authority="marcgt"][text()="conference publication"]]/mods:relatedItem/mods:titleInfo/mods:title', array(
    '#type' => 'textfield',
    '#title' => t('property'),
    '#description' => t('Default Value: //mods:mods[1][mods:genre[@authority="marcgt"][text()="conference publication"]]/mods:relatedItem/mods:titleInfo/mods:title <br/>The XPath to use for property. '),
    '#states' => array(
      'visible' => array(
        ':input[name="islandora_scholar_islandora_scholar_xpaths_enabled"]' => array('checked' => TRUE),
      ),
    ),
  ));
  $form['islandora_scholar_xpaths'] += _islandora_scholar_add_variable('islandora_scholar_default_search_xpath_pages', '//mods:mods[1]/mods:part/mods:extent[@unit="pages"]', array(
    '#type' => 'textfield',
    '#title' => t('Pages'),
    '#description' => t('Default Value: //mods:mods[1]/mods:part/mods:extent[@unit="pages"] <br/>The XPath to use for pages. '),
    '#states' => array(
      'visible' => array(
        ':input[name="islandora_scholar_islandora_scholar_xpaths_enabled"]' => array('checked' => TRUE),
      ),
    ),
  ));
  $form['islandora_scholar_xpaths'] += _islandora_scholar_add_variable('islandora_scholar_default_search_xpath_types', '//mods:mods[1]/mods:genre[@authority="endnote"]', array(
    '#type' => 'textfield',
    '#title' => t('Types'),
    '#description' => t('Default Value: //mods:mods[1]/mods:genre[@authority="endnote"] <br/>The XPath to use for types. '),
    '#states' => array(
      'visible' => array(
        ':input[name="islandora_scholar_islandora_scholar_xpaths_enabled"]' => array('checked' => TRUE),
      ),
    ),
  ));
  $form['islandora_scholar_xpaths'] += _islandora_scholar_add_variable('islandora_scholar_default_search_xpath_type_marcgt', '//mods:mods[1]/mods:genre[@authority="marcgt"]', array(
    '#type' => 'textfield',
    '#title' => t('type_marcgt'),
    '#description' => t('Default Value: //mods:mods[1]/mods:genre[@authority="marcgt"] <br/>The XPath to use for type_marcgt. '),
    '#states' => array(
      'visible' => array(
        ':input[name="islandora_scholar_islandora_scholar_xpaths_enabled"]' => array('checked' => TRUE),
      ),
    ),
  ));
  $form['islandora_scholar_xpaths'] += _islandora_scholar_add_variable('islandora_scholar_default_search_xpath_type_marcgt_related', '//mods:mods[1]/mods:relatedItem/mods:genre[@authority="marcgt"]', array(
    '#type' => 'textfield',
    '#title' => t('type_marcgt_related'),
    '#description' => t('Default Value: //mods:mods[1]/mods:relatedItem/mods:genre[@authority="marcgt"] <br/>The XPath to use for type_marcgt_related. '),
    '#states' => array(
      'visible' => array(
        ':input[name="islandora_scholar_islandora_scholar_xpaths_enabled"]' => array('checked' => TRUE),
      ),
    ),
  ));
  $form['islandora_scholar_xpaths'] += _islandora_scholar_add_variable('islandora_scholar_default_search_xpath_types_local_auth', '//mods:mods[1]/mods:genre[not(@authority="marcgt" or @authority="endnote")]', array(
    '#type' => 'textfield',
    '#title' => t('types_local_auth'),
    '#description' => t('Default Value: //mods:mods[1]/mods:genre[not(@authority="marcgt" or @authority="endnote")] <br/>The XPath to use for types_local_auth. '),
    '#states' => array(
      'visible' => array(
        ':input[name="islandora_scholar_islandora_scholar_xpaths_enabled"]' => array('checked' => TRUE),
      ),
    ),
  ));
  $form['islandora_scholar_xpaths'] += _islandora_scholar_add_variable('islandora_scholar_default_search_xpath_season', '//mods:originInfo/mods:dateOther[@type="season"]', array(
    '#type' => 'textfield',
    '#title' => t('Season'),
    '#description' => t('Default Value: //mods:originInfo/mods:dateOther[@type="season"] <br/>The XPath to use for season. '),
    '#states' => array(
      'visible' => array(
        ':input[name="islandora_scholar_islandora_scholar_xpaths_enabled"]' => array('checked' => TRUE),
      ),
    ),
  ));
  $form['islandora_scholar_xpaths'] += _islandora_scholar_add_variable('islandora_scholar_default_search_xpath_date_captured', '//mods:originInfo/mods:dateCaptured', array(
    '#type' => 'textfield',
    '#title' => t('date_captured'),
    '#description' => t('Default Value: //mods:originInfo/mods:dateCaptured <br/>The XPath to use date_captured. '),
    '#states' => array(
      'visible' => array(
        ':input[name="islandora_scholar_islandora_scholar_xpaths_enabled"]' => array('checked' => TRUE),
      ),
    ),
  ));
  $form['islandora_scholar_xpaths'] += _islandora_scholar_add_variable('islandora_scholar_default_search_xpath_date_issued', '//mods:originInfo/mods:dateIssued | //mods:mods[1]/mods:relatedItem[@type="host"]/mods:part/mods:date', array(
    '#type' => 'textfield',
    '#title' => t('date_issued'),
    '#description' => t('Default Value: //mods:originInfo/mods:dateIssued | //mods:mods[1]/mods:relatedItem[@type="host"]/mods:part/mods:date <br/>The XPath to use for date_issued. '),
    '#states' => array(
      'visible' => array(
        ':input[name="islandora_scholar_islandora_scholar_xpaths_enabled"]' => array('checked' => TRUE),
      ),
    ),
  ));
  $form['islandora_scholar_xpaths'] += _islandora_scholar_add_variable('islandora_scholar_default_search_xpath_title_results', '//mods:mods[1]/mods:titleInfo/mods:title', array(
    '#type' => 'textfield',
    '#title' => t('title_results'),
    '#description' => t('Default Value: //mods:mods[1]/mods:titleInfo/mods:title <br/>The XPath to use for title_results. '),
    '#states' => array(
      'visible' => array(
        ':input[name="islandora_scholar_islandora_scholar_xpaths_enabled"]' => array('checked' => TRUE),
      ),
    ),
  ));
  $form['islandora_scholar_xpaths'] += _islandora_scholar_add_variable('islandora_scholar_default_search_xpath_degree_grantor', '//mods:mods/mods:name[@type="corporate"][mods:role/mods:roleTerm = "Degree grantor"]/mods:namePart', array(
    '#type' => 'textfield',
    '#title' => t('degree_grantor'),
    '#description' => t('Default Value: //mods:mods/mods:name[@type="corporate"][mods:role/mods:roleTerm = "Degree grantor"]/mods:namePart <br/>The XPath to use for degree grantor. '),
    '#states' => array(
      'visible' => array(
        ':input[name="islandora_scholar_islandora_scholar_xpaths_enabled"]' => array('checked' => TRUE),
      ),
    ),
  ));
  $form['google_scholar_search'] += _islandora_scholar_add_variable('islandora_scholar_google_scholar_search_enabled', FALSE, array(
    '#type' => 'checkbox',
    '#title' => t('Render Google Scholar Search link'),
    '#description' => t('Enable Searching in Google Scholar.'),
  ));
  $form['google_scholar_search'] += _islandora_scholar_add_variable('islandora_scholar_google_scholar_primary_search_xpath', '//mods:identifier[@type=\"doi\"]', array(
    '#type' => 'textfield',
    '#title' => t('Google Scholar Primary Search XPath'),
    '#description' => t('The primary xpath you want to use to search.'),
    '#states' => array(
      'visible' => array(
        ':input[name="islandora_scholar_google_scholar_search_enabled"]' => array('checked' => TRUE),
      ),
    ),
  ));
  $form['google_scholar_search'] += _islandora_scholar_add_variable('islandora_scholar_google_scholar_default_search_xpath', '//mods:mods[1]/mods:titleInfo/mods:title', array(
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
  $form['library_catalog_search'] += _islandora_scholar_add_variable('islandora_scholar_library_catalog_primary_search_xpath', '//mods:identifier[@type=\"doi\"]', array(
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
  $variables = array(
    'islandora_scholar_default_search_xpath_title',
    'islandora_scholar_default_search_xpath_abstract',
    'islandora_scholar_default_search_xpath_call-number',
    'islandora_scholar_default_search_xpath_collection-title',
    'islandora_scholar_default_search_xpath_container-title',
    'islandora_scholar_default_search_xpath_DOI',
    'islandora_scholar_default_search_xpath_edition',
    'islandora_scholar_default_search_xpath_event',
    'islandora_scholar_default_search_xpath_genre',
    'islandora_scholar_default_search_xpath_ISBN',
    'islandora_scholar_default_search_xpath_volume',
    'islandora_scholar_default_search_xpath_issue',
    'islandora_scholar_default_search_xpath_note',
    'islandora_scholar_default_search_xpath_number',
    'islandora_scholar_default_search_xpath_publisher',
    'islandora_scholar_default_search_xpath_publisher-place',
    'islandora_scholar_default_search_xpath_URL',
    'islandora_scholar_default_search_xpath_number-pmid',
    'islandora_scholar_default_search_xpath_number-pmcid',
    'islandora_scholar_default_search_xpath_number-nihmsid',
    'islandora_scholar_default_search_xpath_authors_xpath',
    'islandora_scholar_default_search_xpath_property',
    'islandora_scholar_default_search_xpath_pages',
    'islandora_scholar_default_search_xpath_types',
    'islandora_scholar_default_search_xpath_type_marcgt',
    'islandora_scholar_default_search_xpath_type_marcgt_related',
    'islandora_scholar_default_search_xpath_types_local_auth',
    'islandora_scholar_default_search_xpath_season',
    'islandora_scholar_default_search_xpath_date_captured',
    'islandora_scholar_default_search_xpath_date_issued',
    'islandora_scholar_default_search_xpath_title_results',
    'islandora_scholar_default_search_xpath_degree_grantor',
  );
  $message = t('xpaths reset');
  drupal_set_message(check_plain(t($message)));
  array_walk($variables, 'variable_del');
}
