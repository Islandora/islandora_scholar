<?php

/**
 * @file 
 *   this file is used to convert mods data into json for citeproc-js
 *  @author
 *    William Panting
 *  @author
 *    Zachary Howarth-Schueler
 *  @author
 *    Nigel Banks (Added some sub functions to deal with errors from missing nodes in the mods) General clean up.
 */

/**
 * Converts the MODS to a JSON object that represents a Citataion.
 * 
 * @param string $mods
 *   A MODS Document.
 * @param mixed $key within the array.
 *   The id of the citation object to create.
 * 
 * @return string
 *   A JSON encoded string, that represents the Citataion.
 */
function convert_mods_to_citeproc_jsons_escape(&$item, $key) {
  if (is_string($item)) {
    $item = htmlentities($item, ENT_QUOTES, 'UTF-8');
  }
}

function add_mods_namespace(SimpleXMLElement &$mods) {
  static $used_namespace = NULL;
  static $mods_namespace = 'http://www.loc.gov/mods/v3';

  $namespaces = $mods->getNamespaces();

  if (is_null($used_namespace)) {
    if (array_search($mods_namespace, $namespaces) !== FALSE) { //The namespace is there; possibly default, though
      $used_namespace = $mods_namespace;
    }
    else {
      $used_namespace = '';
    }
  }

  if (array_key_exists('mods', $namespaces) === FALSE) {
    $mods->registerXPathNamespace('mods', $used_namespace);
  }
}

/**
 * Coerce data into the proper JSON-like format, recursively.
 */
function _citeproc_array_to_object($in) {
  if (!is_array($in)) {
    return $in;
  }
  elseif (count($in) > 0 && count(array_filter(array_keys($in), 'is_int')) == count($in)) {
    foreach ($in as &$value) {
      $value = _citeproc_array_to_object($value);
    }
    return $in;
  }
  elseif (count($in) > 0 && count(array_filter(array_keys($in), 'is_string')) == count($in)) {
    $obj = new stdClass();
    foreach ($in as $key=>$val) {
      $obj->$key = _citeproc_array_to_object($val);
    }
    return $obj;
  }
  else {
    return FALSE;
  }
}

function convert_mods_to_citeproc_jsons($mods_in) {
  /**
   * FROM HERE ON IN, WE'RE DOING XPATH QUERIES AND POPULATING CSL VARIABLES.
   * STARTING WITH TITLE, THEN FOLLOWING IN MOSTLY ALPHABETICAL ORDER.
   */
  $mods = NULL;
  if ($mods_in instanceof DOMNode) {
    $mods = simplexml_import_dom($mods_in);
  }
  else {
    try {
      $mods = simplexml_load_string($mods_in);
    } catch (Exception $e) {
      watchdog('citeproc', t('Got exception while parsing.  Message: !msg Errors: !error', array(
            '!msg' => $e->getMessage(),
            '!error' => libxml_get_errors())));
      return array();
    }
  }
  if ($mods instanceof SimpleXMLElement) {
    add_mods_namespace($mods);
    $names = convert_mods_to_citeproc_json_names($mods); // Merge with main object
    $dates = convert_mods_to_citeproc_json_dates($mods);
    $type = convert_mods_to_citeproc_json_type($mods);
    $parented = "'$type'='book' or '$type'='chapter' or '$type'='article-journal'";
    $output = array_merge(array(
        'title' => convert_mods_to_citeproc_json_title($mods),
        'abstract' => convert_mods_to_citeproc_json_query($mods, '/mods:mods/mods:abstract'),
        'call-number' => convert_mods_to_citeproc_json_query($mods, '/mods:mods/mods:classification'),
      // nigel: 
      //'collection-title' => convert_mods_to_citeproc_json_query($mods, '/mods:mods/mods:relatedItem[@type="series"]/mods:titleInfo[not(@type)]/mods:title'),
        'collection-title' => convert_mods_to_citeproc_json_query($mods, "/mods:mods/mods:relatedItem[@type='series']/mods:titleInfo/mods:title | 
            /mods:mods[not($parented)]/mods:relatedItem[@type='host']/mods:titleInfo/mods:title" 
            //." | /mods:mods[$parented]/mods:relatedItem[@type='host']/mods:relatedItem[@type='host']/mods:titleInfo/mods:title" //Not wanted for books and chapters (Not applicable for journal articles?)
        ),
      // nigel:
      //'container-title' => convert_mods_to_citeproc_json_query($mods, '/mods:mods/mods:relatedItem[@type="host"]/mods:titleInfo[not(@type)]/mods:title'),
        'container-title' => convert_mods_to_citeproc_json_query($mods, "/mods:mods[$parented]/mods:relatedItem[@type='host']/mods:titleInfo/mods:title"),
        'DOI' => convert_mods_to_citeproc_json_query($mods, '/mods:mods/mods:identifier[@type="doi"]'),
        'edition' => convert_mods_to_citeproc_json_query($mods, '/mods:mods/mods:originInfo/mods:edition'),
        'event' => convert_mods_to_citeproc_json_event($mods),
        'event-place' => convert_mods_to_citeproc_json_event_place($mods),
        // nigel: 
        //'genre' => convert_mods_to_citeproc_json_genre($mods),
        //adam: 
        //'genre' => convert_mods_to_citeproc_json_query($mods, '/mods:mods/mods:relatedItem[@type="host"]/mods:genre[@authority="marcgt"]'),
        'ISBN' => convert_mods_to_citeproc_json_query($mods, '/mods:mods/mods:identifier[@type="isbn"]'),
        'volume' => (int) convert_mods_to_citeproc_json_query($mods, '/mods:mods/mods:part/mods:detail[@type="volume"]/mods:number'),
        'issue' => (int) convert_mods_to_citeproc_json_query($mods, '/mods:mods/mods:part/mods:detail[@type="issue"]/mods:number'),
        'note' => convert_mods_to_citeproc_json_note($mods),
        'number' => convert_mods_to_citeproc_json_query($mods, '/mods:mods/mods:relatedItem[@type="series"]/mods:titleInfo/mods:partNumber'),
        'page' => convert_mods_to_citeproc_json_page($mods),
      // nigel: 
      //'publisher' => convert_mods_to_citeproc_json_query($mods, '/mods:mods/mods:originInfo/mods:publisher'),
        'publisher' => convert_mods_to_citeproc_json_query($mods, '/mods:mods/mods:originInfo/mods:publisher | /mods:mods/mods:relatedItem[@type="host"]/mods:originInfo/mods:publisher'),
      // nigel: 
      //'publisher-place' => convert_mods_to_citeproc_json_query($mods, '/mods:mods/mods:originInfo/mods:place/mods:placeTerm'),
        'publisher-place' => convert_mods_to_citeproc_json_query($mods, '(/mods:mods/mods:originInfo/mods:place/mods:placeTerm[@type="text"] | /mods:mods/mods:relatedItem[@type="host"]/mods:originInfo/mods:place/mods:placeTerm[@type="text"])[1]'),
      // adam: 
      //'URL' => convert_mods_to_citeproc_json_query($mods, '/mods:mods/mods:location/mods:url'),
        'URL' => convert_mods_to_citeproc_json_query($mods, '/mods:mods/mods:location/mods:url'),
      // nigel / adam synthesis:
        'number-pmid' => convert_mods_to_citeproc_json_query($mods, '/mods:mods/mods:identifier[@type="pmid"] | /mods:mods/mods:identifier[@type="accession"]'),
        'number-pmcid' => convert_mods_to_citeproc_json_query($mods, '/mods:mods/mods:identifier[@type="pmcid"] | /mods:mods/mods:identifier[@type="pmc"]'),
        'number-nihmsid' => convert_mods_to_citeproc_json_query($mods, '/mods:mods/mods:identifier[@type="nihmsid"] | /mods:mods/mods:identifier[@type="mid"]'),
      //'type' => $type //adam
        'type' => convert_mods_to_citeproc_json_genre($mods)
      ), 
      $names, 
      $dates
    );
    return $output;
  }
  else {
    watchdog('citeproc', 'Not a SimpleXMLElement!');
    return array();
  }
}

/**
 *
 * @param type $mods 
 */
function convert_mods_to_citeproc_json_genre($mods) {
  $genre = convert_mods_to_citeproc_json_query($mods, '/mods:mods/mods:genre');
  switch (trim($genre)) {
    case 'journal article':
      return 'article-journal';
    case 'book chapter':
    case 'book section':
      return 'chapter';
    case 'book':
      return 'book';
    default:
      return 'article-journal';
  }
}

/**
 * Gets the title property for the Citation.
 * 
 * There may be multiple titles, and relying on the title[@type] is not a wholly 
 * relable method of determining the best title.  MOST OFTEN THERE WILL ONLY BE ONE.
 * My answer is to take the *longest*. 
 * 
 * @param SimpleXMLElement $mods
 *   A MODS document.
 * 
 * @return string
 *   The title property for the Citation.
 */
function convert_mods_to_citeproc_json_title(SimpleXMLElement $mods) {
  $output = '';
  add_mods_namespace($mods);
  $titles = $mods->xpath("/mods:mods/mods:titleInfo/mods:title");

  if (!empty($titles)) {
    while (list($num, $node) = each($titles)) {
      add_mods_namespace($node);
      $title = (string) $node;
      $subtitle = convert_mods_to_citeproc_json_query($node, '../mods:subTitle');
      $nonSort = convert_mods_to_citeproc_json_query($node, '../mods:nonSort');
      $title = !empty($subtitle) ? "$title: $subtitle" : $title;
      $title = !empty($nonSort) ? "$nonSort $title" : $title;
      $output = strlen($title) > strlen($output) ? $title : $output; // Choose the longest title.
    }
  }
  return!empty($output) ? $output : NULL;
}

/**
 * Gets the event property for the Citation.
 * 
 * @param SimpleXMLElement $mods
 *   A MODS document.
 * 
 * @return string
 *   The event property for the Citation.
 */
function convert_mods_to_citeproc_json_event(SimpleXMLElement $mods) {
  $property = convert_mods_to_citeproc_json_query($mods, '/mods:mods[mods:genre[@authority="marcgt"][text()="conference publication"]]/mods:relatedItem/mods:titleInfo/mods:title');
  if (isset($property)) { // marcgt
    return $property;
  }
  else { // zotero
    return convert_mods_to_citeproc_json_query($mods, '/mods:mods[mods:genre[@authority="local"]="conferencePaper"]/mods:relatedItem/mods:titleInfo/mods:title');
  }
}

/**
 * Gets the event-place property for the Citation.
 * 
 * @param SimpleXMLElement $mods
 *   A MODS document.
 * 
 * @return string
 *   The event-place property for the Citation.
 */
function convert_mods_to_citeproc_json_event_place(SimpleXMLElement $mods) {
  $property = convert_mods_to_citeproc_json_query($mods, '/mods:mods[mods:genre[@authority="marcgt"][text()="conference publication"]]/mods:originInfo/mods:place/mods:placeTerm');
  if (isset($property)) { // marcgt
    return $property;
  }
  else { // zotero
    return convert_mods_to_citeproc_json_query($mods, '/mods:mods[mods:genre[@authority="local"][text()="conferencePaper"]]/mods:originInfo/mods:place/mods:placeTerm');
  }
}

/**
 * Gets the note property for the Citation.
 * 
 * KEYWORD seems to be useless to CSL
 * 
 * @param SimpleXMLElement $mods
 *   A MODS document.
 * 
 * @return string
 *   The note property for the Citation.
 */
function convert_mods_to_citeproc_json_note(SimpleXMLElement $mods) {
  $notes = $mods->xpath("/mods:mods/mods:note");
  $ouput = '';
  while (list($num, $note) = each($notes)) {
    $ouput .= $num + 1 . ". " . rtrim(strip_tags($note), '. ') . ".  ";
  }
  return!empty($ouput) ? $ouput : NULL;
}

/**
 * Gets the page property for the Citation.
 * 
 * @param SimpleXMLElement $mods
 *   A MODS document.
 * 
 * @return string
 *   The page property for the Citation.
 */
function convert_mods_to_citeproc_json_page(SimpleXMLElement $mods) {
  $output = NULL;
  $pages = $mods->xpath("/mods:mods/mods:part/mods:extent[@unit='pages']");
  // Note: "pages" is correct, but Zotero uses "page".
  if (empty($pages)) {
    $pages = $mods->xpath("/mods:mods/mods:part/mods:extent[@unit='page']");
  }
  if (isset($pages[0])) {
    if (!empty($pages[0]->total)) {
      $output = (string) $pages[0]->total;
    }
    elseif (!empty($pages[0]->list)) {
      $output = (string) $pages[0]->list;
    }
    elseif (!empty($pages[0]->start)) {
      $output = (string) $pages[0]->start;
      if (!empty($pages[0]->end))
        $output .= "-" . $pages[0]->end;
    }
  }
  return $output;
}

/**
 *  Map from an EndNote 'ref-type' to a CSL type.  (Seems to map better than marcgt)
 *  ('ref-type' were added manually to the MODS as top-level genre with the authority 
 *  set to 'endnote')
 *  TODO:  Map all default types
 *
 */
function _get_endnote_type(SimpleXMLElement $mods) {
  $output = '';
  
  $type_map = array(
    'Book Section' => 'chapter',
    'Book' => 'book',
    'Edited Book' => 'book',
    'Journal Article' => 'article-journal',
    'Working Paper' => 'chapter', //XXX:  This is a custom one for FJM...  Might need a hook?
    'Thesis' => 'thesis'
  );
  $types = $mods->xpath("/mods:mods/mods:genre[@authority='endnote']");
  
  if (!empty($types)) {
    $type = $types[0];
    $output = $type_map[(string)$type];
  }
  
  return $output;
}
function _get_marcgt_type(SimpleXMLElement $mods) {
  $output = '';
  $type_marcgt = $mods->xpath("/mods:mods/mods:genre[@authority='marcgt']");
  if (!empty($type_marcgt)) {
    $interim_type =& $type_marcgt[0];
    add_mods_namespace($interim_type);
    if (!strcasecmp($interim_type, 'book')) {
      $host_titles = $interim_type->xpath("../mods:relatedItem[@type='host']/mods:titleInfo/mods:title");
      if (!empty($host_titles)) {
        // This is but a chapter in a book
        $output = 'chapter';
      }
      else {
        $output = 'book';
      }
    }
    else {
      $output = marcgt_to_csl((string)$interim_type);
    }
  }
  return $output;
}
function _get_related_marcgt_type(SimpleXMLElement $mods) {
  $output = '';
  
  $type_marcgt_related = $mods->xpath("/mods:mods/mods:relatedItem/mods:genre[@authority='marcgt']");
  if (!empty($type_marcgt_related)) {
    $interim_type = (string) $type_marcgt_related[0];
    if (!strcasecmp($interim_type, 'book')) {
      $output = 'chapter';
    }
    else {
      $output = marcgt_to_csl($interim_type);
    }
  }
  
  return $output;
}
function _get_other_types(SimpleXMLElement $mods) {
  $output = '';
  
  $types_local_auth = $mods->xpath("/mods:mods/mods:genre[not(@authority='marcgt' or @authority='endnote')]");
  while (empty($output) && list( $num, $type ) = each($types_local_auth)) {
    $interim_type = (string) $type;
    $output = mods_genre_to_csl_type($interim_type);
  }
  
  return $output;
}
/**
 * Gets the type property for the Citation.
 * 
 * @param SimpleXMLElement $mods
 *   A MODS document.
 * 
 * @return string
 *   The type property for the Citation.
 */
function convert_mods_to_citeproc_json_type(SimpleXMLElement $mods) {
  /**
   * @auth='endnote'-- seems to map to the CSL types easier
   * @auth='marcgt' -- marcgt should be the preferred authority
   * @auth='local'  -- actually better at differentiating some types
   * not(@auth)     -- unauthoritative types from Bibutils
   *
   *  genre == 'book' is especially difficult
   *  //mods/relatedItem[@type='host']/genre[@authority='marcgt'] == 'book' means "Chapter"
   *  //mods/genre[@authority='marcgt'] == 'book' means "Book" 
   *  *UNLESS* //mods/relatedItem[type='host']/titleInfo/title exists
   *  *OR*     //mods/genre[@authority='local'] == 'bookSection'
   */
  module_load_include('inc', 'citeproc', 'generators/mods_csl_type_conversion');
  module_load_include('inc', 'citeproc', 'generators/marcrelator_conversion');
  
  // First try:  Let's map the a provided Endnote genre...
  $output = _get_endnote_type($mods);
  
  // Second try: item's local marcgt genre.
  if (empty($output)) {
    $output = _get_marcgt_type($mods);
  }
  
  // Third try: item's parent marcgt genre (often applies to the original item itself).
  if (empty($output)) {
    $output = _get_related_marcgt_type($mods);
  }
  // Last try: other authority types (most likely Zotero local)
  if (empty($output)) {
    $output = _get_other_types($mods);
  }
  return $output;
}

/**
 * Gets the type property for the Citation.
 * 
 * @param SimpleXMLElement $mods
 *   A MODS document.
 * 
 * @return string
 *   The type property for the Citation.
 */
function convert_mods_to_citeproc_json_names(SimpleXMLElement $mods) {
  /**
   * There are a number of name-type vars which may be populated.
   * We will concern ourselves with the following:
   *  1. Author
   *  2. Editor
   *  3. Translator
   * Note: There is no CSL var for "contributor", we will treat them as additional authors.
   * Note: Each name may be either "corporate" or "given name / family name"
   * Note: As it's unlikely we'll have participles, suffixes, etc properly parsed out, we
   *       will always pass the ("parse-names" : "true") flag with personal names.
   * 
   * mods namepart types (given, family) correspond to citeproc elements,
   * however, more precise mods elements (nonsort, etc.) do not.
   * TODO: make all name handling better.
   * 
   * NAME(s) of RELATED ITEMS (host, series)
   * Zotero vs Bibutils do this a bit differently, but in bibutils it's common 
   * for the editor of a book (containing a chapter, which is our bibliographic item)
   * to be associated with the relatedItem(host).  
   * Also it's a shot in the dark, but relatedItem(series)->name->role->roleTerm=editor is plausible.
   *
   * Note also this section is *highly* repetitive of the section above and this should probably
   * be generalized, but for now a strict procedural reckoning will have to suffice.  The difference
   * is in the very last section, where the appropriate cs:names type is specified. 
   */
  $queries = array(
    0 => array(
      '/mods:mods/mods:name', // Path
      'author', // Default Role
      array(// Valid Roles
        'editor' => 'editor',
        'translator' => 'translator',
        'interviewer' => 'interviewer',
        'composer' => 'composer',
        'original' => 'original-author',
        'recipient' => 'recipient',
        'author' => 'author'
      )
    ),
    1 => array(
      '/mods:mods/mods:relatedItem[@type="host"]/mods:name',
      'container-author',
      array(
        'editor' => 'editor',
        'translator' => 'translator',
        'author' => 'container-author'
      )
    ),
    2 => array(
      '/mods:mods/mods:relatedItem[@type="series"]/mods:name',
      NULL,
      array(
        'editor' => 'collection-editor'
      )
    )
  );
  $output = array();
  foreach ($queries as $query) {
    list($path, $default_role, $valid_roles) = $query;
    $names = $mods->xpath($path);
    if (!empty($names)) {
      foreach ($names as $name) {
        add_mods_namespace($name);
        $role = convert_mods_to_citeproc_json_name_role($name, $valid_roles, $default_role);
        if ($role !== FALSE) {
          $output[$role][] = convert_mods_to_citeproc_json_name($name);
        }
      }
    }
  }
  return $output;
}

/**
 * Gets the array repersentation of the javascript Citation's name properties.
 * 
 * @param SimpleXMLElement $name
 *   A name element.
 * 
 * @return array
 *   An array that embodies the name properties of a Citation javascript object.
 */
function convert_mods_to_citeproc_json_name(SimpleXMLElement $name) {
  $type = (string) $name->attributes()->type;
  $output = ($type == 'personal') ?
      convert_mods_to_citeproc_json_name_personal($name) :
      convert_mods_to_citeproc_json_name_corporate($name);
  return array_map('trim', $output);
}

/**
 * Gets the array repersentation of the javascript Citation's personal name properties.
 * 
 * @param SimpleXMLElement $name
 *   A name element.
 * 
 * @return array
 *   An array that embodies the name properties of a Citation javascript object.
 */
function convert_mods_to_citeproc_json_name_personal(SimpleXMLElement $name) {
  $output = array();
  $nameParts = $name->xpath("mods:namePart");
  foreach ($nameParts as $namePart) {
    $type = (string) $namePart->attributes()->type;
    $content = (string) $namePart;
    $content .= (strlen($content) == 1) ? '. ' : ' '; // Not sure why this is here...
    $output[$type] = isset($output[$type]) ?
        $output[$type] . $content :
        $content;
  }
  $output['parse-names'] = "true"; // add the citeproc-js "parse-names" flag.
  return $output;
}

/**
 * Gets the array repersentation of the javascript Citation's corporate name properties.
 * 
 * @param SimpleXMLElement $name
 *   A name element.
 * 
 * @return array
 *   An array that embodies the name properties of a Citation javascript object.
 */
function convert_mods_to_citeproc_json_name_corporate(SimpleXMLElement $name) {
  $output = array();
  $nameParts = $name->xpath("mods:namePart");
  foreach ($nameParts as $namePart) {
    $content = (string) $namePart . ' ';
    $output['literal'] = isset($output['literal']) ? $output['literal'] . $content : $content;
  }
  return $output;
}

/**
 * Gets the role for the given name element.
 * 
 * If no role can be determined it returns 'author' as a default.
 * 
 * @param SimpleXMLElement $name
 *   A MODS name element.
 * @param array $valid_roles
 *   A map of mods role names to their citeproc equivalents.
 * @param string $default_role
 *   The role to use if a valid role is not found.
 * 
 * @return string
 *   Gets the role of the given name.
 */
function convert_mods_to_citeproc_json_name_role(SimpleXMLElement $name, array $valid_roles, $default_role) {
  module_load_include('inc', 'citeproc', 'generators/marcrelator_conversion');
  if (isset($name->role)) {
    $role = strtolower((string) $name->role->roleTerm);
    if (isset($name->role->roleTerm)) {
      $role_authority = (string) $name->role->roleTerm->attributes()->authority;
      $role_type = (string) $name->role->roleTerm->attributes()->type;
      if ($role_authority == 'marcrelator' && $role_type == 'code') {
        $role = marcrelator_code_to_term($role);
      }
    }
    return array_key_exists($role, $valid_roles) ? $valid_roles[$role] : false;
  }
  return false;
}

function _try_parse_date(&$output, $date_string) {
  //FIXME:  Need a more reliable way to get the date info...
  if (FALSE && ($parsed = date_parse($date_string))) {
    //dd($parsed, 'Parsed date');
    $output['date-parts'] = array(
      array(
        $parsed['year'],
        $parsed['month'],
        $parsed['day']
      )
    );
  }
  else {
    $output['raw'] = $date_string;
  }
}

/**
 * Get the dates.
 */
function convert_mods_to_citeproc_json_dates(SimpleXMLElement $mods) {
  $output = array();
  // Nigel's way
  $date = convert_mods_to_citeproc_json_query($mods, "/mods:mods/mods:originInfo/mods:dateCaptured[@encoding = 'iso8601']");
  if (!empty($date)) {
    $date_time = new DateTime($date);
    $output['accessed']['date-parts'] = array(array(intval($date_time->format('Y')), intval($date_time->format('m')), intval($date_time->format('d'))));
  }
  else {
    $date = convert_mods_to_citeproc_json_query($mods, "/mods:mods/mods:originInfo/mods:dateCaptured");
    $output['accessed']['raw'] = $date;
  }
  $date = convert_mods_to_citeproc_json_query($mods, "/mods:mods/mods:originInfo/mods:dateIssued[@encoding = 'iso8601']");
  if (!empty($date)) {
    $date_time = new DateTime($date);
    $output['issued']['date-parts'] = array(array(intval($date_time->format('Y')), intval($date_time->format('m')), intval($date_time->format('d'))));
  }
  else {
    $date = convert_mods_to_citeproc_json_query($mods, "/mods:mods/mods:originInfo/mods:dateIssued");
    $output['issued']['raw'] = $date;
  }
  $date = convert_mods_to_citeproc_json_query($mods, "/mods:mods/mods:originInfo/mods:dateCreated[@encoding = 'iso8601']");
  if (!empty($date)) {
    $date_time = new DateTime($date);
    $output['issued']['date-parts'] = array(array(intval($date_time->format('Y')), intval($date_time->format('m')), intval($date_time->format('d'))));
  }
  else {
    $date = convert_mods_to_citeproc_json_query($mods, "/mods:mods/mods:originInfo/mods:dateCreated");
    $output['issued']['raw'] = $date;
  }
  /*
  // Adam's way
  $date_captured = convert_mods_to_citeproc_json_query($mods, "/mods:mods/mods:originInfo/mods:dateCaptured");
  if (!empty($date_captured)) {
    _try_parse_date($output['accessed'], $date_captured);
  }
  
  $date_issued = convert_mods_to_citeproc_json_query($mods, "/mods:mods/mods:originInfo/mods:dateIssued");
  if (!empty($date_issued)) {
    _try_parse_date($output['issued'], $date_issued);
  }
  
  $date_created = convert_mods_to_citeproc_json_query($mods, "/mods:mods/mods:originInfo/mods:dateCreated");
  if (!empty($date_created) && empty($output['issued'])) {
    _try_parse_date($output['issued'], $date_created);
  }
  */
  return $output;
}

/**
 * Gets the first result of the provided $path.
 * 
 * @param SimpleXMLElement $mods
 *   A MODS document.
 * @param string $path
 *   An XPath.
 * 
 * @return string
 *   The first results of the query if found NULL otherwise.
 */
function convert_mods_to_citeproc_json_query(SimpleXMLElement $mods, $path) {
  $property = $mods->xpath($path);
  return!empty($property) ? (string) $property[0] : NULL;
}

/**
 * This function will convert mods to citeproc_json
 * @deprecated This function has been broken up
 * @see convert_mods_to_citeproc_jsons
 * @param $mods
 *   The mods to convert to citeproc_json for citation purposes
 * @param $item_id
 *   The id to insert into the json object, needs to be unique for the page
 * @return
 *   The json formated citation data
 */
function convert_mods_to_citeproc_json($mods, $item_id) {
  module_load_include('inc', 'citeproc', 'generators/mods_csl_type_conversion');
  module_load_include('inc', 'citeproc', 'generators/marcrelator_conversion');

  // Beginnings of a CSL json data structure.
  $csl_data = array();

  //insert the item id for use by citeproc
  $csl_data['id'] = $item_id;

  $xml = new SimpleXMLElement($mods);
  //NOTE:  To deal with un-namespaced MODS (which isn't really MODS, right?), you
  //  might try checking if the 'mods' prefix is bound: if it is not, then 
  //  try binding to an empty string?  Dunno...  Something like:
  add_mods_namespace($xml);
  //$mods_namespace = $xml->getNamespaces()['mods'];


  /**
    FROM HERE ON IN, WE'RE DOING XPATH QUERIES AND POPULATING CSL VARIABLES.
    STARTING WITH TITLE, THEN FOLLOWING IN MOSTLY ALPHABETICAL ORDER.
   */
  // TITLES
  // There may be multiple titles, and relying on the title[@type] is not a wholly
  // relable method of determining the best title.  MOST OFTEN THERE WILL ONLY BE ONE.
  // My answer is to take the *longest*. 

  $titles = $xml->xpath("//mods:mods/mods:titleInfo/mods:title");


  if (!empty($titles))
    while (list($num, $node) = each($titles)) {
      //$node->registerXPathNamespace('mods', $mods_namespace);
      add_mods_namespace($node);

      $title = (string) $node;
      $subtitle = $node->xpath("../mods:subTitle");
      if (!empty($subtitle)) {
        $title .= ": " . $subtitle[0];
      }

      $nonSort = $node->xpath("../mods:nonSort");
      if (!empty($nonSort)) {
        $title = $nonSort[0] . " " . $title;
      }

      if (isset($csl_data['title'])) {
        if (strlen($title) > strlen($csl_data['title'])) {
          $csl_data['title'] = $title;
        }
      }
      else {//first time title is set
        $csl_data['title'] = $title;
      }
    }

  // ABSTRACT
  $abstracts = $xml->xpath("//mods:mods/mods:abstract");
  if (!empty($abstracts[0])) {
    $csl_data['abstract'] = (string) $abstracts[0];
  }

  // CALL NUMBER
  $call_numbers = $xml->xpath("//mods:mods/mods:classification");
  if (!empty($call_numbers)) {
    $csl_data['call-number'] = (string) $call_numbers[0];
  }

  // COLLECTION TITLE
  $collection_titles = $xml->xpath("//mods:mods/mods:relatedItem[@type='series']/mods:titleInfo/mods:title");
  if (!empty($collection_titles)) {
    $csl_data['collection-title'] = (string) $collection_titles[0];
  }

  // CONTAINER TITLE
  $container_titles = $xml->xpath("//mods:mods/mods:relatedItem[@type='host']/mods:titleInfo/mods:title");
  if (!empty($container_titles)) {
    $csl_data['container-title'] = (string) $container_titles[0];
  }

  // DOI
  $dois = $xml->xpath("//mods:mods/mods:identifier[@type='doi']");
  if (!empty($dois)) {
    $csl_data['DOI'] = (string) $dois[0];
  }

  // EDITION
  $editions = $xml->xpath("//mods:mods/mods:originInfo/mods:edition/text()");
  if (!empty($editions)) {
    $csl_data['edition'] = (string) $editions[0];
  }

  // EVENT
  // (1. marcgt)
  $events = $xml->xpath("//mods:mods//mods:genre[@authority='marcgt'][text() = 'conference publication']/ancestor::mods:mods[1]/mods:relatedItem/mods:titleInfo/mods:title/text()");
  if (!empty($events)) {
    $csl_data['event'] = (string) $events[0];
  }
  else {
    // (2. zotero)
    $zotero_events = $xml->xpath("//mods:mods//mods:genre[@authority='local'][text() = 'conferencePaper']/ancestor::mods:mods[1]/mods:relatedItem/mods:titleInfo/mods:title/text()");
    if (!empty($zotero_events)) {
      $csl_data['event'] = (string) $zotero_events[0];
    }
  }

  // EVENT PLACE
  // (1. marcgt)
  $event_places = $xml->xpath("//mods:mods//mods:genre[@authority='marcgt'][text() = 'conference publication']/ancestor::mods:mods[1]/mods:originInfo/mods:place/mods:placeTerm/text()");
  if (!empty($event_places)) {
    $csl_data['event-place'] = (string) $event_places[0];
  }
  else {
    $zotero_event_places = $xml->xpath("//mods:mods//mods:genre[@authority='local'][text() ='conferencePaper']/ancestor::mods:mods[1]/mods:originInfo/mods:place/mods:placeTerm/text()");
    if (!empty($zotero_event_places))
      $csl_data['event-place'] = (string) $zotero_event_places[0];
  }

  // GENRE (type of resource)
  $genres = $xml->xpath("//mods:mods//mods:typeOfResource/text()");
  if (!empty($genres)) {
    $csl_data['genre'] = (string) $genres[0];
  }

  // ISBN
  $isbns = $xml->xpath("//mods:mods/mods:identifier[@type='isbn']/text()");
  if (!empty($isbns)) {
    $csl_data['ISBN'] = (string) $isbns[0];
  }

  // VOLUME (counterpart to issue)
  $volumes = $xml->xpath("//mods:mods//mods:part/mods:detail[@type='volume']/*/text()");
  if (!empty($volumes)) {
    $csl_data['volume'] = (int) $volumes[0];
  }

  // ISSUE (counterpart to volume)
  $issues = $xml->xpath("//mods:mods//mods:part/mods:detail[@type='issue']/*/text()");
  if (!empty($issues)) {
    $csl_data['issue'] = (int) $issues[0];
  }

  // KEYWORD seems to be useless to CSL
  // NOTE
  $notes = $xml->xpath("//mods:mods/mods:note");
  $notestr = "";
  while (list( $num, $note ) = each($notes)) {
    $notestr .= $num + 1 . ". " . rtrim(strip_tags($note), '. ') . ".  ";
  }
  if (!empty($notestr)) {
    $csl_data['note'] = $notestr;
  }

  // NUMBER (mainly series number, rarely used)
  $numbers = $xml->xpath("//mods:mods//mods:relatedItem[@type='series']/mods:titleInfo/mods:partNumber/text()");
  if (!empty($numbers)) {
    $csl_data['number'] = $numbers[0];
  }

  // PAGE(s)
  $pages = $xml->xpath("//mods:mods//mods:part/mods:extent[@unit='pages']");

  // Note: "pages" is correct, but Zotero uses "page".
  if (empty($pages)) {
    $pages = $xml->xpath("//mods:mods//mods:part/mods:extent[@unit='page']");
  }

  if (!empty($pages[0]->total)) {
    $csl_data['page'] = (string) $pages[0]->total;
  }
  elseif (!empty($pages[0]->list)) {
    $csl_data['page'] = (string) $pages[0]->list;
  }
  elseif (!empty($pages[0]->start)) {
    $csl_data['page'] = (string) $pages[0]->start;
    if (!empty($pages[0]->end))
      $csl_data['page'] .= "-" . $pages[0]->end;
  }

  // PUBLISHER
  $publishers = $xml->xpath("//mods:mods//mods:originInfo/mods:publisher/text()");
  if (!empty($publishers)) {
    $csl_data['publisher'] = (string) $publishers[0];
  }

  // PUBLISHER PLACE
  $pub_places = $xml->xpath("//mods:mods//mods:originInfo/mods:place/mods:placeTerm/text()");
  if (!empty($pub_places)) {
    $csl_data['publisher-place'] = (string) $pub_places[0];
  }

  // URL
  $urls = $xml->xpath("//mods:mods/mods:location/mods:url");
  if (!empty($urls)) {
    $csl_data['URL'] = (string) $urls[0];
  }


  // TYPE -- this is a big one.
  //    @auth='marcgt' -- marcgt should be the preferred authority
  //    @auth='local'  -- actually better at differentiating some types
  //    not(@auth)     -- unauthoritative types from Bibutils
  //
  //    genre == 'book' is especially difficult
  //      //mods/relatedItem[@type='host']/genre[@authority='marcgt'] == 'book' means "Chapter"
  //      //mods/genre[@authority='marcgt'] == 'book' means "Book" 
  //         *UNLESS* //mods/relatedItem[type='host']/titleInfo/title exists
  //         *OR*     //mods/genre[@authority='local'] == 'bookSection'
  //
  
  // First try: item's local marcgt genre.
  $type_marcgt = $xml->xpath("//mods:mods/mods:genre[@authority='marcgt']/text()");
  if (!empty($type_marcgt)) {
    $interim_type = (string) $type_marcgt[0];

    if (!strcasecmp($interim_type, 'book')) {
      //$type_marcgt[0]->registerXPathNamespace('mods', $mods_namespace);
      add_mods_namespace($type_marcgt[0]);

      $host_titles = $type_marcgt[0]->xpath("../mods:relatedItem[@type='host']/mods:titleInfo/mods:title/text()");
      if (!empty($host_titles)) {
        // This is but a chapter in a book
        $csl_data['type'] = 'chapter';
      }
      else {
        $csl_data['type'] = 'book';
      }
    }
    else {
      $csl_data['type'] = marcgt_to_csl($interim_type);
    }

    $csl_type = marcgt_to_csl($interim_type);
  }

  // Second try: item's parent marcgt genre (often applies to the original item itself).
  if (empty($csl_data['type'])) {

    $type_marcgt_related = $xml->xpath("//mods:mods/mods:relatedItem/mods:genre[@authority='marcgt']/text()");
    if (!empty($type_marcgt_related)) {
      $interim_type = (string) $type_marcgt_related[0];

      if (!strcasecmp($interim_type, 'book')) {
        $csl_data['type'] = 'chapter';
      }
      else {
        $csl_data['type'] = marcgt_to_csl($interim_type);
      }
    }
  }

  // Third try: other authority types (most likely Zotero local)
  if (empty($csl_data['type'])) {

    $types_local_auth = $xml->xpath("//mods:mods//mods:genre[not(@authority='marcgt')]/text()");
    while (empty($csl_data['type']) && list( $num, $type ) = each($types_local_auth)) {
      $interim_type = (string) $type;
      $csl_data['type'] = mods_genre_to_csl_type($interim_type);
    }
  }

  // NAME(s) -- Another Biggie
  // There are a number of name-type vars which may be populated.
  // We will concern ourselves with the following:
  //  1. Author
  //  2. Editor
  //  3. Translator
  // Note: There is no CSL var for "contributor", we will treat them as additional authors.
  // Note: Each name may be either "corporate" or "given name / family name"
  // Note: As it's unlikely we'll have participles, suffixes, etc properly parsed out, we
  //       will always pass the ("parse-names" : "true") flag with personal names.

  $names = $xml->xpath("//mods:mods/mods:name");
  while (list( $num, $name ) = each($names)) {
    // print_r($name);
    $personal_corporate = (string) $name->attributes()->type;

    $role = strtolower((string) $name->role->roleTerm);
    $role_authority = isset($name->role->roleTerm) ?
        (string) $name->role->roleTerm->attributes()->authority :
        NULL;
    $role_type = (string) isset($name->role->roleTerm) ? $name->role->roleTerm->attributes()->type : '';
    if ($role_authority == 'marcrelator' && $role_type == 'code') {
      $role = marcrelator_code_to_term($role);
    }

    $csl_name = array();
    switch ($personal_corporate) {
      case 'personal':
        $nameParts = $name->xpath("./mods:namePart");
        while (list( $namePart_num, $namePart ) = each($nameParts)) {
          // mods namepart types (given, family) correspond to citeproc elements,
          // however, more precise mods elements (nonsort, etc.) do not.
          // TODO: make all name handling better.
          $namePart_type = (string) $namePart->attributes()->type;
          $namePart_string = (string) $namePart;
          if (strlen($namePart_string) == 1) {
            $namePart_string .= ".";
          }
          $string = isset($csl_name[$namePart_type]) ? $csl_name[$namePart_type] : '';
          $csl_name[$namePart_type] = $string . $namePart_string . " ";
        }
        // trim extra whitespace from each array element
        array_walk($csl_name, create_function('&$val', '$val = trim($val);'));
        // add the citeproc-js "parse-names" flag.
        $csl_name['parse-names'] = "true";
        break;
      case 'corporate':
      default:
        $nameParts = $name->xpath("./mods:namePart");
        while (list( $namePart_num, $namePart ) = each($nameParts)) {
          $namePart_string = (string) $namePart;
          $literal = isset($csl_name['literal']) ? $csl_name['literal'] : '';
          $csl_name['literal'] = $literal . $namePart_string . " ";
        }
        $csl_name['literal'] = trim($csl_name['literal']);
        break;
    }

    switch ($role) {
      case 'editor':
        $csl_data['editor'][] = $csl_name;
        break;
      case 'translator':
        $csl_data['translator'][] = $csl_name;
        break;
      case 'interviewer':
        $csl_data['interviewer'][] = $csl_name;
        break;
      case 'composer':
        $csl_data['composer'][] = $csl_name;
        break;
      case 'originator':
        $csl_data['original-author'][] = $csl_name;
        break;
      case 'recipient':
        $csl_data['recipient'][] = $csl_name;
        break;
      case 'author':
      default:
        $csl_data['author'][] = $csl_name;
        break;
    }
  }

  // NAME(s) of RELATED ITEMS (host, series)
  // Zotero vs Bibutils do this a bit differently, but in bibutils it's common 
  // for the editor of a book (containing a chapter, which is our bibliographic item)
  // to be associated with the relatedItem(host).  
  // Also it's a shot in the dark, but relatedItem(series)->name->role->roleTerm=editor is plausible.
  //
  // Note also this section is *highly* repetitive of the section above and this should probably
  // be generalized, but for now a strict procedural reckoning will have to suffice.  The difference
  // is in the very last section, where the appropriate cs:names type is specified.

  $hostNames = $xml->xpath("//mods:mods/mods:relatedItem[@type='host']/mods:name");
  if (!empty($hostNames))
    while (list( $num, $name ) = each($hostNames)) {
      // print_r($name);
      $personal_corporate = (string) $name->attributes()->type;

      $role = strtolower((string) $name->role->roleTerm);
      $role_authority = (string) $name->role->roleTerm->attributes()->authority;
      $role_type = (string) $name->role->roleTerm->attributes()->type;
      if ($role_authority == 'marcrelator' && $role_type == 'code') {
        $role = marcrelator_code_to_term($role);
      }

      $csl_name = array();
      switch ($personal_corporate) {
        case 'personal':
          $nameParts = $name->xpath("./mods:namePart");
          while (list( $namePart_num, $namePart ) = each($nameParts)) {
            // mods namepart types (given, family) correspond to citeproc elements,
            // however, more precise mods elements (nonsort, etc.) do not.
            // TODO: make all name handling better.
            $namePart_type = (string) $namePart->attributes()->type;
            $namePart_string = (string) $namePart;
            if (strlen($namePart_string) == 1) {
              $namePart_string .= ".";
            }
            $csl_name[$namePart_type] .= $namePart_string . " ";
          }
          // trim extra whitespace from each array element
          array_walk($csl_name, create_function('&$val', '$val = trim($val);'));
          // add the citeproc-js "parse-names" flag.
          $csl_name['parse-names'] = "true";
          break;
        case 'corporate':
        default:
          $nameParts = $name->xpath("./mods:namePart");
          while (list( $namePart_num, $namePart ) = each($nameParts)) {
            $namePart_string = (string) $namePart;
            $csl_name['literal'] .= $namePart_string . " ";
          }
          $csl_name['literal'] = trim($csl_name['literal']);
          break;
      }

      switch ($role) {
        case 'editor':
          $csl_data['editor'][] = $csl_name;
          break;
        case 'translator':
          $csl_data['translator'][] = $csl_name;
          break;
        case 'author':
        default:
          $csl_data['container-author'][] = $csl_name;
          break;
      }
    }

  $seriesNames = $xml->xpath("//mods:mods/mods:relatedItem[@type='series']/mods:name");
  if (!empty($seriesNames))
    while (list( $num, $name ) = each($seriesNames)) {
      // print_r($name);
      $personal_corporate = (string) $name->attributes()->type;

      $role = strtolower((string) $name->role->roleTerm);
      $role_authority = (string) $name->role->roleTerm->attributes()->authority;
      $role_type = (string) $name->role->roleTerm->attributes()->type;
      if ($role_authority == 'marcrelator' && $role_type == 'code') {
        $role = marcrelator_code_to_term($role);
      }

      $csl_name = array();
      switch ($personal_corporate) {
        case 'personal':
          $nameParts = $name->xpath("./mods:namePart");
          while (list( $namePart_num, $namePart ) = each($nameParts)) {
            // mods namepart types (given, family) correspond to citeproc elements,
            // however, more precise mods elements (nonsort, etc.) do not.
            // TODO: make all name handling better.
            $namePart_type = (string) $namePart->attributes()->type;
            $namePart_string = (string) $namePart;
            if (strlen($namePart_string) == 1) {
              $namePart_string .= ".";
            }
            $csl_name[$namePart_type] .= $namePart_string . " ";
          }
          // trim extra whitespace from each array element
          array_walk($csl_name, create_function('&$val', '$val = trim($val);'));
          // add the citeproc-js "parse-names" flag.
          $csl_name['parse-names'] = "true";
          break;
        case 'corporate':
        default:
          $nameParts = $name->xpath("./mods:namePart");
          while (list( $namePart_num, $namePart ) = each($nameParts)) {
            $namePart_string = (string) $namePart;
            $csl_name['literal'] .= $namePart_string . " ";
          }
          $csl_name['literal'] = trim($csl_name['literal']);
          break;
      }

      switch ($role) {
        case 'editor':
          $csl_data['collection-editor'][] = $csl_name;
          break;
        default:
          break;
      }
    }

  // DATES - yet another biggie
  // 1. Date Accessed
  // 2. Date Issued

  $date_captured = $xml->xpath("//mods:mods//mods:originInfo/mods:dateCaptured");
  if (!empty($date_captured)) {
    $csl_data['accessed']['raw'] = (string) $date_captured[0];
  }

  $date = $xml->xpath("//mods:mods//mods:originInfo/mods:dateIssued");
  if (!empty($date)) {
    $csl_data['issued']['raw'] = (string) $date[0];
  }

  $date = $xml->xpath("//mods:mods//mods:originInfo/mods:dateCreated");
  if (!empty($date) && empty($csl_data['issued'])) {
    $csl_data['issued']['raw'] = (string) $date[0];
  }


  //return json data
  return (json_encode($csl_data));
}
