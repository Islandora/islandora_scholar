<?php

/**
 * @file
 * Handles the display/submission of the admin settings form for Islandora DOI.
 */

/**
 * Defines the settings for the Islandora DOI admin form.
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
  form_load_include($form_state, 'inc', 'islandora_doi', 'includes/utilities');
  $form += array(
    'islandora_doi_use_legacy_mode' => array(
      '#type' => 'checkbox',
      '#title' => t('Use legacy retrieval mode'),
      '#description' => t("Keep fetching metadata directly from CrossREF's OpenURL (account required!). If unchecked, use content negotiation as explained by https://www.crosscite.org."),
      '#default_value' => (islandora_doi_use_legacy_mode() === TRUE ? 1 : 0),
      // Ensure correct return value if selected (although 1 is the default).
      '#return_value' => 1,
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
      '#title' => t('DOI URL supporting content negotiation'),
      '#description' => t("The URL which we will query in non-legacy mode to obtain our metadata (it is probably not recommended to change this from the default value '@url').", array(
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

/**
 * Validation function for the Islandora DOI admin form.
 *
 * @param array $form
 *   The Drupal form definition.
 * @param array $form_state
 *   The Drupal form state.
 */
function islandora_doi_admin_form_validate($form, &$form_state) {
  $openurl_pid = $form_state['values']['islandora_doi_openurl_pid'];
  if ($openurl_pid == 'user@example.com') {
    form_set_error('islandora_doi_openurl_pid', t("Please update the OpenURL PID, 'user@example.org' isn't valid."));
  }

  module_load_include('inc', 'islandora_doi', 'includes/utilities');
  if (!islandora_doi_openurl_pid_valid($openurl_pid)) {
    form_set_error('islandora_doi_openurl_pid', t("Error with provided OpenURL PID; is this email registered properly?"));
  }
}
