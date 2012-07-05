<?php

/**
 * Importer interface for the heavy-lifting.
 * @file
 */

/**
 * Scholar importer interface/abstract class.
 *
 * Implement the abstract functions (and those in the Import Object/item class)
 * and it should be able to work.
 */
abstract class IslandoraScholarBatchImporter {
  /**
   * A string containing the name of a timer used in the run_batch method.
   * @var string
   */
  protected $timer_name = 'islandora_scholar_batch_import';

  /**
   * A string containing the name of the class to actually do the heavy
   * lifting.
   * @var string
   */
  protected $item_class = "IslandoraScholarImportObject";

  /**
   * May be a  bit of a misnomer...  We don't really make any assumptions
   *   about what this holds...  We just pass it as a parameter to
   *   abstract methods...  Irrespective of whether the given implementation
   *   actually uses them.  Probably has to be serializable to work properly
   *   with the batch stuff.
   * @var mixed
   */
  protected $file;

  /**
   * The Fedora collection PID into which items should be imported as members.
   * @var string
   */
  protected $parent_pid;

  /**
   * Store the batch context, to avoid passing it to individual methods.
   * @var array
   */
  protected $context;

  /**
   *
   * @param $file
   *   May be a  bit of a misnomer...  We don't really make any assumptions
   *   about what this holds...  We just pass it along to the item class.
   * @param $context
   *   An array containing the batch context.
   */
  public function __construct($file, $parent_pid, &$context) {
    $this->file = $file;
    $this->parent_pid = $parent_pid;
    $this->context =& $context;
  }

  /**
   * Determine if we're ready to run our batch process...
   *
   * ... probably checking values.  Might almost be thought of as validation?
   *
   * @param $form_state
   *   An array containing the form state.
   * @return boolean
   *   Whether or not we have all the info to start the batch.
   */
  abstract public static function ready_for_batch(&$form_state);

  /**
   * Get the required info to pass into the file parameter of the constructor.
   *
   * @param $form_state
   *   An array containing the form state (very likely resulting from the
   *   submission of the form returned from self::get_form
   */
  abstract public static function get_batch_info(&$form_state);

  /**
   * Get the form for this importer...
   *
   * Could have a multiple-page form given ready_for_batch and the form state.
   */
  public static function get_form(&$form_state) {
    return array(
      'nothing' => array(
        '#type' => 'item',
        '#value' => t('This importer does not define a form...  This is a problem.'),
      ),
    );
  }

  /**
   * Get the number of items to import from $this->file
   *
   * @return int
   *   The number of items left to process.
   */
  public abstract function get_number();

  /**
   * Loop import until we have used up 1/3 of the max execution time.
   *
   * @param &$context
   *   A reference to the array representing the batch's context.
   */
  public function run_batch() {
    timer_start($this->timer_name);

    if (empty($this->context['sandbox'])) {
      $this->context['sandbox']['progress'] = 0;
      $this->context['sandbox']['max'] = $this->get_number();
    }
    $initial_progress = $this->context['sandbox']['progress'];

    $item_class = $this->item_class;

    module_load_include('inc', 'fedora_repository', 'api/fedora_item');

    //Assume we'll have enough time if less than 1/3 of the execution time has passed.
    //  If it's zero, we should have an infinite amount of time (running the batch
    //  from drush?)
    $met = intval(ini_get('max_execution_time')) * 1000;
    while ((
      $met > 0
        ||
      timer_read($this->timer_name) < ($met / 3)
        || (
      $this->context['sandbox']['progress'] - $initial_progress == 0
      ))
      && (
        $this->context['sandbox']['progress'] < $this->context['sandbox']['max']
      )) {
      $item = $item_class::get_one($this->file, $this->parent_pid);

      $result = NULL;
      if ($item) {
        if (empty($this->context['results']['pid_cache'])) {
          //Get enough PIDs for half of the remaining items...
          //  (plus one, so we'll always get at least one).
          $this->context['results']['pid_cache'] = Fedora_Item::get_next_PID_in_namespace(
            $item->pid_namespace,
            intval((($this->context['sandbox']['max'] - $this->context['sandbox']['progress']) / 2) + 1)
          );
        }
        $pid = array_shift($this->context['results']['pid_cache']);

        $result = $item->write_to_fedora($pid);

        if ($result === NULL) {
          //We failed...
          $this->context['results'][] = t('Failed to import title %title at item number %num.', array(
            '%title' => $item->get_title(),
            '%num' => $this->context['sandbox']['progress']));
        }
      }
      else {
        $this->context['results'][] = t('Failed to extract information for import item number %num.', array(
          '%num' => $this->context['sandbox']['progress'],
          ));
      }

      $this->context['sandbox']['progress']++;
    }

    if ($this->context['sandbox']['max'] > 0) {
      if ($this->context['sandbox']['progress'] < $this->context['sandbox']['max']) {
        //Treat as floating point to give feedback...
        $this->context['finished'] = 1.0 * $this->context['sandbox']['progress'] / $this->context['sandbox']['max'];
      }
      else {
        //Treat as integers to avoid floating point issues. (1 -> finished)
        $this->context['finished'] = $this->context['sandbox']['progress'] / $this->context['sandbox']['max'];
      }
    }

    timer_stop($this->timer_name);
  }
}

abstract class IslandoraScholarImportObject {
  public $pid_namespace = 'ir';
  public $pid_cache_size = 2;

  protected $title = 'No title';

  /**
   * Get the title.
   *
   * @return string
   *   A string containing the title (eventually used as a label).
   */
  public function get_title() {
    return $title;
  }

  /**
   * Get an item from the source
   *
   * Get one item from the source, and write the results back less
   * the one you're getting.  Return an instantiation of this class
   * representing the one extracted from the source.
   *
   * Return NULL if there's none to get.
   */
  public static abstract function get_one(&$source, $parent_pid);

  /**
   * Private constructor.
   *
   * Instantiations should be obtained via the "get_one" static method, which
   * should know how to extract and feed what is needed to the constructor.
   *
   * Need to extract the title in here somewhere...
   * @param $source
   */
  protected abstract function __construct($source, $parent_pid);

  /**
   * Create some MODS representing us.
   *
   * Don't really care how it is obtained, as long as it's a string containing
   * MODS XML.
   *
   * @return string
   */
  protected abstract function get_mods();

  /**
   * Create some DC representing us.
   *
   * @return string
   *   A string containing DC XML.
   */
  protected function get_dc() {
    $transform_filename = drupal_get_path('module', 'islandora_scholar_importer') . '/xsl/mods_to_dc.xsl';
    $transform = new DOMDocument();
    $transform->load($transform_filename);
    $doc = DOMDocument::loadXML($this->mods);
    if (isset($doc->documentElement)) {
      $xslt = new XSLTProcessor();
      $xslt->importStyleSheet($transform);
      $doc = $xslt->transformToDoc($doc->documentElement);
      return $doc->saveXML();
    }
  }

  /**
   * Write this item to Fedora.
   */
  public function write_to_fedora($pid = NULL) {
    global $user;
    module_load_include('inc', 'fedora_item', 'api/fedora_item');
    static $pid_cache = array();
    if ($pid === NULL) {
      if (empty($pid_cache)) {
        $pid_cache = Fedora_Item::get_next_PID_in_namespace($this->pid_namespace, $this->pid_cache_size);
      }
      $pid = array_shift($pid_cache);
    }

    $success = $item = Fedora_Item::ingest_new_item($pid, 'A', $this->title);
    if ($success) {
      $success = $item->add_datastream_from_string($this->get_mods(), 'MODS', 'MODS Record', 'text/xml', 'X');
      if ($success) {
        $dc = $this->get_dc();
        if ($dc) {
          $item->modify_datastream($this->get_dc(), 'DC', 'DC Record', 'text/xml');
          if (!$success) {
            drupal_set_message(t('Failed to upload DC to @pid in Fedora.', array('@pid' => $pid)));
          }
          else {
            $item->add_relationship('hasModel', 'ir:citationCModel', FEDORA_MODEL_URI);
            $item->add_relationship('isMemberOfCollection', $this->parent_pid);
          }
        }
        else {
          drupal_set_message(t('Failed to produce DC for @pid.', array('@pid' => $pid)));
        }
      }
      else {
        drupal_set_message(t('Failed to upload MODS to @pid in Fedora.', array('@pid' => $pid)), 'error');
      }
    }
    else {
      drupal_set_message(t('Failed to create item @pid in Fedora.', array('@pid' => $pid)), 'error');
    }
  }
}
