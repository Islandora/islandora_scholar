<?php
/**
 * @file
 * Holds default rules.
 */

/**
 * Implements hook_default_rules_configuration().
 */
function islandora_scholar_embargo_default_rules_configuration() {
  $datastream_expired_rule = rules_reaction_rule();
  $object_expired_rule = rules_reaction_rule();
  $datastream_expired_rule->event('islandora_scholar_datastream_embargo_expired')
    ->action(
      'islandora_scholar_embargo_lift_embargo'
    );
  $object_expired_rule->event('islandora_scholar_object_embargo_expired')
    ->action(
      'islandora_scholar_embargo_lift_embargo'
    );

  return array(
    'islandora_scholar_embargo_datastream_lift_expired_embargo' => $datastream_expired_rule,
    'islandora_scholar_embargo_object_lift_expired_embargo' => $object_expired_rule,
  );
}
