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
    'islandora_doi_handle_imported_metadata' => array(
      '#type' => 'radios',
      '#title' => t('How should embedded HTML in imported DOI metadata be handled?'),
      '#description' => t('For example, stripping tags will change "H&lt;sub&gt;2&lt;/sub&gt;O" to "H2O". Encoding tags will change  "H&lt;sub&gt;2&lt;/sub&gt;O" to  "H&amp;lt;sub&amp;gt;2&amp;lt;/sub&amp;gt;O". Re-decoding tags for display is not yet implemented in Islandora.'),
      '#options' => array(
        'strip' => t('Strip embedded HTML tags'),
        'encode' => t('Encode embedded HTML tags'),
      ),
      '#default_value' => variable_get('islandora_doi_handle_imported_metadata', 'strip'),
    ),
    'islandora_doi_url_config' => array(
      '#type' => 'fieldset',
      '#title' => t('DOI API configuration'),
      '#collapsible' => TRUE,
    ),
  );
  $form['islandora_doi_url_config'] += array(
    'islandora_doi_use_legacy_mode' => array(
      '#type' => 'checkbox',
      '#title' => t('Use legacy API (Crossref OpenURL)'),
      '#description' => t("Fetch metadata directly from Crossref's OpenURL API (account required!). If unchecked, use doi.org's content negotiation as explained by https://www.crosscite.org/docs.html."),
      '#default_value' => (islandora_doi_use_legacy_mode() === TRUE ? 1 : 0),
      // Ensure correct return value if selected (although 1 is the default).
      '#return_value' => 1,
    ),
    'islandora_doi_doiurl' => array(
      '#type' => 'textfield',
      '#title' => t('URL for Content Negotiation API'),
      '#description' => t("The URL which we will query in non-legacy mode to obtain metadata (it is probably not recommended to change this from the default value '@url').", array(
        '@url' => ISLANDORA_DOI_DOIURL_DEFAULT,
      )),
      '#default_value' => variable_get('islandora_doi_doiurl', ISLANDORA_DOI_DOIURL_DEFAULT),
      '#states' => array(
        'visible' => array(
          ':input[name="islandora_doi_use_legacy_mode"]' => array('checked' => FALSE),
        ),
      ),
    ),
    'islandora_doi_openurl' => array(
      '#type' => 'textfield',
      '#title' => t('URL for Crossref OpenURL ("legacy") API'),
      '#description' => t("The URL which we will query in legacy mode to obtain metadata. (it is probably not recommended to change this from the default value '@url').", array(
        '@url' => ISLANDORA_DOI_OPENURL_DEFAULT,
      )),
      '#default_value' => variable_get('islandora_doi_openurl', ISLANDORA_DOI_OPENURL_DEFAULT),
      '#states' => array(
        'visible' => array(
          ':input[name="islandora_doi_use_legacy_mode"]' => array('checked' => TRUE),
        ),
      ),
    ),
    'islandora_doi_openurl_pid' => array(
      '#type' => 'textfield',
      '#title' => t('OpenURL PID'),
      '#description' => t("Email address registered with Crossref's free query service. https://www.crossref.org/requestaccount/"),
      '#default_value' => variable_get('islandora_doi_openurl_pid', ISLANDORA_DOI_OPENURL_PID_DEFAULT),
      '#states' => array(
        'visible' => array(
          ':input[name="islandora_doi_use_legacy_mode"]' => array('checked' => TRUE),
        ),
      ),
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
  if ($form_state['values']['islandora_doi_use_legacy_mode'] === 1) {
    if ($form_state['values']['islandora_doi_openurl_pid'] == ISLANDORA_DOI_OPENURL_PID_DEFAULT) {
      form_set_error('islandora_doi_openurl_pid', t("Cannot use legacy mode, the default '@pid' is not a valid OpenURL PID. Please change it.", array(
        '@pid' => ISLANDORA_DOI_OPENURL_PID_DEFAULT,
      )));
    }

    form_load_include($form_state, 'inc', 'islandora_doi', 'includes/utilities');
    if (!islandora_doi_openurl_pid_valid($form_state['values']['islandora_doi_openurl_pid'])) {
      form_set_error('islandora_doi_openurl_pid', t("Cannot use legacy mode, there is an error with the provided OpenURL PID. Is this identifier registered properly?"));
    }
  }
}
