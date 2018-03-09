<?php

/**
 * @file
 * Handles the display/submission of the admin settings form for this module.
 */

/**
 * Defines the admin settings form.
 *
 * @param array $form
 *   The Drupal form definition.
 * @param array $form_state
 *   The Drupal form state.
 *
 * @return array
 *   The Drupal form definition.
 */
function islandora_doi_admin_form(array $form, array &$form_state) {
  $form += array(
    'islandora_doi_use_legacy_mode' => array(
      '#type' => 'checkbox',
      '#title' => t('Use legacy retrieval mode'),
      '#description' => t('Keep fetching metadata directly from CrossREF\'s OpenURL (account required!). If unchecked, use content negociation as explained by https://www.crosscite.org.'),
      '#default_value' => variable_get('islandora_doi_use_legacy_mode', variable_get('islandora_doi_openurl_pid', ISLANDORA_DOI_OPENURL_PID_DEFAULT) !== ISLANDORA_DOI_OPENURL_PID_DEFAULT),
    ),
    'islandora_doi_url_config' => array(
      '#type' => 'fieldset',
      '#title' => t('DOI URL configuration'),
      '#description' => t('Various URL related parameters used by the metadata retrieval via DOI.'),
      '#collapsible' => TRUE,
    ),
  );
  $form['islandora_doi_url_config'] += array(
    'islandora_doi_doiurl' => array(
      '#type' => 'textfield',
      '#title' => t('DOI URL supporting content negociation'),
      '#description' => t('The URL which we will query in non-legacy mode to obtain our metadata (it is probably not recommended to change this from the default value \'@url\').', array(
        '@url' => ISLANDORA_DOI_DOIURL_DEFAULT,
      )),
      '#default_value' => variable_get('islandora_doi_doiurl', ISLANDORA_DOI_DOIURL_DEFAULT),
    ),
    'islandora_doi_openurl' => array(
      '#type' => 'textfield',
      '#title' => t('DOI OpenURL'),
      '#description' => t('The URL which we will query in legacy mode to obtain our CrossREF information.'),
      '#default_value' => variable_get('islandora_doi_openurl', ISLANDORA_DOI_OPENURL_DEFAULT),
    ),
    'islandora_doi_openurl_pid' => array(
      '#type' => 'textfield',
      '#title' => t('OpenURL PID'),
      '#description' => t('An identifier to call yourself, for the OpenURL endpoint. To use this service you first need to register for an account here: http://www.crossref.org/requestaccount/'),
      '#default_value' => variable_get('islandora_doi_openurl_pid', ISLANDORA_DOI_OPENURL_PID_DEFAULT),
    ),
  );

  return system_settings_form($form);
}
