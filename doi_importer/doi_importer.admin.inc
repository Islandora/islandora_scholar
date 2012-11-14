<?php

function doi_importer_admin_form($form, &$form_state) {
  $form += array(
    'doi_importer_openurl' => array(
      '#type' => 'textfield',
      '#title' => t('DOI OpenURL'),
      '#description' => t('The URL which we will query to obtain our CrossREF information.'),
      '#default_value' => variable_get('doi_importer_openurl', 'http://www.crossref.org/openurl'),
    ),
    'doi_importer_openurl_pid' => array(
      '#type' => 'textfield',
      '#title' => t('OpenURL PID'),
      '#description' => t('An identifier to call yourself, for the OpenURL endpoint.'),
      '#default_value' => variable_get('doi_importer_openurl_pid', 'zac@yesguy.biz'),
    ),
  );

  return system_settings_form($form);
}

