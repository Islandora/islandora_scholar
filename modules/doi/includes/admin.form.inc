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
  $form += array(
    'islandora_doi_openurl' => array(
      '#type' => 'textfield',
      '#title' => t('DOI OpenURL'),
      '#description' => t('The URL which we will query to obtain our CrossREF information.'),
      '#default_value' => variable_get('islandora_doi_openurl', 'http://www.crossref.org/openurl'),
    ),
    'islandora_doi_openurl_pid' => array(
      '#type' => 'textfield',
      '#title' => t('OpenURL PID'),
      '#description' => t('An identifier to call yourself, for the OpenURL endpoint. To use this service you first need to register for an account here: http://www.crossref.org/requestaccount/'),
      '#default_value' => variable_get('islandora_doi_openurl_pid', 'user@example.com'),
    ),
    'save_configuration' => array(
      '#type' => 'submit',
      '#name' => 'saveConfiguration',
      '#default_value' => t('Save configuration'),
    ),
  );

  return $form;
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

/**
 * Submit handler for the Islandora DOI admin form.
 *
 * @param array $form
 *   An array representing a form in Drupal.
 * @param array $form_state
 *   An array containing the Drupal form state.
 */
function islandora_doi_admin_form_submit($form, &$form_state) {
  variable_set('islandora_doi_openurl', $form_state['values']['islandora_doi_openurl']);
  variable_set('islandora_doi_openurl_pid', $form_state['values']['islandora_doi_openurl_pid']);
  drupal_set_message(
    t("The configuration options have been saved.")
  );
}
