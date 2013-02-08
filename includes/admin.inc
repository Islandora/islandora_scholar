<?php

/**
 * @file
 * Admin form for the islandora_scholar.module
 */

/**
 *
 * @param type $name
 * @param type $default
 * @param type $other
 * @return type
 */
function _islandora_scholar_add_variable($name, $default = '', $other = array()) {
  $other += array( //Some defaults...
    '#type' => 'textfield',
    '#title' => $name
  );
  $other['#default_value'] = variable_get($name, $default);
  $to_return = array(
    $name => $other,
  );
  return $to_return;
}

/**
 * Admin form islandora_scholar.
 *
 * @return type
 */
function islandora_scholar_admin_form() {
  $form = array(
    'romeo' => array(
      '#type' => 'fieldset',
      '#title' => t('RoMEO configuration'),
      '#description' => t('Various parameters used by RoMEO'),
      '#collapsible' => TRUE,
    ),
  );

  $form += _islandora_scholar_add_variable('islandora_scholar_google_direct_to_fedora', FALSE, array(
    '#type' => 'checkbox',
    '#title' => t('Provide Google PDF viewer with direct URL.'),
    '#description' => t('Normally, we would have all calls go through Drupal ' .
      'get the relevant PDF...  This option tries to make the call go directly' .
      ' to your associated Fedora instance.  (Note:  This will not work if you ' .
      'use "localhost" or a non-public IP address in the Islandora\'s base Fedora URL.)'),
  ));

  $form['romeo'] += _islandora_scholar_add_variable('islandora_scholar_romeo_enable', FALSE, array(
    '#type' => 'checkbox',
    '#title' => t('Enable RoMEO attempts.'),
    '#description' => t('Whether or not we should even try getting information from RoMEO.'),
  ));
  $form['romeo'] += _islandora_scholar_add_variable('islandora_scholar_romeo_url', 'http://www.sherpa.ac.uk/romeo/api29.php', array(
    '#title' => t('Sherpa/RoMEO URL'),
    '#description' => t('The URL to which to make requests.'),
  ));
  $form['romeo'] += _islandora_scholar_add_variable('islandora_scholar_romeo_key', '', array(
    '#title' => t('Sherpa/RoMEO API Key'),
    '#description' => t('Your institution\'s key to access RoMEO.'),
  ));
  $form['romeo'] += _islandora_scholar_add_variable('islandora_scholar_issn_cache_time', 2, array(
    '#title' => t('ISSN cache time'),
    '#description' => t('The amount of time in days for which to maintain an ISSN ' .
      'value obtained by scanning an object.'),
  ));
  $form['romeo'] += _islandora_scholar_add_variable('islandora_scholar_romeo_cache_time', 2, array(
    '#title' => t('RoMEO cache time'),
    '#description' => t('The amount of time in days for which to maintain a copy of the markup ' .
      'for a given ISSN.'),
  ));


  return system_settings_form($form);
}
