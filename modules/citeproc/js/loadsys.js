/**
 * @file
 *   This javascript creates a sys object for the citeproc-js engine.  This object is meant to allow 
 *   access to abreviations, citation data and localization information that has been made available
 *   through the module's API.
 * @author 
 *   William Panting
 */

/**
 * Constructor
 */
var citeproc_sys = function(){
  return;
};

/**
 * This function will let the citeproc-js engine access to citation data set by the Drupal module
 * @param string id
 *   Identifier for the citaiton data
 */
citeproc_sys.prototype.retrieveItem = function(id){
  return Drupal.settings.citeproc.item[id];
};

/**
 * This function will let the citeproc-js engine access to localization data set by the Drupal module
 * @param string lang
 *   Identifier for the localization data
 */
citeproc_sys.prototype.retrieveLocale = function(lang){
  return Drupal.settings.citeproc.locale[lang];
};

/**
 * This function will let the citeproc-js engine access to abbreviation data set by the Drupal module
 * @param string name
 *   Identifier for the abbreviation data
 * @param string vartype
 *   Indicates which sub-section of abbreviation data to retrieve
 */
citeproc_sys.prototype.getAbbreviations = function(name, vartype){
  return Drupal.settings.citeproc.abbreviation[name][vartype];
};

/**
 * This function is not used by citeproc-js, but instead is used by the runcites.js to get a style for a render object.
 * 
 * @param string id
 *   Identifier for style.
 */
citeproc_sys.prototype.retrieveStyle = function(id) {
  return Drupal.settings.citeproc.style[id];
};

/**
 * This function is not used by citeproc-js, but instead is used by the runcites.js to the required info 
 * to render the bibliography.
 */
citeproc_sys.prototype.retrieveBibliographies = function() {
  return Drupal.settings.citeproc.bibliography;
};