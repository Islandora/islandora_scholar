<?php

/**
 * @file
 * Module used to create an upload tab.
 */

/**
 * Display a form to add/modify the objects PDF datastream.
 *
 * Show the form to upload if the PDF does not exist on the given object.
 * Show the form to modify if there is a PDF.
 *
 * @param AbstractFedoraObject $object
 *   The object.
 *
 * @return array
 *   The Drupal form definition.
 */
function islandora_scholar_upload_view(AbstractFedoraObject $object) {
  $form_name = 'islandora_scholar_upload_form';
  if (isset($object['PDF'])) {
    $form_name = 'islandora_scholar_modify_form';
  }
  return drupal_get_form($form_name, $object);
}

/**
 * Gets the modify form.
 *
 * @param array $form
 *   The Drupal form definition.
 * @param array $form_state
 *   The Drupal form state.
 * @param AbstractFedoraObject $object
 *   The object.
 *
 * @return array
 *   The Drupal form definition.
 */
function islandora_scholar_modify_form(array $form, array &$form_state, AbstractFedoraObject $object) {
  $mods_str = trim($object['MODS']->content);

  $mods_doc = new DOMDocument();
  $mods_doc->loadXML($mods_str);
  $mods_xpath = new DOMXPath($mods_doc);
  $mods_xpath->registerNamespace('m', 'http://www.loc.gov/mods/v3');
  $usage = t('No usage statement');
  $mods_usages = $mods_xpath->query('//m:mods/m:accessCondition[@type="use and reproduction"]');
  if ($mods_usages->length > 0) {
    $usage = $mods_usages->item(0)->textContent;
  }
  $version = t('No version statement');
  $mods_versions = $mods_xpath->query('//m:mods/m:physicalDescription[@authority="local"]');
  if ($mods_versions->length > 0) {
    $version = $mods_versions->item(0)->textContent;
  }

  return array(
    'pid' => array(
      '#type' => 'value',
      '#value' => $object->id,
    ),
    'dsid' => array(
      '#type' => 'value',
      '#value' => 'PDF',
    ),
    'usage' => array(
      '#type' => 'item',
      '#title' => t('Usage'),
      '#markup' => $usage,
    ),
    'version' => array(
      '#type' => 'item',
      '#title' => t('Publication version'),
      '#markup' => $version,
    ),
    'delete_document' => array(
      '#name' => 'delete_document',
      '#type' => 'submit',
      '#value' => t('Delete Document/PDF'),
    ),
  );
}

/**
 * Submit handler for modify form, delete's the PDF datastream.
 *
 * @param array $form
 *   The Drupal form definition.
 * @param array $form_state
 *   The Drupal form state.
 */
function islandora_scholar_modify_form_submit(array $form, array &$form_state) {
  extract($form_state['values']);

  $object = islandora_object_load($pid);

  if ($form_state['clicked_button']['#name'] == 'delete_document') {
    // @todo Also remove the PDF's derivatives.
    $status = islandora_purge_datastream($object, $dsid);

    if ($status === NULL) {
      drupal_set_message(t("DON'T PANIC!, but we may have failed purging the datastream...  Some debug info: @info", array('@info' => print_r($status, TRUE))));
    }
  }
}

/**
 * Gets the upload form.
 *
 * @param array $form
 *   The Drupal form definition.
 * @param array $form_state
 *   The Drupal form state.
 * @param AbstractFedoraObject $object
 *   The object.
 *
 * @return array
 *   The Drupal form definition.
 */
function islandora_scholar_upload_form(array $form, array &$form_state, AbstractFedoraObject $object) {
  $get_default_value = function($name, $default) use(&$form_state) {
    return isset($form_state['values'][$name]) ? $form_state['values'][$name] : $default;
  };
  $upload_size = min((int) ini_get('post_max_size'), (int) ini_get('upload_max_filesize'));
  return array(
    '#type' => 'form',
    'upload_document' => array(
      '#type' => 'fieldset',
      '#title' => t('Upload Document'),
      'pid' => array(
        '#type' => 'value',
        '#value' => $object->id,
      ),
      'dsid' => array(
        '#type' => 'value',
        '#value' => 'PDF',
      ),
      'file' => array(
        '#type' => 'managed_file',
        '#title' => 'PDF file',
        '#required' => TRUE,
        '#default_value' => $get_default_value('file', NULL),
        '#upload_location' => 'temporary://',
        '#upload_validators' => array(
          'file_validate_extensions' => array('pdf'),
          'file_validate_size' => array($upload_size * 1024 * 1024),
        ),
      ),
      'version' => array(
        '#type' => 'radios',
        '#title' => t('Document Version'),
        '#options' => array(
          'PRE-PUBLICATION' => t('Pre-Publication'),
          'PUBLISHED' => t('Published'),
          'POST-PUBLICATION' => t('Post-Publication'),
          'OTHER' => t('Other'),
        ),
        '#required' => TRUE,
      ),
      'usage' => array(
        '#type' => 'radios',
        '#title' => t('Use Permission'),
        '#options' => array(
          'publisher' => t('Contact Publisher (I do not hold copyright).'),
          'author' => t('Contact Author (I hold the copyright and wish to retain all rights).'),
        ),
        '#required' => TRUE,
      ),
      'certifying' => array(
        '#type' => 'checkboxes',
        '#title' => t('Certify'),
        '#options' => array(
          'certify' => t('I certify that I have the right to upload this version.'),
        ),
        '#required' => TRUE,
      ),
      'submit' => array(
        '#type' => 'submit',
        '#value' => t('Upload'),
      ),
    ),
  );
}

/**
 * Updates the MODS datastream with the given usage and version.
 *
 * @param AbstractFedoraObject $object
 *   The object.
 * @param string $usage
 *   The usage either the 'publisher' or 'author' holds the copyright.
 * @param string $version
 *   The version of the uploaded document either 'PRE-PUBLICATION',
 *  'PUBLISHED', 'POST-PUBLICATION', 'OTHER'.
 */
function islandora_scholar_add_usage_and_version_elements_to_mods(AbstractFedoraObject $object, $usage, $version) {
  if (isset($object['MODS'])) {
    $mods_str = trim($object['MODS']->content);
    if (!$mods_str) {
      drupal_set_message(t('MODS in object @pid is empty...  Refusing to add usage and publication version statements alone.', array(
            '@pid' => $object->id)), 'error');
      return FALSE;
    }

    // Load the MODS...
    $mods_doc = new DOMDocument();
    $mods_doc->loadXML($mods_str);
    $mods_xpath = new DOMXPath($mods_doc);
    $mods_ns = 'http://www.loc.gov/mods/v3';
    $mods_xpath->registerNamespace('m', $mods_ns);
    // DOMXPath creates a copy...  let's use it instead.
    $mods_doc = &$mods_xpath->document;

    $mods_element = NULL;
    $mods_elements = $mods_xpath->query('//m:mods');
    if (count($mods_elements) > 0) {
      $mods_element = $mods_elements->item(0);
    }
    else {
      drupal_set_message(t('Could not find MODS element in object @pid is empty...  Refusing to add usage and publication version statements alone.', array(
            '@pid' => $object->id)), 'error');
      return FALSE;
    }

    $usage_element = NULL;
    $usage_elements = $mods_xpath->query('m:accessCondition[@type="use and reproduction"]', $mods_element);
    if ($usage_elements->length > 0) {
      // Found at least one...  Grab the first?
      $usage_element = $usage_elements->item(0);
    }
    else {
      // Didn't find any...  Create element with attributes.
      $usage_element = $mods_doc->createElementNS($mods_ns, 'accessCondition');
      $usage_element->setAttribute('type', 'use and reproduction');
      $mods_element->appendChild($usage_element);
    }

    // Delete all children.
    while ($node = $usage_element->firstChild) {
      $usage_element->removeChild($node);
    }
    $usage_element->appendChild($mods_doc->createTextNode($usage));

    $version_element = NULL;
    $version_elements = $mods_xpath->query('m:physicalDescription[@authority="local"]', $mods_element);
    if ($version_elements->length > 0) {
      // Found at least one... Grab the first?
      $version_element = $version_elements->item(0);
    }
    else {
      // Didn't find any... Create element with attributes.
      $version_element = $mods_doc->createElementNS($mods_ns, 'physicalDescription');
      $version_element->setAttribute('authority', 'local');
      $mods_element->appendChild($version_element);
    }

    // Delete all children.
    while ($node = $version_element->firstChild) {
      $version_element->removeChild($node);
    }
    $version_element->appendChild($mods_doc->createTextNode($version));

    $object['MODS']->content = $mods_doc->saveXML();
  }
  else {
    drupal_set_message(t('No MODS in object @pid...  Refusing to add usage and publication version statements alone.', array(
          '@pid' => $object->id)), 'error');
  }
}

/**
 * Upload form Submit handler, adds PDF datastream.
 *
 * Also updates the MODS usage and version for the PDF datastream.
 *
 * @param array $form
 *   The Drupal form definition.
 * @param array $form_state
 *   The Drupal form state.
 */
function islandora_scholar_upload_form_submit(array $form, array &$form_state) {
  extract($form_state['values']);

  $object = islandora_object_load($pid);

  try {
    $datastream = $object->constructDatastream($dsid, 'M');
    $datastream->label = 'PDF Datastream';
    $datastream->mimetype = 'application/pdf';
    $file = file_load($file);
    $path = drupal_realpath($file->uri);
    $datastream->setContentFromFile($path);

    $object->ingestDatastream($datastream);

    islandora_scholar_add_usage_and_version_elements_to_mods($object, $usage, $version);
  }
  catch (Exception $e) {
    drupal_set_message(t('@message', array('@message' => check_plain($e->getMessage()))), 'error');
  }
}
