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
  $options = array(
    'object' => t('Object-level embargo'),
    'PDF' => t('PDF'),
  );
  $embargo_results = islandora_scholar_embargo_get_embargoed($item);

  // Table seems somewhat of overkill for now, but as the embargo offering
  // grows more robust this sort of formatting for the UI will make sense.
  $rows = array();
  $current_embargos = array();
  $date = '';
  foreach ($embargo_results as $result) {
    if ($result['obj']['value'] == $item->id) {
      // Object.
      $type = t('Object');
    }
    else {
      $type = str_replace("$pid/", '', $result['obj']['value']);
    }
    $date = $result['date']['value'];
    if ($date != 'indefinite') {
      $date = new dateTime($date);
      $date = $date->format('M d, Y ');
    }
    else {
      $date = ucwords($date);
    }
    $rows[] = array(
      $type,
      $date,
    );
    $current_embargos[] = array(
      'type' => $type,
      'date' => $date,
    );
  }
  $form_state['islandora_embargo'] = $current_embargos;
  $embargoed = TRUE;
  if (empty($embargo_results)) {
    $embargoed = FALSE;
  }

  // We're just using the PDF datastram at this time, but we'll leave this here
  // in case we decide to expand.
  foreach ($item as $datastream) {
    $options[$datastream->id] = $datastream->label;
  }

  /* XXX:  Chop 'em down to only those we want...
   * Another hook/configuration bit? */
  $used_options = array(
    'object' => '',
    'PDF' => '',
  );
  $options = array_intersect_key($options, $used_options);

  $form = array(
    '#tree' => TRUE,
    'pid' => array(
      '#type' => 'value',
      '#value' => $pid,
    ),
  );
  if (!$embargoed) {
    $form['dsid'] = array(
      '#type' => 'select',
      '#title' => t('Item to be embargoed'),
      '#options' => $options,
      '#default_value' => 'object',
    );
    $date_state = array(
      ':input[name="update_date_options"]' => array('value' => 'change_embargo_date'),
    );
  }
  else {
    $form['update_options'] = array(
      '#type' => 'radios',
      '#title' => t('Embargo Options'),
      '#options' => array(
        'update_date' => t('Update embargo'),
        'lift_embargo' => t('Lift the embargo'),
      ),
      '#required' => TRUE,
    );
    $date_state = array(
      ':input[name="update_options"]' => array('value' => 'update_date'),
      ':input[name="update_date_options"]' => array('value' => 'change_embargo_date'),
    );
  }
  $form['update_date_options'] = array(
    '#type' => 'radios',
    '#title' => check_plain($embargoed ? t('Update Options') : t('Embargo Options')),
    '#options' => array(
      'change_embargo_date' => $embargoed ? t('Change the embargo date') : t('Set the embargo date'),
      'indefinite_embargo' => t('Embargo indefinitely (Indefinite embargos must be lifted manually).'),
    ),
    '#default_value' => ($date === 'Indefinite') ? 'indefinite_embargo' : 'change_embargo_date',
    '#states' => array(
      'visible' => array(
        ':input[name="update_options"]' => array('value' => 'update_date'),
      ),
    ),
  );
  $form['embargo_date'] = array(
    '#type' => 'date',
    '#title' => t('Embargo date'),
    '#description' => t('A date until which this item should be embargoed.'),
    '#default_value' => ($embargoed && $date !== 'Indefinite') ? date_parse($date) : NULL,
    '#after_build' => array('_embargo_set_year_range'),
    '#states' => array(
      'visible' => $date_state,
    ),
  );
  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Update'),
  );

  $params = array(
    'header' => array(
      t('Type'),
      t('Embargo Expiry'),
    ),
    'rows' => $rows,
    'attributes' => array(),
    'caption' => '',
    'colgroups' => array(),
    'sticky' => FALSE,
    'empty' => t('No items currently embargoed!'),
  );
  $form['embargo_table'] = array(
    '#markup' => theme('table', $params),
    '#weight' => 0,
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
  $embargoed = !empty($form_state['islandora_embargo']);
  if (($embargoed && isset($form_state['values']['update_options']) && $form_state['values']['update_options'] !== 'lift_embargo') || (!$embargoed)) {
    if ($form_state['values']['update_date_options'] === 'change_embargo_date') {
      $date = $form_state['values']['embargo_date'];
      $parsed_date = format_string('@year-@month-@day', array(
        '@year' => $form_state['values']['embargo_date']['year'],
        '@month' => $form_state['values']['embargo_date']['month'],
        '@day' => $form_state['values']['embargo_date']['day'],
      ));
      $time_now = time();
      $parsed = strtotime($parsed_date);
      if ($embargoed) {
        // This will need to be updated in the future to handle multiple
        // embargos on a single object.
        foreach ($form_state['islandora_embargo'] as $embargo_vals) {
          $embargo_date = $embargo_vals['date'];
          break;
        }
        if (strtotime($embargo_date) === $parsed) {
          form_set_error('embargo_date', t('The selected date matches the currently embargoed date!'));
        }
      }
      if ($parsed <= $time_now) {
        form_set_error('embargo_date', t('You cannot specify a date in the past!'));
      }
    }
    elseif ($form_state['values']['update_date_options'] === 'indefinite_embargo') {
      // This will need to be updated in the future to handle multiple embargos
      // on a single object.
      $embargo_date = '';
      foreach ($form_state['islandora_embargo'] as $embargo_vals) {
        $embargo_date = $embargo_vals['date'];
        break;
      }
      if ($embargo_date === 'Indefinite') {
        form_set_error('update_date_options', t("The embargo's duration is already set indefinitely!"));
      }
    }
  }
}

/**
 * Implements hook_submit().
 */
function islandora_scholar_embargo_form_submit(array $form, array &$form_state) {
  $embargoed = !empty($form_state['islandora_embargo']);
  $pid = $form_state['values']['pid'];
  $object = islandora_object_load($pid);
  // We are lifting an embargo from an object that already has one.
  if (isset($form_state['values']['update_options']) && $form_state['values']['update_options'] === 'lift_embargo') {
    // This will have to be re-defined if and when we decide to allow multiple
    // separate embargoing on an object likely by using this function:
    // islandora_scholar_embargo_lift_associated_except(). For now we can only
    // have one embargo.
    foreach ($form_state['islandora_embargo'] as $embargo_vals) {
      $type = $embargo_vals['type'];
      $dsid = NULL;
      if ($type !== 'Object') {
        $dsid = $type;
      }
      islandora_scholar_embargo_lift_embargo($pid, $dsid);
      drupal_set_message(t('The embargo on @label (@pid) has been lifted!', array(
        '@label' => $object->label,
        '@pid' => $pid,
      )));
    }
  }
  elseif (isset($form_state['values']['update_date_options'])) {
    // Embargoed indefinitely.
    if ($form_state['values']['update_date_options'] === 'indefinite_embargo') {
      $end = 'indefinite';
    }
    else {
      $time = $form_state['values']['embargo_date'];
      $date_string = $time['year'] . '-' . $time['month'] . '-' . $time['day'];
      $end = gmdate("Y-m-d\TH:i:s\Z", strtotime($date_string));
    }

    // Object is currently embargoed and we are updating things.
    if ($embargoed) {
      foreach ($form_state['islandora_embargo'] as $embargo_vals) {
        $type = $embargo_vals['type'];
        break;
      }
      if ($type === 'Object') {
        $dsid = NULL;
      }
      else {
        $dsid = $type;
      }
    }
    // New embargo entry.
    else {
      if ($form_state['values']['dsid'] === 'object') {
        $dsid = NULL;
      }
      else {
        $dsid = $form_state['values']['dsid'];
      }
    }
    // Currently we just overwrite what's already in place. In the future, for
    // multiple embargos on a single object, this will have to be adjusted.
    islandora_scholar_embargo_set_embargo($pid, $dsid, $end);
    islandora_scholar_embargo_set_display_message($pid, $dsid, $end);
  }
}

/**
 * Retrieve all embargoed items from an object.
 *
 * @param AbstractObject $object
 *   An AbstractObject representing a Fedora object.
 *
 * @return array
 *   An array containing the list of embargoed items on an object
 */
function islandora_scholar_embargo_get_embargoed($object) {
  $current_embargo = <<<EOQ
PREFIX is: <info:islandora/islandora-system:def/scholar#>
SELECT ?obj ?date
FROM <#ri>
WHERE {
  ?obj is:embargo-until ?date .
  FILTER(REGEX(STR(?obj),'$object->id'))
}
EOQ;
  $embargo_results = $object->repository->ri->sparqlQuery($current_embargo, 'unlimited', '0');
  return $embargo_results;
}

/**
 * Retrieve all embargoed items from an object.
 *
 * @return array
 *   An array containing the list of embargoed items on an object
 */
function islandora_scholar_embargo_get_all_embargoed() {
  $current_embargo = <<<EOQ
PREFIX is: <info:islandora/islandora-system:def/scholar#>
SELECT ?obj ?date
FROM <#ri>
WHERE {
  ?obj is:embargo-until ?date
}
ORDER BY ?obj
EOQ;
  $connection = islandora_get_tuque_connection();
  $embargo_results = $connection->repository->ri->sparqlQuery($current_embargo, 'unlimited', '0');
  return $embargo_results;
}

/**
 * Displays a drupal_set_message when setting an embargo through the form.
 *
 * @param string $pid
 *   Pid of the object being embargoed;
 * @param string|NULL $dsid
 *   The DSID of the datastream we are embargoing, or NULL if it's the object.
 * @param string $end
 *   Either "indefinite" or a valid date string.
 */
function islandora_scholar_embargo_set_display_message($pid, $dsid, $end) {
  $object = islandora_object_load($pid);
  if (is_null($dsid)) {
    if ($end === 'indefinite') {
      drupal_set_message(t('A object-level embargo has been set on @label (@pid) indefinitely.',
          array(
            '@label' => $object->label,
            '@pid' => $pid,
          )
      ));
    }
    else {
      $date = new dateTime($end);
      $date = $date->format('M d, Y');
      drupal_set_message(t('A object-level embargo has been set on @label (@pid) until @duration.',
           array(
             '@label' => $object->label,
             '@pid' => $pid,
             '@duration' => $date,
           )
       ));
    }
  }
  else {
    if ($end === 'indefinite') {
      drupal_set_message(t('A @dsid datastream embargo has been set on @label (@pid) indefinitely.',
          array(
            '@dsid' => $dsid,
            '@label' => $object->label,
            '@pid' => $pid,
          )
      ));
    }
    else {
      $date = new dateTime($end);
      $date = $date->format('M d, Y');
      drupal_set_message(t('A @dsid datastream embargo has been set on @label (@pid) until @duration.',
           array(
             '@dsid' => $dsid,
             '@label' => $object->label,
             '@pid' => $pid,
             '@duration' => $date,
           )
       ));
    }
  }
}
