<?php

/**
 * @file
 *   This file holds supporting admin functions for the embargo module.
 */


/**
 * Admin form
 *
 * @param array $form
 * @param array $form_state
 * @return array
 */

function islandora_embargo_admin($form, &$form_state) {
  $options = islandora_embargo_get_content_models_as_option_array();
  $selected = variable_get('islandora_embargo_content_models', array('ir:citationCModel'));
  foreach ($selected as $cmodel) {
    $options = array($cmodel => $options[$cmodel]) + $options;
  }

  foreach ($options as $key => $value) {
    $rows[$key] = array(
      'pid' => $key,
      'title' => $value,
    );
    in_array($key, $selected) ? $defaults[$key] = TRUE : $defaults[$key] = FALSE;
  }
  $header = array(
    'pid' => array('data' => t('PID')),
    'title' => array('data' => t('Content Model')),
  );
  $form = array();
  $form['intro'] = array(
    '#type' => 'item',
    '#title' => 'Select Content Models for objects to be embargoed',
    '#description' => t('Objects defined by selected Content Models will be eligible for embargoing.'),
  );
  $form['the_table'] = array(
    '#type' => 'tableselect',
    '#header' => $header,
    '#options' => $rows,
    '#default_value' => $defaults,
  );

  $form['actions'] = array(
    '#type' => 'actions'
  );

  $form['actions']['configure'] = array(
    '#type' => 'submit',
    '#value' => t('Configure Embargo'),
    '#weight' => 1,
    '#submit' => array('islandora_embargo_admin_submit'),
  );

  $form['actions']['reset'] = array(
    '#type' => 'submit',
    '#value' => t('Reset to default'),
    '#weight' => 2,
    '#submit' => array('islandora_embargo_admin_submit'),
  );


  return $form;
}

/**
 * Admin submit
 *
 * @param array $form
 * @param array $form_state
 *
 */

function islandora_embargo_admin_submit($form, &$form_state) {
  $op = $form_state['clicked_button']['#value'];
  switch ($op) {
    case 'Reset to default':
      variable_del('islandora_embargo_content_models');
      break;
    default :
      $enabled = array_filter($form_state['values']['the_table']);
      variable_set('islandora_embargo_content_models', $enabled);
  }
}

/**
 * Function: get_content_models_as_option_array
 *
 * Description: Returns an associative array of all available content models in Fedora instance
 *
 * @return array
 */
function islandora_embargo_get_content_models_as_option_array() {
  $connection = islandora_get_tuque_connection();
  $restricted = variable_get('islandora_namespace_restriction_enforced', FALSE);
  $allowed_string = variable_get('islandora_pids_allowed', 'default: demo: changeme: islandora:');
  $namespaces = explode(':', $allowed_string);
  foreach ($namespaces as $namespace) {
    if ($namespace) {
      $allowed[] = trim($namespace);
    }
  }
  $query = 'select $object $title from <#ri>
          where ($object <fedora-model:label> $title
          and ($object <fedora-model:hasModel> <info:fedora/fedora-system:ContentModel-3.0>
          or $object <fedora-rels-ext:isMemberOfCollection> <info:fedora/islandora:ContentModelsCollection>)
          and $object <fedora-model:state> <info:fedora/fedora-system:def/model#Active>)
          order by $title';

  $list = $connection->repository->ri->itqlQuery($query, 'unlimited');
  $options = array();
  foreach ($list as $item) {   //removes  blanks
    if ($item) {
      $item_namespace = explode(':', $item['object']['value']);
      if (!$restricted || in_array($item_namespace[0], $allowed)) {

        if (!preg_match('/fedora-system/', $item['object']['value'])) {
          $options[$item['object']['value']] = $item['title']['value'];
        }
      }
    }
  }

  return $options;
}