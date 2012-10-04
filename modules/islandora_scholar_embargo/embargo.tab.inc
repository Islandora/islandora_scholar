<?php
/**
 * @file
 *   This file holds supporting code for the scholar embargo module.
 */

class EmbargoView {

  /**
   * Get a tabset to merge into the set of Islandora tabs.
   *
   * @param string $pid
   *   A string containing the PID for which we're to render a view.
   *
   * @return array
   *   A tabset (to be merged with existing tabs).
   */
  public static function get_tabs($pid) {
    $tab_set = array();
    /*
     * @todo implement $is_owner access
     * if (user_access('ingest new fedora objects') || $is_owner) {
     */
    if (user_access('ingest new fedora objects')) {
      $tab_set['embargo'] = array(
        '#title' => t('Embargo'),
        '#type' => 'tabpage',
        '#tab_name' => 'embargo',
        'embargo' => array(
          '#title' => t('Set Embargo'),
          '#type' => 'fieldset',
          '#value' => drupal_get_form('islandora_scholar_embargo_form', $pid),
        ),
      );
    }

    return $tab_set;
  }
}

/**
 * This funciton will build a form for embargoing an object.
 *
 * @param array $form_state
 *   The state of the form.
 * @param string $pid
 *   The Fedora PID to build a form for.
 */
function islandora_scholar_embargo_form(&$form_state, $pid) {
  module_load_include('inc', 'fedora_repository', 'api/fedora_item');
  $item = new Fedora_Item($pid);

  // XXX: Get the options...  Probably a good place for a hook?  Anyway...
  $options = array(
    // XXX: Magic value...  Invalid DSID, so we should be good.
    '!object/level!'  => t('Object-level embargo'),
  );
  foreach ($item->datastreams as $dsid => $values) {
    $options[$dsid] = $values['label'];
  }

  // Retrieve the embargo status for the object.
  $query = "select ?e from <#ri> where { <info:fedora/$pid> <info:islandora/islandora-system:def/scholar#embargo-until> ?e }";
  $result = ObjectHelper::performSparqlQuery($query);

  $embargo_stat = "no_embargo";
  $embargo_date = "";
  $embargo_level = "!object/level!";


  if (empty($result)) {
    // Object level embargo isn't set - try datastream level.
    $query = "select ?stream ?e from <#ri> where { <info:fedora/$pid> <fedora-view:disseminates> ?stream . ?stream <info:islandora/islandora-system:def/scholar#embargo-until> ?e }";
    $result = ObjectHelper::performSparqlQuery($query);

    /* Now we have a list of datastreams that are embargoed
     * and how they are embargoed.*/
    if (empty($result)) {
      // No embargo detected.
    }
    else {
      $exp = explode("/", $result[0]['stream']);
      $embargo_level = end($exp);
    }
  }
  else {
    $embargo_level = "!object/level!";
  }
  /* Now we've tried filling $result,
   * if theres nothing in it, then there's no embargo.*/
  if (!empty($result)) {
    $embargo_stat = $result[0]['e'];
    if ($embargo_stat !== "indefinite") {
      $embargo_date = $embargo_stat;
      $embargo_stat = "date";
    }
  }

  /* XXX:  Chop 'em down to only those we want...
   * Another hook/configuration bit?*/
  $used_options = array(
    '!object/level!' => '',
    'PDF' => '',
  );
  $options = array_intersect_key($options, $used_options);

  $form = array(
    '#tree' => TRUE,
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
      '#default_value' => $embargo_stat,
      '#options' => array(
        'no_embargo' => t('Do not embargo/Clear embargo'),
        'indefinite' => t('Embargo indefinitely (must be lifted manually)'),
        'date' => t('Embargo until date specified'),
      ),
    ),
    'embargo_date' => array(
      '#type' => 'datepicker',
      '#title' => t('Embargo date'),
      '#description' => t('A date until which this item should be embargoed.'),
      '#default_value' => $embargo_date,
    ),
    'submit' => array(
      '#type' => 'submit',
      '#value' => t('Submit'),
    ),
  );

  return $form;
}

/**
 * Facilitates adding without first lifting all associated embargoes...
 */
function islandora_scholar_embargo_lift_associated_except($pid, $dsid = NULL, $expiry = NULL) {
  module_load_include('inc', 'fedora_repository', 'ObjectHelper');
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
  $results = ObjectHelper::performSparqlQuery($query);

  foreach ((array) $results as $result) {
    $e_pid = $result['embargoed'];
    $e_dsid = NULL;
    $e_exp = $result['expiry'];
    if (strpos($e_pid, '/') !== FALSE) {
      // Is DSID.
      list($e_pid, $e_dsid) = explode('/', $e_pid);
    }

    /* XXX:  Might get into trouble if we somehow have multiple embargo
      statements for a single datastream, due to how we remove them from the
      XACML.*/
    // Do not remove statements which match our parameters.
    if ($e_pid != $pid || $e_dsid != $dsid || $e_exp != $expiry) {
      islandora_scholar_embargo_lift_embargo($e_pid, $e_dsid);
    }
  }
}

/**
 * Implements hook_validate().
 * Validates the embargo_form form.
 *
 * @param arrray $form
 *   The form being validated.
 * @param array $form_state
 *   The state of the form being validated.
 */
function islandora_scholar_embargo_form_validate ($form, $form_state) {
  if ($form_state['values']['embargo_select'] == 'date') {
    if (empty($form_state['values']['embargo_date'])) {
      form_set_error('embargo_date', t('Date must be provided, due to the selected duration.'));
    }
    $now = time();
    $time = $form_state['values']['embargo_date'];
    $parsed = strtotime($time);
    if ($parsed <= $time) {
      form_set_error('embargo_date', t('Date parsing resulted in a date in the past (%date)!', array(
        '%date' => format_date($parsed),
      )));
    }
  }
  elseif (!empty($form_state['values']['embargo_date'])) {
    /* Set to empty string if no-embargo was selected.
     * @XXX In the future this could be made more user friendly
     *   with ahah to reset the date field.
     */
    drupal_set_message(t('Embargo date field ignored due to embargo duration set.'));
  }
}

/**
 * Implements hook_submit().
 * This function will handle the submission of the embargo form.
 *
 * @param unknown_type $form
 *   The form being submitted.
 * @param unknown_type $form_state
 *   The state of the form being submited.
 */
function islandora_scholar_embargo_form_submit ($form, $form_state) {
  $pid = $form_state['values']['pid'];
  $dsid = $form_state['values']['dsid'];
  if ($dsid == '!object/level!') {
    $dsid = NULL;
  }
  $date = NULL;

  if ($form_state['values']['embargo_select'] != 'no_embargo') {
    $date = 'indefinite';
    if ($form_state['values']['embargo_select'] == 'date') {
      $date = gmdate("Y-m-d\TH:i:s\Z", strtotime($form_state['values']['embargo_date']));
    }
    islandora_scholar_embargo_set_embargo($pid, $dsid, $date);
  }

  islandora_scholar_embargo_lift_associated_except($pid, $dsid, $date);
}
