<?php

/**
 * @file
 * This file holds supporting code for the scholar embargo module.
 */

/**
 * This builds form for embargoing an object.
 *
 * @param array $form
 *   The Drupal form definition.
 * @param array $form_state
 *   The Drupal form state.
 * @param AbstractObject $item
 *   The Object to embargo.
 *
 * @return array
 *   The Drupal form definition.
 */
function islandora_scholar_embargo_form(array $form, array &$form_state, $item) {
  $pid = $item->id;
  $label = $item->label;
  $options = array(
    '!object/level!' => t('Object-level embargo'),
  );

  // We're jsut using the PDF datastram at this time, but we'll eave this hear
  // in case we decide to expand.
  foreach ($item as $datastream) {
    $options[$datastream->id] = $datastream->label;
  }

  $embargo_date = "";
  $embargo_level = "!object/level!";
  $embargo_state = 'no_embargo';
  $choices = array();
  $prompt = t('Embargo until specified date.');
  $item_info = t('@label has no current embargo', array('@label' => $label));

  $object_result = $item->relationships->get(ISLANDORA_SCHOLAR_EMBARGO_RELS_URI, ISLANDORA_SCHOLAR_EMBARGO_EXPIRY_PRED);
  if (isset($object_result[0]['object']['value'])) {
    $embargo_date = $object_result[0]['object']['value'];
  }
  if ($item['PDF']) {
    $ds_result = $item['PDF']->relationships->get(ISLANDORA_SCHOLAR_EMBARGO_RELS_URI, ISLANDORA_SCHOLAR_EMBARGO_EXPIRY_PRED);
  }

  if (isset($ds_result[0]['object']['value'])) {
    $embargo_date = $ds_result[0]['object']['value'];
    $embargo_level = "PDF";
  }

  if (!empty($embargo_date)) {
    $prompt = t('Change curent embargo to specified date.');
    if ($embargo_date == 'indefinite') {
      $message = 'Indefinite';
      $embargo_state = 'Indefinite';
    }
    else {
      $date = new dateTime($embargo_date);
      $message = $date->format('M d, Y ');
      $embargo_state = 'date';
    }
    $choices['no_embargo'] = t("Lift Curent Embargo");
    $item_info = t("Embargo for @label is currently set to @date", array('@label' => $label, '@date' => $message));
  }

  /* XXX:  Chop 'em down to only those we want...
   * Another hook/configuration bit? */
  $used_options = array(
    '!object/level!' => '',
    'PDF' => '',
  );
  $options = array_intersect_key($options, $used_options);
  $date_array = NULL;
  if (!empty($embargo_date) && $embargo_date != 'indefinite') {
    $date_array = date_parse($embargo_date);
  }

  if ($embargo_date != 'indefinite') {
    $choices['indefinite'] = t('Embargo indefinitely (Indefinite embargos must be lifted manually).');
  }

  $choices ['date'] = $prompt;

  $form = array(
    '#tree' => TRUE,
    'prompt' => array(
      '#type' => 'item',
      '#markup' => '<h1>' . $item_info . '</h1>',
    ),
    'pid' => array(
      '#type' => 'value',
      '#value' => $pid,
    ),
    'dsid' => array(
      '#type' => 'select',
      '#title' => t('Item to be embargoed'),
      '#options' => $options,
      '#default_value' => $embargo_level,
    ),
    'embargo_select' => array(
      '#type' => 'radios',
      '#title' => t('Duration of Embargo'),
      '#required' => TRUE,
      '#default_value' => $embargo_state,
      '#options' => $choices,
    ),
    'embargo_date' => array(
      '#type' => 'date',
      '#title' => t('Embargo date'),
      '#description' => t('A date until which this item should be embargoed.'),
      '#default_value' => $date_array,
      '#after_build' => array('_embargo_set_year_range'),
      '#states' => array(
        'visible' => array(
          ':input[name="embargo_select"]' => array('value' => 'date'),
        ),
      ),
    ),
    'submit' => array(
      '#type' => 'submit',
      '#value' => t('Submit'),
    ),
  );

  return $form;
}

/**
 * Customizes date range for form element.
 */
function _embargo_set_year_range($form_element, $form_values) {
  $year = date("Y");
  $form_element['year']['#options'] = drupal_map_assoc(range($year, $year + 20));
  return $form_element;
}

/**
 * Facilitates adding without first lifting all associated embargoes...
 */
function islandora_scholar_embargo_lift_associated_except($pid, $dsid = NULL, $expiry = NULL) {

  $is = ISLANDORA_SCHOLAR_EMBARGO_RELS_URI;
  $eu = ISLANDORA_SCHOLAR_EMBARGO_EXPIRY_PRED;
  $query = <<<EOQ
PREFIX is:  <$is>
SELECT ?embargoed ?expiry
WHERE {{
    ?embargoed is:$eu ?expiry ;
               <fedora-model:hasModel> ?model .
      FILTER(sameTerm(?embargoed, <info:fedora/$pid>))
  }
  UNION {
    <info:fedora/$pid> <fedora-view:disseminates> ?embargoed ;
                       <fedora-model:hasModel> ?model .
    ?embargoed is:$eu ?expiry .
  }
}
EOQ;
  $item = islandora_object_load($pid);
  $results = $item->repository->ri->sparqlQuery($query);

  foreach ((array) $results as $result) {
    $e_pid = $result['embargoed']['value'];
    $e_dsid = NULL;
    $e_exp = $result['expiry']['value'];
    if (strpos($e_pid, '/') !== FALSE) {
      // Is DSID.
      list($e_pid, $e_dsid) = explode('/', $e_pid);
    }

    // XXX: Might get into trouble if we somehow have multiple embargo
    // statements for a single datastream, due to how we remove them from the
    // XACML.
    // Do not remove statements which match our parameters.
    if ($e_pid != $pid || $e_dsid != $dsid || $e_exp != $expiry) {
      islandora_scholar_embargo_lift_embargo($e_pid, $e_dsid);
    }
  }
}

/**
 * Implements hook_validate().
 */
function islandora_scholar_embargo_form_validate(array $form, array &$form_state) {
  if ($form_state['values']['embargo_select'] == 'date') {
    if (empty($form_state['values']['embargo_date'])) {
      form_set_error('embargo_date', t('Date must be provided, due to the selected duration.'));
    }
    $now = time();
    $time = $form_state['values']['embargo_date'];
    $date_string = $time['year'] . '-' . $time['month'] . '-' . $time['day'];
    $parsed = strtotime($date_string);
    if ($parsed <= $now) {
      form_set_error('embargo_date', t('Date parsing resulted in a date in the past (%date)!', array(
            '%date' => format_date($parsed),
          )));
    }
  }
}

/**
 * Implements hook_submit().
 */
function islandora_scholar_embargo_form_submit(array $form, array &$form_state) {
  $pid = $form_state['values']['pid'];
  $dsid = $form_state['values']['dsid'];
  if ($dsid == '!object/level!') {
    $dsid = NULL;
  }
  $date = NULL;

  if ($form_state['values']['embargo_select'] != 'no_embargo') {
    $date = 'indefinite';
    if ($form_state['values']['embargo_select'] == 'date') {
      $time = $form_state['values']['embargo_date'];
      $date_string = $time['year'] . '-' . $time['month'] . '-' . $time['day'];
      $date = gmdate("Y-m-d\TH:i:s\Z", strtotime($date_string));
    }
    islandora_scholar_embargo_set_embargo($pid, $dsid, $date);
  }

  islandora_scholar_embargo_lift_associated_except($pid, $dsid, $date);
}
