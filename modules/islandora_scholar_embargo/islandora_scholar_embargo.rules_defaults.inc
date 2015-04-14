<?php
/**
 * @file
 * Holds default rules.
 */

/**
 * Implements hook_default_rules_configuration().
 */
function islandora_scholar_embargo_default_rules_configuration() {
  // @codingStandardsIgnoreStart
  $datastreams_expired_rule_export = <<<'EOE'
{ "islandora_scholar_embargo_datastreams_lift_expired_embargo" : {
    "LABEL" : "islandora_scholar_embargo_datastreams_lift_expired_embargo",
    "PLUGIN" : "reaction rule",
    "OWNER" : "rules",
    "REQUIRES" : [ "islandora_scholar_embargo" ],
    "ON" : { "islandora_scholar_datastreams_embargo_expired" : [] },
    "DO" : [
      { "islandora_scholar_embargo_lift_embargo" : { "object" : [ "object" ], "datastream" : [ "datastreams" ] } }
    ]
  }
}
EOE;
  // @codingStandardsIgnoreEnd
  $datastreams_expired_rule = rules_import($datastreams_expired_rule_export);
  $object_expired_rule = rules_reaction_rule();
  $object_expired_rule->event('islandora_scholar_object_embargo_expired')
    ->action(
      'islandora_scholar_embargo_lift_embargo'
    );
  return array(
    'islandora_scholar_embargo_datastreams_lift_expired_embargo' => $datastreams_expired_rule,
    'islandora_scholar_embargo_object_lift_expired_embargo' => $object_expired_rule,
  );
}
